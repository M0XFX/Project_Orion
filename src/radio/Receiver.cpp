#include "Receiver.h"
#include "Logger.h"

Receiver::Receiver(
    HFSDR::IQSource* activeSource,
    QObject* parent)
    : QObject(parent),
      m_activeSource(activeSource),
      m_receiverIqBuffer(64),
      m_spectrumIqBuffer(16)
{
    createWorkers();
    publishConfiguration();

    HFSDR::Logger::info(
        "Receiver initialised with independent DSP and spectrum workers."
        );
}

Receiver::~Receiver()
{
    stopWorkers();

    if (m_activeSource)
        m_activeSource->close();
}

void Receiver::createWorkers()
{
    m_sourceWorker =
        new IQSourceWorker(
            m_activeSource,
            &m_receiverIqBuffer,
            &m_spectrumIqBuffer
            );

    m_sourceWorker->moveToThread(&m_sourceThread);

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
            &m_receiverIqBuffer
            );

    m_dspWorker->moveToThread(&m_dspThread);

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
        &HFSDR::DSPWorker::errorOccurred,
        this,
        [](const QString& message) {
            HFSDR::Logger::error(message);
        }
        );

    m_spectrumWorker =
        new HFSDR::SpectrumWorker(
            &m_spectrumIqBuffer
            );

    m_spectrumWorker->setSpectrumSpanHz(
        m_spectrumSpanHz
        );

    m_spectrumWorker->moveToThread(
        &m_spectrumThread
        );

    connect(
        &m_spectrumThread,
        &QThread::started,
        m_spectrumWorker,
        &HFSDR::SpectrumWorker::start
        );

    connect(
        m_spectrumWorker,
        &HFSDR::SpectrumWorker::stopped,
        &m_spectrumThread,
        &QThread::quit
        );

    connect(
        m_spectrumWorker,
        &HFSDR::SpectrumWorker::spectrumReady,
        this,
        &Receiver::handleSpectrumReady,
        Qt::QueuedConnection
        );

    connect(
        m_spectrumWorker,
        &HFSDR::SpectrumWorker::errorOccurred,
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

    if (m_spectrumWorker)
        m_spectrumWorker->stop();

    m_receiverIqBuffer.stop();
    m_spectrumIqBuffer.stop();

    if (m_sourceThread.isRunning()) {
        m_sourceThread.quit();
        m_sourceThread.wait();
    }

    if (m_dspThread.isRunning()) {
        m_dspThread.quit();
        m_dspThread.wait();
    }

    if (m_spectrumThread.isRunning()) {
        m_spectrumThread.quit();
        m_spectrumThread.wait();
    }

    delete m_sourceWorker;
    m_sourceWorker = nullptr;

    delete m_dspWorker;
    m_dspWorker = nullptr;

    delete m_spectrumWorker;
    m_spectrumWorker = nullptr;
}

QString Receiver::status() const
{
    return m_status;
}

void Receiver::setStatus(const QString& status)
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

void Receiver::startSpectrum()
{
    HFSDR::Logger::info(
        "Spectrum display enabled."
        );
}

void Receiver::stopSpectrum()
{
    m_spectrumBins.clear();
    emit spectrumBinsChanged();
}

bool Receiver::openActiveSource()
{
    if (!m_activeSource) {
        setStatus("No SDR Selected");
        return false;
    }

    m_activeSource->open();

    if (!m_activeSource->connected()) {
        setStatus("SDR Open Failed");
        return false;
    }

    setStatus("SDR Connected");

    // Start consumers before the source begins filling their queues.
    if (!m_dspThread.isRunning()) {
        HFSDR::Logger::info(
            "Starting receiver DSP worker thread."
            );
        m_dspThread.start();
    }

    if (!m_spectrumThread.isRunning()) {
        HFSDR::Logger::info(
            "Starting spectrum worker thread."
            );
        m_spectrumThread.start();
    }

    if (!m_sourceThread.isRunning()) {
        HFSDR::Logger::info(
            "Starting IQ source worker thread."
            );
        m_sourceThread.start();
    }

    return true;
}

HFSDR::IQSource* Receiver::activeSource()
{
    return m_activeSource;
}

bool Receiver::setCenterFrequencyHz(quint64 frequencyHz)
{
    return m_activeSource &&
           m_activeSource->setCenterFrequencyHz(frequencyHz);
}

bool Receiver::setAutomaticGain(bool enabled)
{
    return m_activeSource &&
           m_activeSource->setAutomaticGain(enabled);
}

bool Receiver::setRfGainDb(double gainDb)
{
    return m_activeSource &&
           m_activeSource->setGainDb(gainDb);
}

void Receiver::setMode(HFSDR::DemodulationMode mode)
{
    if (m_configuration.mode == mode)
        return;

    m_configuration.mode = mode;
    publishConfiguration();
}

void Receiver::setRxBandwidthHz(int bandwidthHz)
{
    if (bandwidthHz < 50)
        bandwidthHz = 50;

    if (bandwidthHz > 250000)
        bandwidthHz = 250000;

    if (m_configuration.bandwidthHz == bandwidthHz)
        return;

    m_configuration.bandwidthHz = bandwidthHz;
    publishConfiguration();
}

void Receiver::setSpectrumSpanHz(int spanHz)
{
    if (spanHz < 10000)
        spanHz = 10000;

    if (m_spectrumSpanHz == spanHz)
        return;

    m_spectrumSpanHz = spanHz;

    if (m_spectrumWorker) {
        m_spectrumWorker->setSpectrumSpanHz(
            m_spectrumSpanHz
            );
    }

    HFSDR::Logger::info(
        QString(
            "Receiver spectrum span request set to %1 Hz."
            ).arg(m_spectrumSpanHz)
        );
}


void Receiver::setSpectrumSmoothingEnabled(bool enabled)
{
    if (m_spectrumWorker)
        m_spectrumWorker->setSmoothingEnabled(enabled);
}

void Receiver::setSpectrumSmoothingWindowSize(int windowSize)
{
    if (m_spectrumWorker)
        m_spectrumWorker->setSmoothingWindowSize(windowSize);
}

void Receiver::setSpectrumSmoothingDownwardThresholdDb(double thresholdDb)
{
    if (m_spectrumWorker) {
        m_spectrumWorker->setSmoothingDownwardThresholdDb(
            static_cast<float>(thresholdDb)
            );
    }
}

void Receiver::setSpectrumSmoothingBlend(double blend)
{
    if (m_spectrumWorker)
        m_spectrumWorker->setSmoothingBlend(static_cast<float>(blend));
}

void Receiver::setReceiverConfiguration(
    const HFSDR::ReceiverConfiguration& configuration)
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

    m_dspWorker->setConfiguration(m_configuration);
}

void Receiver::handleSpectrumReady(
    const QVariantList& spectrumBins)
{
    m_spectrumBins = spectrumBins;
    emit spectrumBinsChanged();
}
