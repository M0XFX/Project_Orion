#include "SpectrumSmoother.h"

#include <algorithm>
#include <cmath>

namespace HFSDR
{

bool SpectrumSmoother::enabled() const
{
    return m_enabled;
}

void SpectrumSmoother::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

int SpectrumSmoother::windowSize() const
{
    return m_windowSize;
}

void SpectrumSmoother::setWindowSize(int windowSize)
{
    windowSize = std::clamp(windowSize, 3, 9);

    if ((windowSize % 2) == 0)
        ++windowSize;

    m_windowSize = std::min(windowSize, 9);
}

float SpectrumSmoother::downwardThresholdDb() const
{
    return m_downwardThresholdDb;
}

void SpectrumSmoother::setDownwardThresholdDb(float thresholdDb)
{
    if (!std::isfinite(thresholdDb))
        return;

    m_downwardThresholdDb =
        std::clamp(thresholdDb, 0.0f, 20.0f);
}

float SpectrumSmoother::blend() const
{
    return m_blend;
}

void SpectrumSmoother::setBlend(float blend)
{
    if (!std::isfinite(blend))
        return;

    m_blend = std::clamp(blend, 0.0f, 1.0f);
}

void SpectrumSmoother::process(
    const std::vector<float>& inputDbfs,
    std::vector<float>& outputDbfs) const
{
    outputDbfs = inputDbfs;

    if (!m_enabled || inputDbfs.size() < 3)
        return;

    const std::size_t radius =
        static_cast<std::size_t>(m_windowSize / 2);

    std::vector<float> neighbourhood;
    neighbourhood.reserve(
        static_cast<std::size_t>(m_windowSize)
        );

    for (std::size_t index = 0;
         index < inputDbfs.size();
         ++index) {
        const std::size_t first =
            index > radius ? index - radius : 0;

        const std::size_t last =
            std::min(
                inputDbfs.size() - 1,
                index + radius
                );

        neighbourhood.clear();

        for (std::size_t neighbour = first;
             neighbour <= last;
             ++neighbour) {
            neighbourhood.push_back(
                inputDbfs[neighbour]
                );
        }

        const auto medianPosition =
            neighbourhood.begin() +
            static_cast<std::ptrdiff_t>(
                neighbourhood.size() / 2
                );

        std::nth_element(
            neighbourhood.begin(),
            medianPosition,
            neighbourhood.end()
            );

        const float localMedianDb = *medianPosition;
        const float minimumAllowedDb =
            localMedianDb - m_downwardThresholdDb;

        const float currentDb = inputDbfs[index];

        if (currentDb >= minimumAllowedDb)
            continue;

        outputDbfs[index] =
            currentDb +
            m_blend * (minimumAllowedDb - currentDb);
    }
}

} // namespace HFSDR
