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

private:
    Radio m_radio;
    Receiver m_receiver;
    DisplaySettings m_displaySettings;
};

} // namespace HFSDR

#endif