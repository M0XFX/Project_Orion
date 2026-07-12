#ifndef DSPPROCESSOR_H
#define DSPPROCESSOR_H

#include <vector>
#include "IQBuffer.h"
#include "AGCProcessor.h"
#include "AMDemodulator.h"
#include "AudioDecimator.h"
#include "AudioFIRFilter.h"

namespace HFSDR
{

class DSPProcessor
{
public:
    DSPProcessor();
    //void process(const IQBuffer& input, IQBuffer& output);
    void process(const IQBuffer& input,std::vector<float>& outputAudio);


private:
    AGCProcessor m_agc;
    AMDemodulator m_amDemodulator;

    AudioFIRFilter m_audioFilter;
    AudioDecimator m_audioDecimator;

    std::vector<float> m_demodulatedAudio;
    std::vector<float> m_filteredAudio;

};

} // namespace HFSDR

#endif