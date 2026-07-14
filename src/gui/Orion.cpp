#include "Orion.h"

#include "Logger.h"
#include "Version.h"

#include <QString>

namespace HFSDR
{

Orion::Orion(QObject* parent)
    : QObject(parent)
{
    Logger::info(
        QString("Project Orion v%1 starting...")
            .arg(ORION_VERSION_STRING)
    );

    // All control commands pass through Orion.
    connect(
        this,
        &Orion::modeCommanded,
        &m_receiver,
        &Receiver::setMode
    );

    connect(
        this,
        &Orion::rxBandwidthCommanded,
        &m_receiver,
        &Receiver::setRxBandwidthHz
    );

    setFrequencyHz(28400000);

    /*
    setMode(DemodulationMode::NFM);
    setRxBandwidthHz(1250);
    */

/*
    setMode(DemodulationMode::WFM);
    setRxBandwidthHz(180000);

*/

    setMode(DemodulationMode::USB);
    setRxBandwidthHz(2700);



    Logger::info("Radio initialised.");

    m_receiver.startSpectrum();
    m_receiver.openRtlDevice();

    m_displaySettings.setPeakHold(false);

    Logger::info(
        "Display settings initialised."
    );
}

QObject* Orion::radio()
{
    return &m_radio;
}

QObject* Orion::receiver()
{
    return &m_receiver;
}

DisplaySettings* Orion::displaySettings()
{
    return &m_displaySettings;
}

void Orion::setMode(
    const QString& modeName)
{
    DemodulationMode mode;

    if (!stringToDemodulationMode(
            modeName,
            mode)) {

        Logger::warning(
            QString(
                "Unsupported radio mode requested: %1"
            ).arg(modeName)
        );

        return;
    }

    setMode(mode);
}

void Orion::setMode(
    DemodulationMode mode)
{
    m_radio.setMode(mode);

    const int defaultBandwidth =
        m_radio.rxBandwidthHz();

    Logger::info(
        QString("Radio mode set to %1.")
            .arg(m_radio.modeName())
    );

    emit modeCommanded(mode);

    // Radio::setMode() may have applied a new
    // mode-dependent default bandwidth.
    emit rxBandwidthCommanded(
        defaultBandwidth
    );
}

void Orion::setFrequencyHz(
    quint64 frequencyHz)
{
    m_radio.setFrequencyHz(
        frequencyHz
    );

    Logger::info(
        QString(
            "Radio frequency set to %1 Hz."
        ).arg(frequencyHz)
    );

    emit frequencyCommanded(
        frequencyHz
    );
}

void Orion::setRxBandwidthHz(
    int bandwidthHz)
{
    m_radio.setRxBandwidthHz(
        bandwidthHz
    );

    Logger::info(
        QString(
            "RX bandwidth set to %1 Hz."
        ).arg(
            m_radio.rxBandwidthHz()
        )
    );

    emit rxBandwidthCommanded(
        m_radio.rxBandwidthHz()
    );
}

} // namespace HFSDR