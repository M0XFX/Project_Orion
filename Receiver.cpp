#include "Receiver.h"
#include "Logger.h"

#include <cmath>

Receiver::Receiver(QObject* parent)
    : QObject(parent),
    m_iqBuffer(2048),
    m_displayBuffer(256),
    m_receiverBuffer(256),
    m_audioIqBuffer(256),
    m_iqRingBuffer(64)
{
    m_activeSource = &m_rtlDevice;
    m_displayDdc.setSampleRate(2048000.0f);
    m_displayDdc.setFrequencyShiftHz(100000.0f);
    m_receiverDdc.setSampleRate(2048000.0f);
    m_receiverDdc.setFrequencyShiftHz(100000.0f);
    connect(&m_spectrumTimer, &QTimer::timeout, this, &Receiver::updateSpectrum);

    m_spectrumTimer.setInterval(33);

    m_sourceWorker = new IQSourceWorker(
        m_activeSource,
        &m_iqRingBuffer
        );

    m_sourceWorker->moveToThread(
        &m_sourceThread
        );

    connect(
        &m_sourceThread,
        &QThread::started,
        m_sourceWorker,
        &IQSourceWorker::start
        );

    connect(
        m_sourceWorker,
        &IQSourceWorker::stopped,
        &m_sourceThread,
        &QThread::quit
        );

    connect(
        m_sourceWorker,
        &IQSourceWorker::errorOccurred,
        this,
        [](const QString& message) {
            HFSDR::Logger::error(message);
        }
        );

    connect(
        &m_sourceThread,
        &QThread::finished,
        this,
        []() {
            HFSDR::Logger::info(
                "IQ source worker thread finished."
                );
        }
        );
}

Receiver::~Receiver()
{
    m_spectrumTimer.stop();

    if (m_sourceWorker)
        m_sourceWorker->stop();

    m_iqRingBuffer.stop();

    if (m_sourceThread.isRunning()) {
        m_sourceThread.quit();
        m_sourceThread.wait();
    }

    delete m_sourceWorker;
    m_sourceWorker = nullptr;
}


QString Receiver::status() const
{
    return m_status;
}

void Receiver::setStatus(const QString &status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged();
}

QVariantList Receiver::spectrumBins() const
{
    return m_spectrumBins;
}

void Receiver::startSpectrum()
{
    HFSDR::Logger::info("Starting spectrum display.");

    setStatus("Simulated IQ Source");

    m_spectrumTimer.start();
}

void Receiver::stopSpectrum()
{
    HFSDR::Logger::info("Stopping simulated spectrum.");

    m_spectrumTimer.stop();
    setStatus("Stopped");
}

void Receiver::updateSpectrum()
{

    bool gotSamples = false;

    if (m_rtlDevice.connected()) {
        // Drain the queue and retain the newest available IQ block.
        // The spectrum display does not need to process every historical block.
        while (m_iqRingBuffer.pop(m_iqBuffer)) {
            gotSamples = true;
        }
    }





    if (!gotSamples && m_simulatorEnabled) {
        m_signalGenerator.generate(m_iqBuffer, m_sampleRate, m_toneFrequency);
        gotSamples = true;
    }

    if (!gotSamples) {
        m_fftBins.clear();
        m_spectrumBins.clear();
        emit spectrumBinsChanged();
        return;
    }

    //m_ddc.process(m_iqBuffer, m_ddcBuffer);
    //m_dspProcessor.process(m_ddcBuffer, m_dspBuffer);
    //m_fftProcessor.process(m_dspBuffer, m_fftBins);

    //m_displayDdc.process(m_iqBuffer, m_displayBuffer);
    //m_fftProcessor.process(m_displayBuffer, m_fftBins);

    // Display chain
    m_displayDdc.process(m_iqBuffer, m_displayBuffer);

    // Receiver chain
    m_receiverDdc.process(m_iqBuffer, m_receiverBuffer);

    // FFT uses the display path
    m_fftProcessor.process(m_displayBuffer, m_fftBins);

    // DSP (soon this will become AGC + demodulator)
    m_dspProcessor.process(m_receiverBuffer, m_audioBuffer);

    m_spectrumBins.clear();
    for (float db : m_fftBins)
        m_spectrumBins.append(db);
    emit spectrumBinsChanged();
}




QObject* Receiver::rtlDevice()
{
    return &m_rtlDevice;
}

void Receiver::openRtlDevice()
{
    m_rtlDevice.open();
    setStatus(m_rtlDevice.status());

    if (!m_rtlDevice.connected())
        return;

    if (!m_sourceThread.isRunning()) {
        HFSDR::Logger::info(
            "Starting IQ source worker thread."
            );

        m_sourceThread.start();
    }
}

bool Receiver::simulatorEnabled() const
{
    return m_simulatorEnabled;
}

void Receiver::setSimulatorEnabled(bool enabled)
{
    if (m_simulatorEnabled == enabled)
        return;

    m_simulatorEnabled = enabled;
    emit simulatorEnabledChanged();
}

