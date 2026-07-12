#include "DigitalDownConverter.h"

#include <cmath>

namespace HFSDR
{

DigitalDownConverter::DigitalDownConverter()
{
    m_lowPassFilter.setLowPass(2048000.0f, 12000.0f, 101);
    m_decimator.setFactor(8);
}

void DigitalDownConverter::setSampleRate(float sampleRate)
{
    m_sampleRate = sampleRate;
    m_lowPassFilter.setLowPass(m_sampleRate, 12000.0f, 101);
}

float DigitalDownConverter::sampleRate() const
{
    return m_sampleRate;
}

void DigitalDownConverter::setFrequencyShiftHz(float frequencyHz)
{
    m_frequencyShiftHz = frequencyHz;
}

float DigitalDownConverter::frequencyShiftHz() const
{
    return m_frequencyShiftHz;
}

void DigitalDownConverter::process(const IQBuffer& input, IQBuffer& output)
{
    m_mixerBuffer.resize(input.size());


    if (input.empty()) {
        output.resize(0);
        return;
    }

    IQBuffer::Sample average(0.0f, 0.0f);

    for (const auto& sample : input.samples())
        average += sample;

    average /= static_cast<float>(input.size());

    constexpr float twoPi = 6.28318530717958647692f;
    const float phaseIncrement = -twoPi * m_frequencyShiftHz / m_sampleRate;

    for (std::size_t i = 0; i < input.size(); ++i) {
        const IQBuffer::Sample dcRemoved = input.samples()[i] - average;

        const IQBuffer::Sample oscillator(
            std::cos(m_phase),
            std::sin(m_phase)
            );

        m_mixerBuffer.samples()[i] = dcRemoved * oscillator;

        m_phase += phaseIncrement;
/*
        if (m_phase > twoPi)
            m_phase -= twoPi;
        else if (m_phase < -twoPi)
            m_phase += twoPi;
*/

        while (m_phase > twoPi)
            m_phase -= twoPi;

        while (m_phase < -twoPi)
            m_phase += twoPi;


    }

    //m_lowPassFilter.process(m_mixerBuffer, m_filterBuffer);
    //output = m_filterBuffer;

    m_lowPassFilter.process(m_mixerBuffer, m_filterBuffer);
    m_decimator.process(m_filterBuffer, output);
}

} // namespace HFSDR