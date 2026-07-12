#include "SignalGenerator.h"

#include <cmath>
#include <random>

namespace HFSDR
{

SignalGenerator::SignalGenerator()
{
}

void SignalGenerator::generate(IQBuffer& buffer,
                               float sampleRate,
                               float toneFrequency)
{
    constexpr float twoPi = 6.28318530717958647692f;

    static std::mt19937 rng{std::random_device{}()};
    static std::normal_distribution<float> noiseDist(0.0f, 0.03f);

    const float phaseIncrement = twoPi * toneFrequency / sampleRate;

    for (auto& sample : buffer.samples()) {
        const float i = std::cos(m_phase);
        const float q = std::sin(m_phase);

        const float noiseI = noiseDist(rng);
        const float noiseQ = noiseDist(rng);

        sample = IQBuffer::Sample(i + noiseI, q + noiseQ);

        m_phase += phaseIncrement;

        if (m_phase >= twoPi)
            m_phase -= twoPi;
    }
}

} // namespace HFSDR
