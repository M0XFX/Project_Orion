#include "AGCProcessor.h"

#include <cmath>

namespace HFSDR
{

AGCProcessor::AGCProcessor()
{
}

void AGCProcessor::setTargetLevel(float target)
{
    m_targetLevel = target;
}

float AGCProcessor::targetLevel() const
{
    return m_targetLevel;
}

void AGCProcessor::process(const IQBuffer& input, IQBuffer& output)
{
    output.resize(input.size());

    if (input.empty())
        return;

    float power = 0.0f;

    for (const auto& sample : input.samples())
        power += std::norm(sample);

    const float rms = std::sqrt(power / static_cast<float>(input.size()));
    const float desiredGain = m_targetLevel / (rms + 1.0e-6f);

    const float rate = desiredGain < m_gain ? m_attack : m_decay;

    m_gain += (desiredGain - m_gain) * rate;

    for (std::size_t i = 0; i < input.size(); ++i)
        output.samples()[i] = input.samples()[i] * m_gain;
}

} // namespace HFSDR