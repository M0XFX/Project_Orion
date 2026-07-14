#ifndef DIGITALDOWNCONVERTER_H
#define DIGITALDOWNCONVERTER_H

#include "Decimator.h"
#include "FIRFilter.h"
#include "IQBuffer.h"

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

    void setLowPassCutoffHz(float cutoffHz);
    float lowPassCutoffHz() const;

    void setDecimationFactor(int factor);
    int decimationFactor() const;

    float outputSampleRate() const;

    void process(
        const IQBuffer& input,
        IQBuffer& output
        );

private:
    void redesignFilter();

    float m_sampleRate = 2048000.0f;
    float m_frequencyShiftHz = 0.0f;
    float m_lowPassCutoffHz = 12000.0f;
    float m_phase = 0.0f;

    int m_decimationFactor = 8;
    int m_filterTapCount = 101;

    FIRFilter m_lowPassFilter;
    Decimator m_decimator;

    IQBuffer m_mixerBuffer;
    IQBuffer m_filterBuffer;
};

} // namespace HFSDR

#endif