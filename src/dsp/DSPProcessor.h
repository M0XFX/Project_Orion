#ifndef DSPPROCESSOR_H
#define DSPPROCESSOR_H

#include <vector>

#include "AGCProcessor.h"
#include "AMDemodulator.h"
#include "AudioDecimator.h"
#include "AudioFIRFilter.h"
#include "ComplexSidebandFilter.h"
#include "Decimator.h"
#include "DSBDemodulator.h"
#include "FIRFilter.h"
#include "IQBuffer.h"
#include "NFMDemodulator.h"
#include "ProductDetector.h"
#include "ReceiverConfiguration.h"
#include "WFMDemodulator.h"

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
    void configureSidebandFilter();

    void processSidebandMode(
        const IQBuffer& input,
        std::vector<float>& outputAudio
        );

    ReceiverConfiguration m_configuration;

    AGCProcessor m_agc;

    AMDemodulator m_amDemodulator;
    DSBDemodulator m_dsbDemodulator;
    NFMDemodulator m_nfmDemodulator;
    WFMDemodulator m_wfmDemodulator;
    ProductDetector m_productDetector;

    // Reduces the SSB IQ rate from
    // 256 kS/s to 32 kS/s.
    FIRFilter m_ssbPreFilter;
    Decimator m_ssbDecimator;

    ComplexSidebandFilter
        m_sidebandFilter;

    AudioFIRFilter m_audioFilter;
    AudioDecimator m_audioDecimator;

    IQBuffer m_ssbPreFilteredIq;
    IQBuffer m_ssbDecimatedIq;
    IQBuffer m_sidebandFilteredIq;

    std::vector<float> m_demodulatedAudio;
    std::vector<float> m_filteredAudio;
};

} // namespace HFSDR

#endif