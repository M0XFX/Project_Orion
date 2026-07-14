#include "ProductDetector.h"

#include <complex>

namespace HFSDR
{

void ProductDetector::setMode(
    DemodulationMode mode)
{
    if (mode != DemodulationMode::USB &&
        mode != DemodulationMode::LSB &&
        mode != DemodulationMode::CW) {
        return;
    }

    m_mode = mode;
}

void ProductDetector::process(
    const IQBuffer& input,
    std::vector<float>& outputAudio)
{
    outputAudio.resize(input.size());

    for (std::size_t i = 0;
         i < input.size();
         ++i) {

        const IQBuffer::Sample sample =
            input.samples()[i];

        switch (m_mode) {
        case DemodulationMode::USB:
            outputAudio[i] = sample.real();
            break;

        case DemodulationMode::LSB:
            /*
             * Conjugation reverses the complex-frequency
             * sense for lower-sideband processing.
             *
             * The real component is unchanged, but keeping
             * this distinction here prepares the detector
             * for later BFO and sideband-filter processing.
             */
            outputAudio[i] =
                std::conj(sample).real();
            break;

        case DemodulationMode::CW:
            // A proper CW BFO will be added later.
            outputAudio[i] = sample.real();
            break;

        default:
            outputAudio[i] = 0.0f;
            break;
        }
    }
}

void ProductDetector::reset()
{
    // Reserved for the future BFO/NCO state.
}

} // namespace HFSDR