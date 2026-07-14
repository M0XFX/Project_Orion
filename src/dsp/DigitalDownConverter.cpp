#include "DigitalDownConverter.h"

#include <algorithm>
#include <cmath>

namespace HFSDR
{

DigitalDownConverter::DigitalDownConverter()
{
    m_decimator.setFactor(
        m_decimationFactor
        );

    redesignFilter();
}

void DigitalDownConverter::setSampleRate(
    float sampleRate)
{
    if (sampleRate <= 0.0f)
        return;

    if (m_sampleRate == sampleRate)
        return;

    m_sampleRate = sampleRate;
    redesignFilter();
}

float DigitalDownConverter::sampleRate() const
{
    return m_sampleRate;
}

void DigitalDownConverter::setFrequencyShiftHz(
    float frequencyHz)
{
    m_frequencyShiftHz = frequencyHz;
}

float
DigitalDownConverter::frequencyShiftHz() const
{
    return m_frequencyShiftHz;
}

void DigitalDownConverter::setLowPassCutoffHz(
    float cutoffHz)
{
    const float outputNyquist =
        outputSampleRate() * 0.5f;

    // Leave a small guard band below Nyquist.
    const float maximumCutoff =
        outputNyquist * 0.90f;

    cutoffHz = std::clamp(
        cutoffHz,
        100.0f,
        maximumCutoff
        );

    if (m_lowPassCutoffHz == cutoffHz)
        return;

    m_lowPassCutoffHz = cutoffHz;
    redesignFilter();
}

float
DigitalDownConverter::lowPassCutoffHz() const
{
    return m_lowPassCutoffHz;
}

void DigitalDownConverter::setDecimationFactor(
    int factor)
{
    if (factor < 1)
        factor = 1;

    if (m_decimationFactor == factor)
        return;

    m_decimationFactor = factor;

    m_decimator.setFactor(
        m_decimationFactor
        );

    redesignFilter();
}

int
DigitalDownConverter::decimationFactor() const
{
    return m_decimationFactor;
}

float
DigitalDownConverter::outputSampleRate() const
{
    return m_sampleRate /
           static_cast<float>(
               m_decimationFactor
               );
}

void DigitalDownConverter::redesignFilter()
{
    const float outputNyquist =
        outputSampleRate() * 0.5f;

    const float maximumCutoff =
        outputNyquist * 0.90f;

    m_lowPassCutoffHz = std::clamp(
        m_lowPassCutoffHz,
        100.0f,
        maximumCutoff
        );

    m_lowPassFilter.setLowPass(
        m_sampleRate,
        m_lowPassCutoffHz,
        m_filterTapCount
        );
}

void DigitalDownConverter::process(
    const IQBuffer& input,
    IQBuffer& output)
{
    if (input.empty()) {
        output.resize(0);
        return;
    }

    m_mixerBuffer.resize(
        input.size()
        );

    IQBuffer::Sample average(
        0.0f,
        0.0f
        );

    for (const auto& sample :
         input.samples()) {
        average += sample;
    }

    average /=
        static_cast<float>(
            input.size()
            );

    constexpr float twoPi =
        6.28318530717958647692f;

    const float phaseIncrement =
        -twoPi *
        m_frequencyShiftHz /
        m_sampleRate;

    for (std::size_t i = 0;
         i < input.size();
         ++i) {

        const IQBuffer::Sample dcRemoved =
            input.samples()[i] -
            average;

        const IQBuffer::Sample oscillator(
            std::cos(m_phase),
            std::sin(m_phase)
            );

        m_mixerBuffer.samples()[i] =
            dcRemoved * oscillator;

        m_phase += phaseIncrement;

        while (m_phase > twoPi)
            m_phase -= twoPi;

        while (m_phase < -twoPi)
            m_phase += twoPi;
    }

    m_lowPassFilter.process(
        m_mixerBuffer,
        m_filterBuffer
        );

    m_decimator.process(
        m_filterBuffer,
        output
        );
}

} // namespace HFSDR