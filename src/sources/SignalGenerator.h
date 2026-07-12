#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include "IQBuffer.h"

namespace HFSDR
{

class SignalGenerator
{
public:
    SignalGenerator();

    void generate(IQBuffer& buffer,
                  float sampleRate,
                  float toneFrequency);

private:
    float m_phase = 0.0f;
};

} // namespace HFSDR

#endif
