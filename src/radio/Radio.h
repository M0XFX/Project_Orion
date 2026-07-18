#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QString>
#include <QtGlobal>
#include "RadioTypes.h"
#include "ReceiverConfiguration.h"

class Radio : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint64 frequencyHz READ frequencyHz WRITE setFrequencyHz NOTIFY frequencyChanged)
    Q_PROPERTY(QString frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(QString mode READ modeName WRITE setModeFromString NOTIFY modeChanged)
    Q_PROPERTY(QString modeName READ modeName NOTIFY modeChanged)
    Q_PROPERTY(int rxBandwidthHz READ rxBandwidthHz WRITE setRxBandwidthHz NOTIFY rxBandwidthHzChanged)
    Q_PROPERTY(quint32 spectrumSpanHz READ spectrumSpanHz WRITE setSpectrumSpanHz NOTIFY spectrumSpanHzChanged)
    Q_PROPERTY(bool automaticRfGain READ automaticRfGain WRITE setAutomaticRfGain NOTIFY automaticRfGainChanged)
    Q_PROPERTY(double rfGainDb READ rfGainDb WRITE setRfGainDb NOTIFY rfGainDbChanged)

public:
    explicit Radio(QObject* parent = nullptr);
    quint64 frequencyHz() const;
    void setFrequencyHz(quint64 frequencyHz);
    QString frequency() const;
    void setFrequency(const QString& frequency);
    HFSDR::DemodulationMode mode() const;
    void setMode(HFSDR::DemodulationMode mode);
    QString modeName() const;
    void setModeFromString(const QString& modeName);
    int rxBandwidthHz() const;
    void setRxBandwidthHz(int bandwidthHz);
    quint32 spectrumSpanHz() const;
    void setSpectrumSpanHz(quint32 spanHz);
    bool automaticRfGain() const;
    void setAutomaticRfGain(bool enabled);
    double rfGainDb() const;
    void setRfGainDb(double gainDb);
    HFSDR::SDRType sdrType() const;
    void setSdrType(HFSDR::SDRType type);
    HFSDR::ReceiverConfiguration receiverConfiguration() const;

signals:
    void frequencyChanged();
    void modeChanged();
    void rxBandwidthHzChanged();
    void spectrumSpanHzChanged();
    void automaticRfGainChanged();
    void rfGainDbChanged();
    void sdrTypeChanged();
    void receiverConfigurationChanged(const HFSDR::ReceiverConfiguration& configuration);

private:
    static QString formatFrequency(quint64 frequencyHz);
    void publishReceiverConfiguration();
    quint64 m_frequencyHz = 28400000;
    quint32 m_spectrumSpanHz = 250000;
    bool m_automaticRfGain = true;
    double m_rfGainDb = 28.0;
    HFSDR::SDRType m_sdrType = HFSDR::SDRType::RTLSDR;
    HFSDR::ReceiverConfiguration m_receiverConfiguration;
};

#endif
