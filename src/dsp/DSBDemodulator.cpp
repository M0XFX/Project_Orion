#include "DSBDemodulator.h"

namespace HFSDR
{

void DSBDemodulator::process(
    const IQBuffer& input,
    std::vector<float>& outputAudio)
{
    outputAudio.resize(input.size());

    for (std::size_t i = 0;
         i < input.size();
         ++i) {

        // Synchronous product detection.
        // The received DSB carrier must be accurately
        // aligned with the receiver's digital oscillator.
        outputAudio[i] =
            input.samples()[i].real();
    }
}

} // namespace HFSDR