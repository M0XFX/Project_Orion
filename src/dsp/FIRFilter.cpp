
#include "FIRFilter.h"

#include <cmath>

namespace HFSDR
{

FIRFilter::FIRFilter()
{
}

void FIRFilter::setLowPass(float sampleRate, float cutoffHz, int tapCount)
{
    m_taps.clear();
    m_taps.resize(tapCount);

    m_delayLine.clear();
    m_delayLine.resize(tapCount);

    constexpr float pi = 3.14159265358979323846f;

    const float fc = cutoffHz / sampleRate;
    const int middle = tapCount / 2;

    float sum = 0.0f;

    for (int i = 0; i < tapCount; ++i) {
        const int n = i - middle;

        float sinc;

        if (n == 0)
            sinc = 2.0f * fc;
        else
            sinc = std::sin(2.0f * pi * fc * n) / (pi * n);

        const float window =
            0.54f - 0.46f * std::cos((2.0f * pi * i) / (tapCount - 1));

        m_taps[i] = sinc * window;
        sum += m_taps[i];
    }

    for (float& tap : m_taps)
        tap /= sum;
}

void FIRFilter::process(const IQBuffer& input, IQBuffer& output)
{
    output.resize(input.size());

    if (m_taps.empty()) {
        for (std::size_t i = 0; i < input.size(); ++i)
            output.samples()[i] = input.samples()[i];

        return;
    }

    for (std::size_t i = 0; i < input.size(); ++i) {
        for (std::size_t d = m_delayLine.size() - 1; d > 0; --d)
            m_delayLine[d] = m_delayLine[d - 1];

        m_delayLine[0] = input.samples()[i];

        IQBuffer::Sample acc(0.0f, 0.0f);

        for (std::size_t t = 0; t < m_taps.size(); ++t)
            acc += m_delayLine[t] * m_taps[t];

        output.samples()[i] = acc;
    }
}

} // namespace HFSDR