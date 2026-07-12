#include "DSPWorker.h"
#include "Logger.h"

namespace HFSDR
{

DSPWorker::DSPWorker(
    IQBlockRingBuffer* inputBuffer,
    QObject* parent
    )
    : QObject(parent),
    m_inputBuffer(inputBuffer),
    m_inputBlock(2048),
    m_displayBuffer(256),
    m_receiverBuffer(256)
{
    constexpr float inputSampleRate = 2048000.0f;
    constexpr float frequencyShiftHz = 100000.0f;

    m_displayDdc.setSampleRate(inputSampleRate);
    m_displayDdc.setFrequencyShiftHz(
        frequencyShiftHz
        );

    m_receiverDdc.setSampleRate(inputSampleRate);
    m_receiverDdc.setFrequencyShiftHz(
        frequencyShiftHz
        );
}

bool DSPWorker::running() const
{
    return m_running.load();
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

    HFSDR::Logger::info(
        "DSP worker started."
        );

    if (!startAudio()) {
        HFSDR::Logger::warning(
            "DSP processing will continue without audio output."
            );
    }

    m_spectrumTimer.start();

    emit started();

    while (m_running.load()) {
        if (!receiveNextBlock())
            break;

        processReceiverPath();

        if (spectrumUpdateDue()) {
            processDisplayPath();
            publishSpectrum();
        }
    }

    stopAudio();

    m_running.store(false);

    HFSDR::Logger::info(
        "DSP worker stopped."
        );

    emit stopped();
}

void DSPWorker::stop()
{
    m_running.store(false);

    // Wake waitAndPop() if DSPWorker is blocked waiting
    // for another IQ block.
    if (m_inputBuffer)
        m_inputBuffer->stop();
}

bool DSPWorker::receiveNextBlock()
{
    if (!m_inputBuffer)
        return false;

    const bool received =
        m_inputBuffer->waitAndPop(m_inputBlock);

    if (!received && m_running.load()) {
        emit errorOccurred(
            "DSPWorker failed to receive an IQ block."
            );
    }

    return received;
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
    // AudioOutput must be created here because start()
    // executes inside the DSP thread.
    m_audioOutput =
        std::make_unique<AudioOutput>();

    if (!m_audioOutput->start()) {
        m_audioOutput.reset();
        return false;
    }

    HFSDR::Logger::info(
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

    HFSDR::Logger::info(
        "DSP audio output stopped."
        );
}

} // namespace HFSDR