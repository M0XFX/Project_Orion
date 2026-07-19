#include "SpectrumFrequencySmoother.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace HFSDR
{

bool SpectrumFrequencySmoother::enabled() const
{
    return m_enabled;
}

void SpectrumFrequencySmoother::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

int SpectrumFrequencySmoother::radius() const
{
    return m_radius;
}

void SpectrumFrequencySmoother::setRadius(int radius)
{
    m_radius = std::clamp(radius, 1, 8);
}

float SpectrumFrequencySmoother::strength() const
{
    return m_strength;
}

void SpectrumFrequencySmoother::setStrength(float strength)
{
    if (!std::isfinite(strength))
        return;

    m_strength = std::clamp(strength, 0.0f, 1.0f);
}

std::vector<float> SpectrumFrequencySmoother::makeKernel() const
{
    const int kernelSize = (m_radius * 2) + 1;
    std::vector<float> kernel(
        static_cast<std::size_t>(kernelSize),
        0.0f
        );

    // A compact Gaussian kernel gives a natural-looking trace without the
    // ringing produced by a rectangular moving average.
    const float sigma = std::max(
        0.8f,
        static_cast<float>(m_radius) * 0.65f
        );

    const float denominator = 2.0f * sigma * sigma;
    float sum = 0.0f;

    for (int offset = -m_radius;
         offset <= m_radius;
         ++offset) {
        const float distance = static_cast<float>(offset);
        const float weight = std::exp(
            -(distance * distance) / denominator
            );

        kernel[static_cast<std::size_t>(offset + m_radius)] = weight;
        sum += weight;
    }

    if (sum > 0.0f) {
        for (float& weight : kernel)
            weight /= sum;
    }

    return kernel;
}

void SpectrumFrequencySmoother::process(
    const std::vector<float>& inputDbfs,
    std::vector<float>& outputDbfs) const
{
    outputDbfs = inputDbfs;

    if (!m_enabled ||
        m_strength <= 0.0f ||
        inputDbfs.size() < 3) {
        return;
    }

    const std::vector<float> kernel = makeKernel();
    const std::size_t sampleCount = inputDbfs.size();

    for (std::size_t index = 0;
         index < sampleCount;
         ++index) {
        float smoothedDb = 0.0f;
        float usedWeight = 0.0f;

        for (int offset = -m_radius;
             offset <= m_radius;
             ++offset) {
            const long long candidate =
                static_cast<long long>(index) + offset;

            if (candidate < 0 ||
                candidate >= static_cast<long long>(sampleCount)) {
                continue;
            }

            const float weight = kernel[
                static_cast<std::size_t>(offset + m_radius)
                ];

            smoothedDb +=
                inputDbfs[static_cast<std::size_t>(candidate)] * weight;
            usedWeight += weight;
        }

        if (usedWeight <= 0.0f)
            continue;

        smoothedDb /= usedWeight;

        outputDbfs[index] =
            inputDbfs[index] +
            m_strength * (smoothedDb - inputDbfs[index]);
    }
}

} // namespace HFSDR
