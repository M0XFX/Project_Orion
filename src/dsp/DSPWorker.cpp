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

    /*
 * Medium FFT averaging.
 *
 * 1.00 = Off
 * 0.40 = Fast
 * 0.20 = Medium
 * 0.08 = Slow
 */
    m_fftProcessor.setAveragingAlpha(
        0.20f
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

void DSPWorker::setSpectrumSpanHz(
    int spanHz)
{
    if (spanHz < 10000)
        spanHz = 10000;

    std::lock_guard<std::mutex> lock(
        m_spectrumSpanMutex
        );

    if (m_pendingSpectrumSpanHz ==
        spanHz) {
        return;
    }

    m_pendingSpectrumSpanHz = spanHz;
    m_spectrumSpanDirty = true;
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
        applyPendingSpectrumSpan();

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
    /*
    float channelCutoffHz =
        static_cast<float>(
            configuration.bandwidthHz
            ) * 0.5f;

*/
    float channelCutoffHz = 0.0f;

    switch (configuration.mode) {
    case DemodulationMode::USB:
    case DemodulationMode::LSB:
    case DemodulationMode::CW:
        /*
     * Keep a wider complex channel before
     * DSPProcessor performs its own 32 kHz
     * sideband selection.
     */
        channelCutoffHz = 12000.0f;
        break;

    case DemodulationMode::AM:
    case DemodulationMode::DSB:
    case DemodulationMode::NFM:
    case DemodulationMode::WFM:
        channelCutoffHz =
            static_cast<float>(
                configuration.bandwidthHz
                ) * 0.5f;
        break;
    }

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

void DSPWorker::applyPendingSpectrumSpan()
{
    int requestedSpanHz = 0;

    {
        std::lock_guard<std::mutex> lock(
            m_spectrumSpanMutex
            );

        if (!m_spectrumSpanDirty)
            return;

        requestedSpanHz =
            m_pendingSpectrumSpanHz;

        m_spectrumSpanDirty = false;
    }

    /*
     * Select a practical Display DDC decimation factor.
     *
     * The DDC output sample rate becomes the actual
     * spectrum width currently shown by the FFT.
     *
     * With the RTL-SDR at 2.048 MS/s:
     *
     *   Decimation 1  = 2.048 MHz span
     *   Decimation 2  = 1.024 MHz span
     *   Decimation 4  = 512 kHz span
     *   Decimation 8  = 256 kHz span
     *   Decimation 16 = 128 kHz span
     */

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
    } else {
        decimationFactor = 1;
    }

    m_displayDdc.setDecimationFactor(
        decimationFactor
        );

    const float displaySampleRateHz =
        m_displayDdc.outputSampleRate();

    /*
     * Keep the anti-alias filter slightly inside the
     * output Nyquist limit.
     *
     * A complex output stream represents a total span
     * approximately equal to its sample rate.
     */
    const float displayCutoffHz =
        displaySampleRateHz * 0.45f;

    m_displayDdc.setLowPassCutoffHz(
        displayCutoffHz
        );

    /*
     * Until FFT cropping is added, the actual visible
     * span equals the Display DDC output sample rate.
     */
    m_spectrumSpanHz =
        static_cast<int>(
            displaySampleRateHz
            );

    Logger::info(
        QString(
            "Display spectrum configured: "
            "requested=%1 Hz, "
            "source rate=%2 Hz, "
            "decimation=%3, "
            "applied span=%4 Hz, "
            "filter cutoff=%5 Hz."
            )
            .arg(requestedSpanHz)
            .arg(
                sourceSampleRateHz,
                0,
                'f',
                0
                )
            .arg(decimationFactor)
            .arg(m_spectrumSpanHz)
            .arg(
                displayCutoffHz,
                0,
                'f',
                0
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