#include "FFTProcessor.h"

#include <algorithm>
#include <cmath>

namespace HFSDR
{

FFTProcessor::FFTProcessor(std::size_t fftSize)
    : m_fftSize(std::max<std::size_t>(fftSize, 2))
{
    rebuildWindow();
}

std::size_t FFTProcessor::fftSize() const
{
    return m_fftSize;
}

float FFTProcessor::windowSum() const
{
    return m_windowSum;
}

float FFTProcessor::equivalentNoiseBandwidthBins() const
{
    if (m_windowSum <= 0.0f)
        return 1.0f;

    return static_cast<float>(m_fftSize) *
           m_windowPowerSum /
           (m_windowSum * m_windowSum);
}

bool FFTProcessor::process(
    const IQBuffer& input,
    std::vector<std::complex<float>>& outputFft)
{
    if (input.size() != m_fftSize)
        return false;

    outputFft.resize(m_fftSize);

    for (std::size_t i = 0; i < m_fftSize; ++i) {
        outputFft[i] = input.samples()[i] * m_window[i];
    }

    transform(outputFft);
    return true;
}

void FFTProcessor::rebuildWindow()
{
    m_window.resize(m_fftSize);
    m_windowSum = 0.0f;
    m_windowPowerSum = 0.0f;

    constexpr float pi = 3.14159265358979323846f;

    for (std::size_t i = 0; i < m_fftSize; ++i) {
        const float window =
            0.5f *
            (1.0f -
             std::cos(
                 2.0f * pi * static_cast<float>(i) /
                 static_cast<float>(m_fftSize - 1)
                 ));

        m_window[i] = window;
        m_windowSum += window;
        m_windowPowerSum += window * window;
    }
}

void FFTProcessor::transform(
    std::vector<std::complex<float>>& data) const
{
    const std::size_t n = data.size();

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

    for (std::size_t length = 2;
         length <= n;
         length <<= 1) {

        const float angle =
            -2.0f * pi / static_cast<float>(length);

        const std::complex<float> stageRotation(
            std::cos(angle),
            std::sin(angle)
            );

        for (std::size_t start = 0;
             start < n;
             start += length) {

            std::complex<float> rotation(1.0f, 0.0f);

            for (std::size_t k = 0;
                 k < length / 2;
                 ++k) {

                const std::complex<float> even =
                    data[start + k];

                const std::complex<float> odd =
                    data[start + k + length / 2] * rotation;

                data[start + k] = even + odd;
                data[start + k + length / 2] = even - odd;

                rotation *= stageRotation;
            }
        }
    }
}

} // namespace HFSDR
