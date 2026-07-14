#include "NFMDemodulator.h"

#include <cmath>

namespace HFSDR
{

void NFMDemodulator::process(
    const IQBuffer& input,
    std::vector<float>& outputAudio)
{
    outputAudio.resize(input.size());

    for (std::size_t i = 0;
         i < input.size();
         ++i) {

        const IQBuffer::Sample currentSample =
            input.samples()[i];

        const IQBuffer::Sample phaseProduct =
            currentSample *
            std::conj(m_previousSample);

        outputAudio[i] =
            std::atan2(
                phaseProduct.imag(),
                phaseProduct.real()
                );

        m_previousSample = currentSample;
    }
}

void NFMDemodulator::reset()
{
    m_previousSample =
        IQBuffer::Sample(1.0f, 0.0f);
}

} // namespace HFSDR