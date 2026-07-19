#include "SpectrumCalibrator.h"

#include <algorithm>
#include <cmath>

namespace HFSDR
{

void SpectrumCalibrator::normaliseTonePower(
    const std::vector<std::complex<float>>& fftData,
    float windowSum,
    std::vector<float>& outputPower) const
{
    const std::size_t n = fftData.size();
    outputPower.resize(n);

    if (n == 0)
        return;

    const float safeWindowSum =
        std::max(windowSum, 1.0e-12f);

    const float powerScale =
        1.0f / (safeWindowSum * safeWindowSum);

    const std::size_t half = n / 2;

    for (std::size_t i = 0; i < n; ++i) {
        const std::size_t shiftedIndex =
            (i + half) % n;

        outputPower[i] =
            std::norm(fftData[shiftedIndex]) * powerScale;
    }
}

void SpectrumCalibrator::powerToDbfs(
    const std::vector<float>& inputPower,
    std::vector<float>& outputDbfs) const
{
    outputDbfs.resize(inputPower.size());

    const float minimumPower =
        std::pow(10.0f, m_minimumDbfs / 10.0f);

    for (std::size_t i = 0;
         i < inputPower.size();
         ++i) {

        const float power =
            std::max(inputPower[i], minimumPower);

        outputDbfs[i] =
            std::max(
                10.0f * std::log10(power),
                m_minimumDbfs
                );
    }
}

void SpectrumCalibrator::setMinimumDbfs(float minimumDbfs)
{
    m_minimumDbfs =
        std::clamp(minimumDbfs, -300.0f, 0.0f);
}

float SpectrumCalibrator::minimumDbfs() const
{
    return m_minimumDbfs;
}

} // namespace HFSDR
