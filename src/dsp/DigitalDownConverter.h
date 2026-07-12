#ifndef DIGITALDOWNCONVERTER_H
#define DIGITALDOWNCONVERTER_H

#include "IQBuffer.h"
#include "FIRFilter.h"
#include "Decimator.h"

namespace HFSDR
{

class DigitalDownConverter
{
public:
    DigitalDownConverter();

    void setSampleRate(float sampleRate);
    float sampleRate() const;

    void setFrequencyShiftHz(float frequencyHz);
    float frequencyShiftHz() const;

    void process(const IQBuffer& input, IQBuffer& output);

private:
    float m_sampleRate = 2048000.0f;
    float m_frequencyShiftHz = 0.0f;
    float m_phase = 0.0f;

    FIRFilter m_lowPassFilter;
    IQBuffer m_mixerBuffer;
    IQBuffer m_filterBuffer;
    Decimator m_decimator;
};

} // namespace HFSDR

#endif
