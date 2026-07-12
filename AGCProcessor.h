#ifndef AGCPROCESSOR_H
#define AGCPROCESSOR_H

#include "IQBuffer.h"

namespace HFSDR
{

class AGCProcessor
{
public:
    AGCProcessor();

    void setTargetLevel(float target);
    float targetLevel() const;

    void process(const IQBuffer& input, IQBuffer& output);

private:
    float m_targetLevel = 0.25f;
    float m_gain = 1.0f;
    float m_attack = 0.05f;
    float m_decay = 0.005f;
};

} // namespace HFSDR

#endif
