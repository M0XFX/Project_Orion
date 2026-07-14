#ifndef RADIO_H
#define RADIO_H

#include <QObject>
#include <QString>
#include <QtGlobal>

#include "ReceiverConfiguration.h"

class Radio : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        quint64 frequencyHz
            READ frequencyHz
                WRITE setFrequencyHz
                    NOTIFY frequencyChanged
        )

    Q_PROPERTY(
        QString frequency
            READ frequency
                WRITE setFrequency
                    NOTIFY frequencyChanged
        )

    Q_PROPERTY(
        QString mode
            READ modeName
                WRITE setModeFromString
                    NOTIFY modeChanged
        )

    Q_PROPERTY(
        QString modeName
            READ modeName
                NOTIFY modeChanged
        )

    Q_PROPERTY(
        int rxBandwidthHz
            READ rxBandwidthHz
                WRITE setRxBandwidthHz
                    NOTIFY rxBandwidthHzChanged
        )

public:
    explicit Radio(QObject* parent = nullptr);

    quint64 frequencyHz() const;
    void setFrequencyHz(quint64 frequencyHz);

    QString frequency() const;
    void setFrequency(const QString& frequency);

    HFSDR::DemodulationMode mode() const;

    void setMode(
        HFSDR::DemodulationMode mode
        );

    QString modeName() const;

    void setModeFromString(
        const QString& modeName
        );

    int rxBandwidthHz() const;

    void setRxBandwidthHz(
        int bandwidthHz
        );

    HFSDR::ReceiverConfiguration
    receiverConfiguration() const;

signals:
    void frequencyChanged();
    void modeChanged();
    void rxBandwidthHzChanged();

    void receiverConfigurationChanged(
        const HFSDR::ReceiverConfiguration&
            configuration
        );

private:
    static QString formatFrequency(
        quint64 frequencyHz
        );

    void publishReceiverConfiguration();

    quint64 m_frequencyHz = 28400000;

    HFSDR::ReceiverConfiguration
        m_receiverConfiguration;
};

#endif