#include "FFTProcessor.h"

#include <algorithm>
#include <cmath>

FFTProcessor::FFTProcessor()
{
}

void FFTProcessor::setAveragingAlpha(
    float alpha)
{
    m_averagingAlpha = std::clamp(
        alpha,
        0.01f,
        1.0f
        );

    /*
     * 1.00 = averaging off
     * 0.40 = fast
     * 0.20 = medium
     * 0.08 = slow
     *
     * Restart averaging whenever the setting changes.
     * The next FFT frame becomes the initial reference.
     */
    m_averagingInitialised = false;
}

void FFTProcessor::process(
    const HFSDR::IQBuffer& input,
    std::vector<float>& outputDb)
{
    const std::size_t n = input.size();

    if (n < 2) {
        outputDb.clear();
        return;
    }

    std::vector<std::complex<float>> data(
        input.samples().begin(),
        input.samples().end()
        );

    /*
     * Hann-window coherent-gain correction.
     *
     * A full-scale complex tone positioned exactly
     * on an FFT bin should read approximately 0 dBFS.
     */
    constexpr float pi =
        3.14159265358979323846f;

    float windowSum = 0.0f;

    for (std::size_t i = 0;
         i < n;
         ++i) {

        const float window =
            0.5f *
            (
                1.0f -
                std::cos(
                    2.0f *
                    pi *
                    static_cast<float>(i) /
                    static_cast<float>(n - 1)
                    )
                );

        data[i] *= window;
        windowSum += window;
    }

    fft(data);

    std::vector<float> currentSpectrum(
        n,
        -240.0f
        );

    const std::size_t half =
        n / 2;

    const float normalisation =
        windowSum > 0.0f
            ? windowSum
            : static_cast<float>(n);

    constexpr float minimumMagnitude =
        1.0e-12f;

    for (std::size_t i = 0;
         i < n;
         ++i) {

        /*
         * FFT shift:
         * negative frequencies on the left,
         * positive frequencies on the right.
         */
        const std::size_t shiftedIndex =
            (i + half) % n;

        const float magnitude =
            std::abs(
                data[shiftedIndex]
                ) /
            normalisation;

        currentSpectrum[i] =
            20.0f *
            std::log10(
                std::max(
                    magnitude,
                    minimumMagnitude
                    )
                );
    }

    /*
     * Restart averaging when:
     *
     * - averaging has just been enabled or changed;
     * - the FFT size changes because the spectrum
     *   span or Display DDC decimation changed.
     */
    if (!m_averagingInitialised ||
        m_averagedSpectrum.size() != n) {

        m_averagedSpectrum =
            currentSpectrum;

        m_averagingInitialised =
            true;
    } else {
        const float previousWeight =
            1.0f -
            m_averagingAlpha;

        for (std::size_t i = 0;
             i < n;
             ++i) {

            m_averagedSpectrum[i] =
                m_averagingAlpha *
                    currentSpectrum[i] +
                previousWeight *
                    m_averagedSpectrum[i];
        }
    }

    outputDb =
        m_averagedSpectrum;
}

void FFTProcessor::applyHannWindow(
    std::vector<std::complex<float>>& data)
{
    const std::size_t n = data.size();

    if (n < 2)
        return;

    constexpr float pi =
        3.14159265358979323846f;

    for (std::size_t i = 0;
         i < n;
         ++i) {

        const float window =
            0.5f *
            (
                1.0f -
                std::cos(
                    2.0f *
                    pi *
                    static_cast<float>(i) /
                    static_cast<float>(n - 1)
                    )
                );

        data[i] *= window;
    }
}

void FFTProcessor::fft(
    std::vector<std::complex<float>>& data)
{
    const std::size_t n = data.size();

    if (n <= 1)
        return;

    /*
     * Bit-reversal permutation.
     */
    std::size_t j = 0;

    for (std::size_t i = 1;
         i < n;
         ++i) {

        std::size_t bit =
            n >> 1;

        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }

        j ^= bit;

        if (i < j)
            std::swap(
                data[i],
                data[j]
                );
    }

    constexpr float pi =
        3.14159265358979323846f;

    for (std::size_t length = 2;
         length <= n;
         length <<= 1) {

        const float angle =
            -2.0f *
            pi /
            static_cast<float>(length);

        const std::complex<float> stageRotation(
            std::cos(angle),
            std::sin(angle)
            );

        for (std::size_t start = 0;
             start < n;
             start += length) {

            std::complex<float> rotation(
                1.0f,
                0.0f
                );

            for (std::size_t k = 0;
                 k < length / 2;
                 ++k) {

                const std::complex<float> even =
                    data[start + k];

                const std::complex<float> odd =
                    data[
                        start +
                        k +
                        length / 2
                ] *
                    rotation;

                data[start + k] =
                    even + odd;

                data[
                    start +
                    k +
                    length / 2
                ] =
                    even - odd;

                rotation *=
                    stageRotation;
            }
        }
    }
}