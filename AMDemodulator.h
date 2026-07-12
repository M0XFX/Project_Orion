#ifndef AMDEMODULATOR_H
#define AMDEMODULATOR_H

#include <vector>

#include "IQBuffer.h"

namespace HFSDR
{

class AMDemodulator
{
public:
    AMDemodulator();

    void process(const IQBuffer& input, std::vector<float>& outputAudio);

private:
    float m_dcAverage = 0.0f;
};

} // namespace HFSDR

#endif