#include "AMDemodulator.h"

#include <cmath>

namespace HFSDR
{

AMDemodulator::AMDemodulator()
{
}

void AMDemodulator::process(const IQBuffer& input,
                            std::vector<float>& outputAudio)
{
    outputAudio.resize(input.size());

    constexpr float dcSmoothing = 0.001f;

    for (std::size_t i = 0; i < input.size(); ++i) {
        const float magnitude = std::abs(input.samples()[i]);

        m_dcAverage += dcSmoothing * (magnitude - m_dcAverage);

        outputAudio[i] = magnitude - m_dcAverage;
    }
}

} // namespace HFSDR
