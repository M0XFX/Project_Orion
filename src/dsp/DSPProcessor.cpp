#include "DSPProcessor.h"

#include <algorithm>

namespace HFSDR
{

DSPProcessor::DSPProcessor()
{
    // Normal AM/FM audio path:
    // 256 kS/s to 32 kS/s.
    m_audioDecimator.setFactor(8);

    // SSB complex-IQ path:
    // 256 kS/s to 32 kS/s before applying
    // the narrow complex sideband filter.
    m_ssbPreFilter.setLowPass(
        256000.0f,
        12000.0f,
        129
        );

    m_ssbDecimator.setFactor(8);

    m_wfmDemodulator.setSampleRate(
        256000.0f
        );

    m_wfmDemodulator.setDeviationHz(
        75000.0f
        );

    m_wfmDemodulator
        .setDeEmphasisTimeConstant(
            50.0e-6f
            );

    m_productDetector.setMode(
        DemodulationMode::USB
        );

    configureSidebandFilter();
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

    if (previousMode !=
        m_configuration.mode) {

        switch (m_configuration.mode) {
        case DemodulationMode::NFM:
            m_nfmDemodulator.reset();
            break;

        case DemodulationMode::WFM:
            m_wfmDemodulator.reset();
            break;

        case DemodulationMode::USB:
        case DemodulationMode::LSB:
        case DemodulationMode::CW:
            m_productDetector.setMode(
                m_configuration.mode
                );

            m_productDetector.reset();
            m_sidebandFilter.reset();
            break;

        case DemodulationMode::AM:
        case DemodulationMode::DSB:
            break;
        }
    }

    configureSidebandFilter();
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
    IQBuffer demodulatorInput(
        input.size()
        );

    const bool useAgc =
        m_configuration.agcEnabled &&
        (
            m_configuration.mode ==
                DemodulationMode::AM ||
            m_configuration.mode ==
                DemodulationMode::DSB ||
            m_configuration.mode ==
                DemodulationMode::USB ||
            m_configuration.mode ==
                DemodulationMode::LSB ||
            m_configuration.mode ==
                DemodulationMode::CW
            );

    if (useAgc) {
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
        m_wfmDemodulator.process(
            demodulatorInput,
            m_demodulatedAudio
            );
        break;

    case DemodulationMode::USB:
    case DemodulationMode::LSB:
        processSidebandMode(
            demodulatorInput,
            outputAudio
            );
        return;

    case DemodulationMode::CW:
        outputAudio.assign(
            input.size() / 8,
            0.0f
            );
        return;
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

void DSPProcessor::processSidebandMode(
    const IQBuffer& input,
    std::vector<float>& outputAudio)
{
    m_ssbPreFilter.process(
        input,
        m_ssbPreFilteredIq
        );

    m_ssbDecimator.process(
        m_ssbPreFilteredIq,
        m_ssbDecimatedIq
        );

    m_sidebandFilter.process(
        m_ssbDecimatedIq,
        m_sidebandFilteredIq
        );

    m_productDetector.process(
        m_sidebandFilteredIq,
        m_demodulatedAudio
        );

    // The sideband IQ is already at the final
    // 32 kHz audio rate, so no further audio
    // decimation is required.
    m_audioFilter.process(
        m_demodulatedAudio,
        outputAudio
        );
}

void DSPProcessor::configureSidebandFilter()
{
    constexpr float sidebandSampleRate =
        32000.0f;

    constexpr float lowAudioEdgeHz =
        300.0f;

    constexpr int tapCount = 257;

    float wantedBandwidthHz =
        static_cast<float>(
            m_configuration.bandwidthHz
            );

    wantedBandwidthHz = std::clamp(
        wantedBandwidthHz,
        300.0f,
        5000.0f
        );

    const float highAudioEdgeHz =
        lowAudioEdgeHz +
        wantedBandwidthHz;

    if (m_configuration.mode ==
        DemodulationMode::LSB) {

        m_sidebandFilter.setBandPass(
            sidebandSampleRate,
            -highAudioEdgeHz,
            -lowAudioEdgeHz,
            tapCount
            );
    } else {
        // USB is also the default preparation
        // for the future CW path.
        m_sidebandFilter.setBandPass(
            sidebandSampleRate,
            lowAudioEdgeHz,
            highAudioEdgeHz,
            tapCount
            );
    }
}

void DSPProcessor::configureAudioFilter()
{
    constexpr int tapCount = 101;

    float inputAudioRate =
        256000.0f;

    float audioCutoffHz =
        5000.0f;

    switch (m_configuration.mode) {
    case DemodulationMode::AM:
    case DemodulationMode::DSB:
    case DemodulationMode::NFM:
        audioCutoffHz =
            static_cast<float>(
                m_configuration.bandwidthHz
                ) * 0.5f;
        break;

    case DemodulationMode::WFM:
        audioCutoffHz = 14000.0f;
        break;

    case DemodulationMode::USB:
    case DemodulationMode::LSB:
        inputAudioRate = 32000.0f;

        audioCutoffHz =
            300.0f +
            static_cast<float>(
                m_configuration.bandwidthHz
                );
        break;

    case DemodulationMode::CW:
        inputAudioRate = 32000.0f;
        audioCutoffHz = 1200.0f;
        break;
    }

    audioCutoffHz = std::clamp(
        audioCutoffHz,
        300.0f,
        14000.0f
        );

    m_audioFilter.setLowPass(
        inputAudioRate,
        audioCutoffHz,
        tapCount
        );
}

} // namespace HFSDR