#include "DSPWorker.h"
#include "Logger.h"

#include <algorithm>

namespace HFSDR
{

DSPWorker::DSPWorker(
    IQBlockRingBuffer* inputBuffer,
    QObject* parent)
    : QObject(parent),
    m_inputBuffer(inputBuffer),
    m_inputBlock(2048),
    m_displayBuffer(256),
    m_receiverBuffer(256)
{
    constexpr float inputSampleRate =
        2048000.0f;

    m_displayDdc.setSampleRate(
        inputSampleRate
        );

    m_displayDdc.setFrequencyShiftHz(
        m_baseFrequencyShiftHz
        );

    // The spectrum display shows most of the
    // 256 kHz decimated output bandwidth.
    m_displayDdc.setLowPassCutoffHz(
        110000.0f
        );

    m_receiverDdc.setSampleRate(
        inputSampleRate
        );

    m_receiverDdc.setFrequencyShiftHz(
        m_baseFrequencyShiftHz
        );

    // Initial AM channel:
    // 10 kHz total RF bandwidth = ±5 kHz.
    m_receiverDdc.setLowPassCutoffHz(
        5000.0f
        );
}

bool DSPWorker::running() const
{
    return m_running.load();
}

void DSPWorker::setConfiguration(
    const ReceiverConfiguration& configuration)
{
    std::lock_guard<std::mutex> lock(
        m_configurationMutex
        );

    if (m_pendingConfiguration ==
        configuration) {
        return;
    }

    m_pendingConfiguration =
        configuration;

    m_configurationDirty = true;
}

void DSPWorker::start()
{
    if (m_running.exchange(true))
        return;

    if (!m_inputBuffer) {
        m_running.store(false);

        emit errorOccurred(
            "DSPWorker has no IQ input buffer."
            );

        return;
    }

    Logger::info("DSP worker started.");

    if (!startAudio()) {
        Logger::warning(
            "DSP processing will continue "
            "without audio output."
            );
    }

    m_spectrumTimer.start();

    emit started();

    while (m_running.load()) {
        if (!receiveNextBlock())
            break;

        applyPendingConfiguration();
        processReceiverPath();

        if (spectrumUpdateDue()) {
            processDisplayPath();
            publishSpectrum();
        }
    }

    stopAudio();

    m_running.store(false);

    Logger::info("DSP worker stopped.");

    emit stopped();
}

void DSPWorker::stop()
{
    m_running.store(false);

    if (m_inputBuffer)
        m_inputBuffer->stop();
}

bool DSPWorker::receiveNextBlock()
{
    if (!m_inputBuffer)
        return false;

    const bool received =
        m_inputBuffer->waitAndPop(
            m_inputBlock
            );

    if (!received && m_running.load()) {
        emit errorOccurred(
            "DSPWorker failed to receive "
            "an IQ block."
            );
    }

    return received;
}

void DSPWorker::applyPendingConfiguration()
{
    ReceiverConfiguration configuration;

    {
        std::lock_guard<std::mutex> lock(
            m_configurationMutex
            );

        if (!m_configurationDirty)
            return;

        configuration =
            m_pendingConfiguration;

        m_configurationDirty = false;
    }

    m_receiverProcessor.setConfiguration(
        configuration
        );

    m_receiverDdc.setFrequencyShiftHz(
        m_baseFrequencyShiftHz +
        static_cast<float>(
            configuration.frequencyOffsetHz
            )
        );

    // bandwidthHz is the complete RF channel width.
    // A complex low-pass DDC therefore uses half
    // that value as its positive-frequency cutoff.
    float channelCutoffHz =
        static_cast<float>(
            configuration.bandwidthHz
            ) * 0.5f;

    channelCutoffHz = std::clamp(
        channelCutoffHz,
        100.0f,
        110000.0f
        );

    m_receiverDdc.setLowPassCutoffHz(
        channelCutoffHz
        );

    Logger::info(
        QString(
            "DSP configuration applied: "
            "mode=%1, bandwidth=%2 Hz, "
            "channel cutoff=%3 Hz, "
            "offset=%4 Hz."
            )
            .arg(
                demodulationModeToString(
                    configuration.mode
                    )
                )
            .arg(
                configuration.bandwidthHz
                )
            .arg(
                channelCutoffHz,
                0,
                'f',
                0
                )
            .arg(
                configuration.frequencyOffsetHz
                )
        );
}

void DSPWorker::processReceiverPath()
{
    m_receiverDdc.process(
        m_inputBlock,
        m_receiverBuffer
        );

    m_receiverProcessor.process(
        m_receiverBuffer,
        m_audioBuffer
        );

    if (m_audioOutput &&
        m_audioOutput->isRunning()) {

        m_audioOutput->writeSamples(
            m_audioBuffer
            );
    }
}

void DSPWorker::processDisplayPath()
{
    m_displayDdc.process(
        m_inputBlock,
        m_displayBuffer
        );

    m_fftProcessor.process(
        m_displayBuffer,
        m_fftBins
        );
}

bool DSPWorker::spectrumUpdateDue()
{
    if (!m_spectrumTimer.isValid()) {
        m_spectrumTimer.start();
        return true;
    }

    if (m_spectrumTimer.elapsed() <
        m_spectrumIntervalMs) {
        return false;
    }

    m_spectrumTimer.restart();
    return true;
}

void DSPWorker::publishSpectrum()
{
    QVariantList spectrum;

    spectrum.reserve(
        static_cast<qsizetype>(
            m_fftBins.size()
            )
        );

    for (const float db : m_fftBins)
        spectrum.append(db);

    emit spectrumReady(spectrum);
}

bool DSPWorker::startAudio()
{
    m_audioOutput =
        std::make_unique<AudioOutput>();

    if (!m_audioOutput->start()) {
        m_audioOutput.reset();
        return false;
    }

    Logger::info(
        "DSP audio output enabled."
        );

    return true;
}

void DSPWorker::stopAudio()
{
    if (!m_audioOutput)
        return;

    m_audioOutput->stop();
    m_audioOutput.reset();

    Logger::info(
        "DSP audio output stopped."
        );
}

} // namespace HFSDR