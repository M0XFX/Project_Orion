#include "Orion.h"
#include "Logger.h"
#include "Version.h"
#include <QString>

namespace HFSDR
{

Orion::Orion(QObject *parent)
    : QObject{parent}
{
    HFSDR::Logger::info(
        QString("Project Orion v%1 starting...")
            .arg(ORION_VERSION_STRING)
        );

    m_radio.setFrequency("28.400.000 MHz");
    m_radio.setMode("FM");

    Logger::info("Radio initialised.");
    Logger::info("Receiver initialised.");
    m_receiver.startSpectrum();

    m_receiver.openRtlDevice();

    //m_receiver.setStatus("RTL-SDR Not Connected");

    HFSDR::Logger::info("Peak Hold enabled.");
    m_displaySettings.setPeakHold(false);
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


} // namespace HFSDR
