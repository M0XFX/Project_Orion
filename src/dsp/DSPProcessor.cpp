#include "DSPProcessor.h"

#include <algorithm>

namespace HFSDR
{

DSPProcessor::DSPProcessor()
{
    m_audioDecimator.setFactor(8);
    configureAudioFilter();
}

void DSPProcessor::setConfiguration(
    const ReceiverConfiguration& configuration)
{
    if (m_configuration == configuration)
        return;

    const DemodulationMode previousMode =
        m_configuration.mode;

    m_configuration = configuration;

    if (previousMode != m_configuration.mode &&
        m_configuration.mode ==
            DemodulationMode::NFM) {

        m_nfmDemodulator.reset();
    }

    configureAudioFilter();
}

ReceiverConfiguration
DSPProcessor::configuration() const
{
    return m_configuration;
}

void DSPProcessor::process(
    const IQBuffer& input,
    std::vector<float>& outputAudio)
{
    IQBuffer demodulatorInput(input.size());

    if (m_configuration.agcEnabled) {
        m_agc.process(
            input,
            demodulatorInput
            );
    } else {
        demodulatorInput = input;
    }

    switch (m_configuration.mode) {
    case DemodulationMode::AM:
        m_amDemodulator.process(
            demodulatorInput,
            m_demodulatedAudio
            );
        break;

    case DemodulationMode::DSB:
        m_dsbDemodulator.process(
            demodulatorInput,
            m_demodulatedAudio
            );
        break;

    case DemodulationMode::NFM:
        m_nfmDemodulator.process(
            demodulatorInput,
            m_demodulatedAudio
            );
        break;

    case DemodulationMode::WFM:
    case DemodulationMode::USB:
    case DemodulationMode::LSB:
    case DemodulationMode::CW:
        m_demodulatedAudio.assign(
            input.size(),
            0.0f
            );
        break;
    }

    m_audioFilter.process(
        m_demodulatedAudio,
        m_filteredAudio
        );

    m_audioDecimator.process(
        m_filteredAudio,
        outputAudio
        );
}

void DSPProcessor::configureAudioFilter()
{
    constexpr float inputAudioRate =
        256000.0f;

    constexpr int tapCount = 101;

    float audioCutoffHz = 5000.0f;

    switch (m_configuration.mode) {
    case DemodulationMode::AM:
    case DemodulationMode::DSB:
        audioCutoffHz =
            static_cast<float>(
                m_configuration.bandwidthHz
                ) * 0.5f;
        break;

    case DemodulationMode::NFM:
        audioCutoffHz = 6000.0f;
        break;

    case DemodulationMode::USB:
    case DemodulationMode::LSB:
        audioCutoffHz =
            static_cast<float>(
                m_configuration.bandwidthHz
                );
        break;

    case DemodulationMode::CW:
        audioCutoffHz = 1000.0f;
        break;

    case DemodulationMode::WFM:
        audioCutoffHz = 15000.0f;
        break;
    }

    audioCutoffHz = std::clamp(
        audioCutoffHz,
        300.0f,
        15000.0f
        );

    m_audioFilter.setLowPass(
        inputAudioRate,
        audioCutoffHz,
        tapCount
        );
}

} // namespace HFSDR