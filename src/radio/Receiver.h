#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QVariantList>

#include "DSPWorker.h"
#include "IQBlockRingBuffer.h"
#include "IQSource.h"
#include "IQSourceWorker.h"
#include "ReceiverConfiguration.h"
#include "SpectrumWorker.h"

class Receiver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QVariantList spectrumBins READ spectrumBins NOTIFY spectrumBinsChanged)
    Q_PROPERTY(bool simulatorEnabled READ simulatorEnabled WRITE setSimulatorEnabled NOTIFY simulatorEnabledChanged)

public:
    explicit Receiver(
        HFSDR::IQSource* activeSource,
        QObject* parent = nullptr
        );

    ~Receiver() override;

    QString status() const;
    void setStatus(const QString& status);

    QVariantList spectrumBins() const;

    bool simulatorEnabled() const;
    void setSimulatorEnabled(bool enabled);

    HFSDR::IQSource* activeSource();
    bool openActiveSource();

    bool setCenterFrequencyHz(quint64 frequencyHz);
    bool setAutomaticGain(bool enabled);
    bool setRfGainDb(double gainDb);

public slots:
    void startSpectrum();
    void stopSpectrum();

    void setMode(HFSDR::DemodulationMode mode);
    void setRxBandwidthHz(int bandwidthHz);

    void setReceiverConfiguration(
        const HFSDR::ReceiverConfiguration& configuration
        );

    void setSpectrumSpanHz(int spanHz);

    void setSpectrumSmoothingEnabled(bool enabled);
    void setSpectrumSmoothingWindowSize(int windowSize);
    void setSpectrumSmoothingDownwardThresholdDb(double thresholdDb);
    void setSpectrumSmoothingBlend(double blend);

    void setSpectrumFrequencySmoothingEnabled(bool enabled);
    void setSpectrumFrequencySmoothingRadius(int radius);
    void setSpectrumFrequencySmoothingStrength(double strength);

signals:
    void statusChanged();
    void spectrumBinsChanged();
    void simulatorEnabledChanged();

private slots:
    void handleSpectrumReady(
        const QVariantList& spectrumBins
        );

private:
    void createWorkers();
    void stopWorkers();
    void publishConfiguration();

    QString m_status = "Idle";
    QVariantList m_spectrumBins;

    HFSDR::IQSource* m_activeSource = nullptr;

    // The receiver/audio and spectrum paths have independent queues.
    HFSDR::IQBlockRingBuffer m_receiverIqBuffer;
    HFSDR::IQBlockRingBuffer m_spectrumIqBuffer;

    QThread m_sourceThread;
    IQSourceWorker* m_sourceWorker = nullptr;

    QThread m_dspThread;
    HFSDR::DSPWorker* m_dspWorker = nullptr;

    QThread m_spectrumThread;
    HFSDR::SpectrumWorker* m_spectrumWorker = nullptr;

    int m_spectrumSpanHz = 250000;
    HFSDR::ReceiverConfiguration m_configuration;
    bool m_simulatorEnabled = false;
};

#endif
