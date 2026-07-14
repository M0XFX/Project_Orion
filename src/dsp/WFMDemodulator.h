#ifndef WFMDEMODULATOR_H
#define WFMDEMODULATOR_H

#include <vector>

#include "IQBuffer.h"

namespace HFSDR
{

class WFMDemodulator
{
public:
    WFMDemodulator();

    void setSampleRate(float sampleRate);
    void setDeviationHz(float deviationHz);
    void setDeEmphasisTimeConstant(float timeConstantSeconds);

    void process(
        const IQBuffer& input,
        std::vector<float>& outputAudio
        );

    void reset();

private:
    void updateDeEmphasisCoefficient();

    float m_sampleRate = 256000.0f;
    float m_deviationHz = 75000.0f;

    // European FM broadcast de-emphasis: 50 microseconds.
    float m_deEmphasisTimeConstant =
        50.0e-6f;

    float m_deEmphasisAlpha = 0.0f;
    float m_deEmphasisState = 0.0f;

    IQBuffer::Sample m_previousSample =
        IQBuffer::Sample(1.0f, 0.0f);
};

} // namespace HFSDR

#endif