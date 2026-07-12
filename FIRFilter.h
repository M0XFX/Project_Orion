#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <vector>
#include <complex>

#include "IQBuffer.h"

namespace HFSDR
{

class FIRFilter
{
public:
    FIRFilter();

    void setLowPass(float sampleRate, float cutoffHz, int tapCount);
    void process(const IQBuffer& input, IQBuffer& output);

private:
    std::vector<float> m_taps;
    std::vector<IQBuffer::Sample> m_delayLine;
};

} // namespace HFSDR

#endif
