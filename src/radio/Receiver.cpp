#include "Receiver.h"
#include "Logger.h"

Receiver::Receiver(QObject* parent)
    : QObject(parent),
    m_iqRingBuffer(64)
{
    m_activeSource = &m_rtlDevice;

    createWorkers();
    publishConfiguration();

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
    m_sourceWorker =
        new IQSourceWorker(
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

    m_dspWorker =
        new HFSDR::DSPWorker(
            &m_iqRingBuffer
            );

    m_dspWorker->setSpectrumSpanHz(
        m_spectrumSpanHz
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
}

void Receiver::stopWorkers()
{
    if (m_sourceWorker)
        m_sourceWorker->stop();

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
}

void Receiver::startSpectrum()
{
    HFSDR::Logger::info(
        "Spectrum display ready."
        );
}

void Receiver::stopSpectrum()
{
    m_spectrumBins.clear();
    emit spectrumBinsChanged();
}

void Receiver::openRtlDevice()
{
    m_rtlDevice.open();
    setStatus(m_rtlDevice.status());

    if (!m_rtlDevice.connected())
        return;

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

void Receiver::setMode(
    HFSDR::DemodulationMode mode)
{
    if (m_configuration.mode == mode)
        return;

    m_configuration.mode = mode;
    publishConfiguration();
}

void Receiver::setRxBandwidthHz(
    int bandwidthHz)
{
    if (bandwidthHz < 50)
        bandwidthHz = 50;

    if (bandwidthHz > 250000)
        bandwidthHz = 250000;

    if (m_configuration.bandwidthHz ==
        bandwidthHz) {
        return;
    }

    m_configuration.bandwidthHz =
        bandwidthHz;

    publishConfiguration();
}

void Receiver::setSpectrumSpanHz(
    int spanHz)
{
    if (spanHz < 10000)
        spanHz = 10000;

    if (m_spectrumSpanHz == spanHz)
        return;

    m_spectrumSpanHz = spanHz;

    if (m_dspWorker) {
        m_dspWorker->setSpectrumSpanHz(
            m_spectrumSpanHz
            );
    }

    HFSDR::Logger::info(
        QString(
            "Receiver spectrum span set to %1 Hz."
            ).arg(m_spectrumSpanHz)
        );
}










void Receiver::setReceiverConfiguration(
    const HFSDR::ReceiverConfiguration&
        configuration)
{
    if (m_configuration == configuration)
        return;

    m_configuration = configuration;
    publishConfiguration();
}

void Receiver::publishConfiguration()
{
    if (!m_dspWorker)
        return;

    m_dspWorker->setConfiguration(
        m_configuration
        );
}

void Receiver::handleSpectrumReady(
    const QVariantList& spectrumBins)
{
    m_spectrumBins = spectrumBins;
    emit spectrumBinsChanged();
}