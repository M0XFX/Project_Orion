#include "DSPProcessor.h"

namespace HFSDR
{

DSPProcessor::DSPProcessor()
{
    m_audioFilter.setLowPass(256000.0f, 12000.0f, 101);

    m_audioDecimator.setFactor(8);
}


void DSPProcessor::process(const IQBuffer& input, std::vector<float>& outputAudio)
{
    IQBuffer agcBuffer(input.size());

    m_agc.process(input, agcBuffer);

    m_amDemodulator.process(
        agcBuffer,
        m_demodulatedAudio
        );

    m_audioFilter.process(
        m_demodulatedAudio,
        m_filteredAudio
        );

    m_audioDecimator.process(
        m_filteredAudio,
        outputAudio
        );
}




} // namespace HFSDR