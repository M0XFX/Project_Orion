#include "SpectrumDetector.h"

#include <algorithm>

namespace HFSDR
{

void SpectrumDetector::setMode(SpectrumDetectorMode mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    reset();
}

SpectrumDetectorMode SpectrumDetector::mode() const
{
    return m_mode;
}

void SpectrumDetector::setAveragingAlpha(float alpha)
{
    m_averagingAlpha =
        std::clamp(alpha, 0.01f, 1.0f);

    reset();
}

float SpectrumDetector::averagingAlpha() const
{
    return m_averagingAlpha;
}

void SpectrumDetector::reset()
{
    m_initialised = false;
    m_state.clear();
}

void SpectrumDetector::process(
    const std::vector<float>& inputPower,
    std::vector<float>& outputPower)
{
    if (inputPower.empty()) {
        outputPower.clear();
        return;
    }

    if (m_mode == SpectrumDetectorMode::Sample) {
        outputPower = inputPower;
        return;
    }

    if (!m_initialised ||
        m_state.size() != inputPower.size()) {

        m_state = inputPower;
        m_initialised = true;
    } else if (m_mode == SpectrumDetectorMode::Average) {
        const float previousWeight =
            1.0f - m_averagingAlpha;

        for (std::size_t i = 0;
             i < inputPower.size();
             ++i) {

            m_state[i] =
                m_averagingAlpha * inputPower[i] +
                previousWeight * m_state[i];
        }
    } else {
        for (std::size_t i = 0;
             i < inputPower.size();
             ++i) {

            m_state[i] =
                std::max(m_state[i], inputPower[i]);
        }
    }

    outputPower = m_state;
}

} // namespace HFSDR
