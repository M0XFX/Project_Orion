#ifndef COMPLEXSIDEBANDFILTER_H
#define COMPLEXSIDEBANDFILTER_H

#include <complex>
#include <cstddef>
#include <vector>

#include "IQBuffer.h"

namespace HFSDR
{

class ComplexSidebandFilter
{
public:
    ComplexSidebandFilter() = default;

    void setBandPass(
        float sampleRate,
        float lowFrequencyHz,
        float highFrequencyHz,
        int tapCount
        );

    void process(
        const IQBuffer& input,
        IQBuffer& output
        );

    void reset();

private:
    std::vector<std::complex<float>> m_taps;
    std::vector<IQBuffer::Sample> m_delayLine;

    std::size_t m_writeIndex = 0;
};

} // namespace HFSDR

#endif