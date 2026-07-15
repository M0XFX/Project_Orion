#include "ComplexSidebandFilter.h"

#include <algorithm>
#include <cmath>

namespace HFSDR
{

void ComplexSidebandFilter::setBandPass(
    float sampleRate,
    float lowFrequencyHz,
    float highFrequencyHz,
    int tapCount)
{
    if (sampleRate <= 0.0f)
        return;

    if (lowFrequencyHz > highFrequencyHz)
        std::swap(lowFrequencyHz, highFrequencyHz);

    if (tapCount < 3)
        tapCount = 3;

    if ((tapCount % 2) == 0)
        ++tapCount;

    const float nyquist = sampleRate * 0.5f;

    lowFrequencyHz = std::clamp(
        lowFrequencyHz,
        -nyquist * 0.95f,
        nyquist * 0.95f
        );

    highFrequencyHz = std::clamp(
        highFrequencyHz,
        -nyquist * 0.95f,
        nyquist * 0.95f
        );

    if (highFrequencyHz <= lowFrequencyHz)
        return;

    constexpr float pi =
        3.14159265358979323846f;

    const float centreFrequencyHz =
        (lowFrequencyHz + highFrequencyHz) *
        0.5f;

    const float prototypeCutoffHz =
        (highFrequencyHz - lowFrequencyHz) *
        0.5f;

    const float normalisedCutoff =
        prototypeCutoffHz / sampleRate;

    const int middle = tapCount / 2;

    std::vector<float> prototypeTaps(
        static_cast<std::size_t>(tapCount),
        0.0f
        );

    float prototypeSum = 0.0f;

    for (int i = 0; i < tapCount; ++i) {
        const int n = i - middle;

        float sinc = 0.0f;

        if (n == 0) {
            sinc = 2.0f * normalisedCutoff;
        } else {
            sinc =
                std::sin(
                    2.0f *
                    pi *
                    normalisedCutoff *
                    static_cast<float>(n)
                    ) /
                (
                    pi *
                    static_cast<float>(n)
                    );
        }

        const float window =
            0.54f -
            0.46f *
                std::cos(
                    2.0f *
                    pi *
                    static_cast<float>(i) /
                    static_cast<float>(tapCount - 1)
                    );

        prototypeTaps[
            static_cast<std::size_t>(i)
        ] = sinc * window;

        prototypeSum +=
            prototypeTaps[
                static_cast<std::size_t>(i)
        ];
    }

    if (std::abs(prototypeSum) > 1.0e-12f) {
        for (float& tap : prototypeTaps)
            tap /= prototypeSum;
    }

    m_taps.resize(
        static_cast<std::size_t>(tapCount)
        );

    for (int i = 0; i < tapCount; ++i) {
        const int n = i - middle;

        const float phase =
            2.0f *
            pi *
            centreFrequencyHz *
            static_cast<float>(n) /
            sampleRate;

        const std::complex<float> oscillator(
            std::cos(phase),
            std::sin(phase)
            );

        m_taps[
            static_cast<std::size_t>(i)
        ] =
            prototypeTaps[
                static_cast<std::size_t>(i)
        ] *
            oscillator;
    }

    m_delayLine.assign(
        m_taps.size(),
        IQBuffer::Sample(0.0f, 0.0f)
        );

    m_writeIndex = 0;
}

void ComplexSidebandFilter::process(
    const IQBuffer& input,
    IQBuffer& output)
{
    output.resize(input.size());

    if (m_taps.empty()) {
        output = input;
        return;
    }

    for (std::size_t i = 0;
         i < input.size();
         ++i) {

        m_delayLine[m_writeIndex] =
            input.samples()[i];

        IQBuffer::Sample accumulator(
            0.0f,
            0.0f
            );

        std::size_t delayIndex =
            m_writeIndex;

        for (std::size_t tapIndex = 0;
             tapIndex < m_taps.size();
             ++tapIndex) {

            accumulator +=
                m_delayLine[delayIndex] *
                m_taps[tapIndex];

            if (delayIndex == 0) {
                delayIndex =
                    m_delayLine.size() - 1;
            } else {
                --delayIndex;
            }
        }

        output.samples()[i] =
            accumulator;

        m_writeIndex =
            (m_writeIndex + 1) %
            m_delayLine.size();
    }
}

void ComplexSidebandFilter::reset()
{
    std::fill(
        m_delayLine.begin(),
        m_delayLine.end(),
        IQBuffer::Sample(0.0f, 0.0f)
        );

    m_writeIndex = 0;
}

} // namespace HFSDR