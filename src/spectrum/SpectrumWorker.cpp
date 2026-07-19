#include "SpectrumWorker.h"

#include "Logger.h"

namespace HFSDR
{

SpectrumWorker::SpectrumWorker(
    IQBlockRingBuffer* inputBuffer,
    QObject* parent)
    : QObject(parent),
      m_inputBuffer(inputBuffer),
      m_accumulator(4096),
      m_engine(4096),
      m_inputBlock(2048)
{
    constexpr float inputSampleRate = 2048000.0f;

    m_displayDdc.setSampleRate(inputSampleRate);
    m_displayDdc.setFrequencyShiftHz(
        m_baseFrequencyShiftHz
        );

    m_engine.setDetectorMode(
        SpectrumDetectorMode::Average
        );

    m_engine.setAveragingAlpha(0.05f);
}

bool SpectrumWorker::running() const
{
    return m_running.load();
}

void SpectrumWorker::setSpectrumSpanHz(int spanHz)
{
    if (spanHz < 10000)
        spanHz = 10000;

    std::lock_guard<std::mutex> lock(
        m_spectrumSpanMutex
        );

    if (m_pendingSpectrumSpanHz == spanHz)
        return;

    m_pendingSpectrumSpanHz = spanHz;
    m_spectrumSpanDirty = true;
}

void SpectrumWorker::start()
{
    if (m_running.exchange(true))
        return;

    if (!m_inputBuffer) {
        m_running.store(false);
        emit errorOccurred(
            "SpectrumWorker has no IQ input buffer."
            );
        return;
    }

    Logger::info(
        "Spectrum worker started with 4096-point FFT."
        );

    m_publishTimer.start();
    emit started();

    while (m_running.load()) {
        if (!receiveNextBlock())
            break;

        applyPendingSpectrumSpan();

        m_displayDdc.process(
            m_inputBlock,
            m_decimatedBlock
            );

        if (!m_accumulator.append(
                m_decimatedBlock,
                m_fftFrame)) {
            continue;
        }

        // Complete FFT frames are accumulated continuously, but the
        // expensive FFT and QML publication are rate-limited. This keeps
        // the spectrum smooth without coupling display load to audio DSP.
        if (!spectrumUpdateDue())
            continue;

        if (!m_engine.processFrame(
                m_fftFrame,
                m_fftBins)) {
            continue;
        }

        publishSpectrum();
    }

    m_running.store(false);

    Logger::info("Spectrum worker stopped.");
    emit stopped();
}

void SpectrumWorker::stop()
{
    m_running.store(false);

    if (m_inputBuffer)
        m_inputBuffer->stop();
}

bool SpectrumWorker::receiveNextBlock()
{
    if (!m_inputBuffer)
        return false;

    const bool received =
        m_inputBuffer->waitAndPop(m_inputBlock);

    if (!received && m_running.load()) {
        emit errorOccurred(
            "SpectrumWorker failed to receive an IQ block."
            );
    }

    return received;
}

void SpectrumWorker::applyPendingSpectrumSpan()
{
    int requestedSpanHz = 0;

    {
        std::lock_guard<std::mutex> lock(
            m_spectrumSpanMutex
            );

        if (!m_spectrumSpanDirty)
            return;

        requestedSpanHz = m_pendingSpectrumSpanHz;
        m_spectrumSpanDirty = false;
    }

    const float sourceSampleRateHz =
        m_displayDdc.sampleRate();

    int decimationFactor = 1;

    if (requestedSpanHz <= 100000) {
        decimationFactor = 16;
    } else if (requestedSpanHz <= 250000) {
        decimationFactor = 8;
    } else if (requestedSpanHz <= 500000) {
        decimationFactor = 4;
    } else if (requestedSpanHz <= 1000000) {
        decimationFactor = 2;
    }

    m_displayDdc.setDecimationFactor(
        decimationFactor
        );

    const float displaySampleRateHz =
        m_displayDdc.outputSampleRate();

    const float displayCutoffHz =
        displaySampleRateHz * 0.45f;

    m_displayDdc.setLowPassCutoffHz(
        displayCutoffHz
        );

    m_spectrumSpanHz =
        static_cast<int>(displaySampleRateHz);

    m_accumulator.reset();
    m_engine.reset();

    Logger::info(
        QString(
            "Spectrum engine configured: "
            "requested=%1 Hz, source=%2 Hz, "
            "decimation=%3, applied=%4 Hz, "
            "FFT=4096, bin width=%5 Hz."
            )
            .arg(requestedSpanHz)
            .arg(sourceSampleRateHz, 0, 'f', 0)
            .arg(decimationFactor)
            .arg(m_spectrumSpanHz)
            .arg(
                displaySampleRateHz /
                    static_cast<float>(
                        m_engine.fftSize()),
                0,
                'f',
                2
                )
        );
}

bool SpectrumWorker::spectrumUpdateDue()
{
    if (!m_publishTimer.isValid()) {
        m_publishTimer.start();
        return true;
    }

    if (m_publishTimer.elapsed() <
        m_publishIntervalMs) {
        return false;
    }

    m_publishTimer.restart();
    return true;
}

void SpectrumWorker::publishSpectrum()
{
    QVariantList spectrum;

    spectrum.reserve(
        static_cast<qsizetype>(m_fftBins.size())
        );

    for (const float dbfs : m_fftBins)
        spectrum.append(dbfs);

    emit spectrumReady(spectrum);
}

} // namespace HFSDR
