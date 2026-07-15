#include "FFTProcessor.h"

#include <cmath>
#include <algorithm>

FFTProcessor::FFTProcessor()
{
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
     * Calculate the coherent gain of the Hann window.
     *
     * A full-scale complex tone positioned exactly on
     * an FFT bin should read approximately 0 dBFS after
     * this correction.
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

    outputDb.resize(n);

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
         * negative frequencies appear on the left,
         * positive frequencies on the right.
         */
        const std::size_t shiftedIndex =
            (i + half) % n;

        const float magnitude =
            std::abs(
                data[shiftedIndex]
                ) /
            normalisation;

        outputDb[i] =
            20.0f *
            std::log10(
                std::max(
                    magnitude,
                    minimumMagnitude
                    )
                );
    }
}




void FFTProcessor::applyHannWindow(std::vector<std::complex<float>>& data)
{
    const std::size_t n = data.size();

    if (n < 2)
        return;

    constexpr float pi = 3.14159265358979323846f;

    for (std::size_t i = 0; i < n; ++i) {
        const float w = 0.5f * (1.0f - std::cos(2.0f * pi * i / (n - 1)));
        data[i] *= w;
    }
}

void FFTProcessor::fft(std::vector<std::complex<float>>& data)
{
    const std::size_t n = data.size();

    if (n <= 1)
        return;

    // Bit reversal
    std::size_t j = 0;

    for (std::size_t i = 1; i < n; ++i) {
        std::size_t bit = n >> 1;

        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }

        j ^= bit;

        if (i < j)
            std::swap(data[i], data[j]);
    }

    constexpr float pi = 3.14159265358979323846f;

    for (std::size_t len = 2; len <= n; len <<= 1) {
        const float angle = -2.0f * pi / static_cast<float>(len);
        const std::complex<float> wLen(std::cos(angle), std::sin(angle));

        for (std::size_t i = 0; i < n; i += len) {
            std::complex<float> w(1.0f, 0.0f);

            for (std::size_t k = 0; k < len / 2; ++k) {
                const std::complex<float> u = data[i + k];
                const std::complex<float> v = data[i + k + len / 2] * w;

                data[i + k] = u + v;
                data[i + k + len / 2] = u - v;

                w *= wLen;
            }
        }
    }
}