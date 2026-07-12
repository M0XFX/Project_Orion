#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QTimer>
#include <vector>
#include "RTLDevice.h"
#include "IQBuffer.h"
#include "SignalGenerator.h"
#include "DSPProcessor.h"
#include "FFTProcessor.h"
#include "DigitalDownConverter.h"
#include <QThread>
#include "IQBlockRingBuffer.h"
#include "IQSourceWorker.h"



class Receiver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QVariantList spectrumBins READ spectrumBins NOTIFY spectrumBinsChanged)
    Q_PROPERTY(QObject* rtlDevice READ rtlDevice CONSTANT)
    Q_PROPERTY(bool simulatorEnabled READ simulatorEnabled WRITE setSimulatorEnabled NOTIFY simulatorEnabledChanged)

public:
    explicit Receiver(QObject *parent = nullptr);
    ~Receiver() override;

    QString status() const;
    void setStatus(const QString &status);
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
    void updateSpectrum();

private:
    QString m_status = "Idle";
    QTimer m_spectrumTimer;
    QVariantList m_spectrumBins;
    HFSDR::IQBuffer m_iqBuffer;
    HFSDR::DigitalDownConverter m_displayDdc;
    HFSDR::DigitalDownConverter m_receiverDdc;
    HFSDR::IQBuffer m_displayBuffer;
    HFSDR::IQBuffer m_receiverBuffer;
    HFSDR::IQBuffer m_audioIqBuffer;
    std::vector<float> m_audioBuffer;
    HFSDR::SignalGenerator m_signalGenerator;
    HFSDR::DSPProcessor m_dspProcessor;
    //float m_sampleRate = 48000.0f;
    float m_sampleRate = 2048000.0f;
    float m_toneFrequency = 100000.0f;
    FFTProcessor m_fftProcessor;
    std::vector<float> m_fftBins;    
    RTLDevice m_rtlDevice;
    bool m_simulatorEnabled = false;
    HFSDR::IQSource* m_activeSource = nullptr;
    HFSDR::IQBlockRingBuffer m_iqRingBuffer;
    QThread m_sourceThread;
    IQSourceWorker* m_sourceWorker = nullptr;


};

#endif