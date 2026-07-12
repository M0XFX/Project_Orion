#include "Receiver.h"
#include "Logger.h"

Receiver::Receiver(QObject* parent)
    : QObject(parent),
    m_iqRingBuffer(64)
{
    m_activeSource = &m_rtlDevice;

    createWorkers();

    HFSDR::Logger::info(
        "Receiver initialised."
        );
}

Receiver::~Receiver()
{
    stopWorkers();
}

void Receiver::createWorkers()
{
    //
    // IQ source worker
    //

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

    //
    // DSP worker
    //

    m_dspWorker = new HFSDR::DSPWorker(
        &m_iqRingBuffer
        );

    m_dspWorker->moveToThread(
        &m_dspThread
        );

    connect(
        &m_dspThread,
        &QThread::started,
        m_dspWorker,
        &HFSDR::DSPWorker::start
        );

    connect(
        m_dspWorker,
        &HFSDR::DSPWorker::stopped,
        &m_dspThread,
        &QThread::quit
        );

    connect(
        m_dspWorker,
        &HFSDR::DSPWorker::spectrumReady,
        this,
        &Receiver::handleSpectrumReady,
        Qt::QueuedConnection
        );

    connect(
        m_dspWorker,
        &HFSDR::DSPWorker::errorOccurred,
        this,
        [](const QString& message) {
            HFSDR::Logger::error(message);
        }
        );

    connect(
        &m_dspThread,
        &QThread::finished,
        this,
        []() {
            HFSDR::Logger::info(
                "DSP worker thread finished."
                );
        }
        );
}

void Receiver::stopWorkers()
{
    //
    // Stop acquisition first so no more IQ blocks
    // are written into the ring buffer.
    //

    if (m_sourceWorker)
        m_sourceWorker->stop();

    //
    // Wake and stop DSPWorker if it is blocked
    // inside waitAndPop().
    //

    if (m_dspWorker)
        m_dspWorker->stop();

    m_iqRingBuffer.stop();

    if (m_sourceThread.isRunning()) {
        m_sourceThread.quit();
        m_sourceThread.wait();
    }

    if (m_dspThread.isRunning()) {
        m_dspThread.quit();
        m_dspThread.wait();
    }

    delete m_sourceWorker;
    m_sourceWorker = nullptr;

    delete m_dspWorker;
    m_dspWorker = nullptr;
}

QString Receiver::status() const
{
    return m_status;
}

void Receiver::setStatus(
    const QString& status)
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

QObject* Receiver::rtlDevice()
{
    return &m_rtlDevice;
}

bool Receiver::simulatorEnabled() const
{
    return m_simulatorEnabled;
}

void Receiver::setSimulatorEnabled(
    bool enabled)
{
    if (m_simulatorEnabled == enabled)
        return;

    m_simulatorEnabled = enabled;
    emit simulatorEnabledChanged();

    HFSDR::Logger::info(
        enabled
            ? "IQ simulator enabled."
            : "IQ simulator disabled."
        );
}

void Receiver::startSpectrum()
{
    HFSDR::Logger::info(
        "Spectrum display ready."
        );
}

void Receiver::stopSpectrum()
{
    HFSDR::Logger::info(
        "Spectrum display stopped."
        );

    m_spectrumBins.clear();
    emit spectrumBinsChanged();
}

void Receiver::openRtlDevice()
{
    m_rtlDevice.open();
    setStatus(m_rtlDevice.status());

    if (!m_rtlDevice.connected())
        return;

    //
    // Start the DSP consumer first so it is waiting
    // before the source begins producing IQ blocks.
    //

    if (!m_dspThread.isRunning()) {
        HFSDR::Logger::info(
            "Starting DSP worker thread."
            );

        m_dspThread.start();
    }

    if (!m_sourceThread.isRunning()) {
        HFSDR::Logger::info(
            "Starting IQ source worker thread."
            );

        m_sourceThread.start();
    }
}

void Receiver::handleSpectrumReady(
    const QVariantList& spectrumBins)
{
    m_spectrumBins = spectrumBins;
    emit spectrumBinsChanged();
}