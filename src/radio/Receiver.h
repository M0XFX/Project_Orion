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
#include "RTLDevice.h"

class Receiver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QString status
            READ status
                WRITE setStatus
                    NOTIFY statusChanged
        )

    Q_PROPERTY(
        QVariantList spectrumBins
            READ spectrumBins
                NOTIFY spectrumBinsChanged
        )

    Q_PROPERTY(
        QObject* rtlDevice
            READ rtlDevice
                CONSTANT
        )

    Q_PROPERTY(
        bool simulatorEnabled
            READ simulatorEnabled
                WRITE setSimulatorEnabled
                    NOTIFY simulatorEnabledChanged
        )

public:
    explicit Receiver(QObject* parent = nullptr);
    ~Receiver() override;

    QString status() const;
    void setStatus(const QString& status);

    QVariantList spectrumBins() const;

    QObject* rtlDevice();

    bool simulatorEnabled() const;
    void setSimulatorEnabled(bool enabled);

public slots:
    void startSpectrum();
    void stopSpectrum();
    void openRtlDevice();

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

    QString m_status = "Idle";
    QVariantList m_spectrumBins;

    RTLDevice m_rtlDevice;
    HFSDR::IQSource* m_activeSource = nullptr;

    HFSDR::IQBlockRingBuffer m_iqRingBuffer;

    QThread m_sourceThread;
    IQSourceWorker* m_sourceWorker = nullptr;

    QThread m_dspThread;
    HFSDR::DSPWorker* m_dspWorker = nullptr;

    bool m_simulatorEnabled = false;
};

#endif