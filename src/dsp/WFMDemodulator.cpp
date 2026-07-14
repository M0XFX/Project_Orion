#include "WFMDemodulator.h"

#include <cmath>

namespace HFSDR
{

WFMDemodulator::WFMDemodulator()
{
    updateDeEmphasisCoefficient();
}

void WFMDemodulator::setSampleRate(
    float sampleRate)
{
    if (sampleRate <= 0.0f)
        return;

    m_sampleRate = sampleRate;
    updateDeEmphasisCoefficient();
}

void WFMDemodulator::setDeviationHz(
    float deviationHz)
{
    if (deviationHz <= 0.0f)
        return;

    m_deviationHz = deviationHz;
}

void WFMDemodulator::setDeEmphasisTimeConstant(
    float timeConstantSeconds)
{
    if (timeConstantSeconds <= 0.0f)
        return;

    m_deEmphasisTimeConstant =
        timeConstantSeconds;

    updateDeEmphasisCoefficient();
}

void WFMDemodulator::process(
    const IQBuffer& input,
    std::vector<float>& outputAudio)
{
    outputAudio.resize(input.size());

    if (input.empty())
        return;

    constexpr float twoPi =
        6.28318530717958647692f;

    // Converts discriminator phase change into
    // approximately normalised audio amplitude.
    const float discriminatorScale =
        m_sampleRate /
        (twoPi * m_deviationHz);

    for (std::size_t i = 0;
         i < input.size();
         ++i) {

        const IQBuffer::Sample currentSample =
            input.samples()[i];

        const IQBuffer::Sample phaseProduct =
            currentSample *
            std::conj(m_previousSample);

        const float phaseDifference =
            std::atan2(
                phaseProduct.imag(),
                phaseProduct.real()
                );

        const float discriminatorOutput =
            phaseDifference *
            discriminatorScale;

        // First-order 50 us de-emphasis filter.
        m_deEmphasisState +=
            m_deEmphasisAlpha *
            (
                discriminatorOutput -
                m_deEmphasisState
                );

        outputAudio[i] =
            m_deEmphasisState;

        m_previousSample = currentSample;
    }
}

void WFMDemodulator::reset()
{
    m_previousSample =
        IQBuffer::Sample(1.0f, 0.0f);

    m_deEmphasisState = 0.0f;
}

void WFMDemodulator::
    updateDeEmphasisCoefficient()
{
    const float samplePeriod =
        1.0f / m_sampleRate;

    m_deEmphasisAlpha =
        samplePeriod /
        (
            m_deEmphasisTimeConstant +
            samplePeriod
            );
}

} // namespace HFSDR