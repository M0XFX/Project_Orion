#include "AudioFIRFilter.h"

#include <cmath>

namespace HFSDR
{

AudioFIRFilter::AudioFIRFilter()
{
}

void AudioFIRFilter::setLowPass(float sampleRate,
                                float cutoffHz,
                                int tapCount)
{
    if (tapCount < 3)
        tapCount = 3;

    // An odd tap count gives a symmetrical centre tap.
    if ((tapCount % 2) == 0)
        ++tapCount;

    m_taps.assign(static_cast<std::size_t>(tapCount), 0.0f);
    m_delayLine.assign(static_cast<std::size_t>(tapCount), 0.0f);

    constexpr float pi = 3.14159265358979323846f;

    const float normalisedCutoff = cutoffHz / sampleRate;
    const int middle = tapCount / 2;

    float sum = 0.0f;

    for (int i = 0; i < tapCount; ++i) {
        const int n = i - middle;

        float sinc = 0.0f;

        if (n == 0) {
            sinc = 2.0f * normalisedCutoff;
        } else {
            sinc = std::sin(2.0f * pi * normalisedCutoff *
                            static_cast<float>(n))
                   / (pi * static_cast<float>(n));
        }

        // Hamming window
        const float window =
            0.54f -
            0.46f * std::cos(
                2.0f * pi * static_cast<float>(i) /
                static_cast<float>(tapCount - 1));

        m_taps[static_cast<std::size_t>(i)] = sinc * window;
        sum += m_taps[static_cast<std::size_t>(i)];
    }

    if (std::abs(sum) > 1.0e-12f) {
        for (float& tap : m_taps)
            tap /= sum;
    }
}

void AudioFIRFilter::process(const std::vector<float>& input,
                             std::vector<float>& output)
{
    output.resize(input.size());

    if (m_taps.empty()) {
        output = input;
        return;
    }

    for (std::size_t i = 0; i < input.size(); ++i) {
        for (std::size_t d = m_delayLine.size() - 1; d > 0; --d)
            m_delayLine[d] = m_delayLine[d - 1];

        m_delayLine[0] = input[i];

        float accumulator = 0.0f;

        for (std::size_t t = 0; t < m_taps.size(); ++t)
            accumulator += m_delayLine[t] * m_taps[t];

        output[i] = accumulator;
    }
}

} // namespace HFSDR
