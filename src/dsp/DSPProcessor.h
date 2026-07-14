#ifndef DSPPROCESSOR_H
#define DSPPROCESSOR_H

#include <vector>

#include "AGCProcessor.h"
#include "AMDemodulator.h"
#include "AudioDecimator.h"
#include "AudioFIRFilter.h"
#include "DSBDemodulator.h"
#include "IQBuffer.h"
#include "NFMDemodulator.h"
#include "ReceiverConfiguration.h"

namespace HFSDR
{

class DSPProcessor
{
public:
    DSPProcessor();

    void setConfiguration(
        const ReceiverConfiguration& configuration
        );

    ReceiverConfiguration configuration() const;

    void process(
        const IQBuffer& input,
        std::vector<float>& outputAudio
        );

private:
    void configureAudioFilter();

    ReceiverConfiguration m_configuration;

    AGCProcessor m_agc;

    AMDemodulator m_amDemodulator;
    DSBDemodulator m_dsbDemodulator;
    NFMDemodulator m_nfmDemodulator;

    AudioFIRFilter m_audioFilter;
    AudioDecimator m_audioDecimator;

    std::vector<float> m_demodulatedAudio;
    std::vector<float> m_filteredAudio;
};

} // namespace HFSDR

#endif