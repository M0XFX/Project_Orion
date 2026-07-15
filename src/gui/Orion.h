#ifndef ORION_H
#define ORION_H

#include <QObject>
#include <QString>
#include <QtGlobal>

#include "DisplaySettings.h"
#include "Radio.h"
#include "Receiver.h"
#include "ReceiverConfiguration.h"

namespace HFSDR
{

class Orion : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QObject* radio
            READ radio
                CONSTANT
        )

    Q_PROPERTY(
        QObject* receiver
            READ receiver
                CONSTANT
        )

    Q_PROPERTY(
        DisplaySettings* displaySettings
            READ displaySettings
                CONSTANT
        )

public:
    explicit Orion(QObject* parent = nullptr);

    QObject* radio();
    QObject* receiver();
    DisplaySettings* displaySettings();

    // Central Orion command API.
    Q_INVOKABLE void setMode(
        const QString& modeName
        );

    void setMode(
        DemodulationMode mode
        );

    Q_INVOKABLE void setFrequencyHz(
        quint64 frequencyHz
        );

    Q_INVOKABLE void setRxBandwidthHz(
        int bandwidthHz
        );

    Q_PROPERTY(
        quint32 spectrumSpanHz
            READ spectrumSpanHz
                WRITE setSpectrumSpanHz
                    NOTIFY spectrumSpanHzChanged
        )

    quint32 spectrumSpanHz() const;

    Q_INVOKABLE void setSpectrumSpanHz(
        quint32 spanHz
        );


signals:
    void modeCommanded(
        HFSDR::DemodulationMode mode
        );

    void frequencyCommanded(
        quint64 frequencyHz
        );

    void rxBandwidthCommanded(
        int bandwidthHz
        );
    void spectrumSpanHzChanged(
        quint32 spanHz
        );

private:
    Radio m_radio;
    Receiver m_receiver;
    DisplaySettings m_displaySettings;
    quint32 m_spectrumSpanHz = 250000;
};

} // namespace HFSDR

#endif