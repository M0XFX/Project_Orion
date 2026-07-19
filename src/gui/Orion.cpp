#include "Orion.h"
#include "Logger.h"
#include "Version.h"

namespace HFSDR
{
Orion::Orion(QObject* parent)
    : QObject(parent),
      m_radio(),
      m_sdrManager(),
      m_receiver(m_sdrManager.activeSource()),
      m_radioController(&m_radio, &m_receiver, &m_sdrManager),
      m_displaySettings()
{
    Logger::info(QString("Project Orion v%1 starting...").arg(ORION_VERSION_STRING));

    // Central test configuration. No GUI controls are required yet.
    setSdrType(SDRType::RTLSDR);
    setFrequencyHz(103600000); // IF Frequency
    setMode(DemodulationMode::WFM);
    setRxBandwidthHz(180000);
    setSpectrumSpanHz(2000000);

    // Change these two lines to test RTL tuner gain.
    setAutomaticRfGain(true);
    setRfGainDb(28.0); //0.0 - 49.6 dB for RTL // -3 - +71 dB for PlutoSDR (AD9363)

    m_displaySettings.setPeakHold(false);
    m_receiver.startSpectrum();


    ///// FFT

    m_displaySettings.setAutoScale(false);
    m_displaySettings.setSpectrumOffsetDb(10.0);
    m_displaySettings.setSpectrumRangeDb(60.0);

    ///// FFT





    if (!m_radioController.start())
        Logger::error("Project Orion receiver did not start.");

    Logger::info("Radio and display settings initialised.");
}

QObject* Orion::radio() { return &m_radio; }
QObject* Orion::receiver() { return &m_receiver; }
DisplaySettings* Orion::displaySettings() { return &m_displaySettings; }

void Orion::setMode(const QString& modeName)
{
    DemodulationMode mode;
    if (!stringToDemodulationMode(modeName, mode)) {
        Logger::warning(QString("Unsupported radio mode requested: %1").arg(modeName));
        return;
    }
    setMode(mode);
}

void Orion::setMode(DemodulationMode mode)
{
    m_radio.setMode(mode);
    Logger::info(QString("Radio mode set to %1.").arg(m_radio.modeName()));
}

void Orion::setFrequencyHz(quint64 frequencyHz)
{
    m_radio.setFrequencyHz(frequencyHz);
    Logger::info(QString("Requested radio frequency set to %1 Hz.").arg(frequencyHz));
}

void Orion::setRxBandwidthHz(int bandwidthHz)
{
    m_radio.setRxBandwidthHz(bandwidthHz);
    Logger::info(QString("Requested RX bandwidth set to %1 Hz.").arg(m_radio.rxBandwidthHz()));
}

void Orion::setSpectrumSpanHz(quint32 spanHz)
{
    m_radio.setSpectrumSpanHz(spanHz);
    Logger::info(QString("Requested spectrum span set to %1 Hz.").arg(m_radio.spectrumSpanHz()));
}

void Orion::setAutomaticRfGain(bool enabled)
{
    m_radio.setAutomaticRfGain(enabled);
    Logger::info(QString("Requested RF gain mode set to %1.").arg(enabled ? "automatic" : "manual"));
}

void Orion::setRfGainDb(double gainDb)
{
    m_radio.setRfGainDb(gainDb);
    Logger::info(QString("Requested RF gain set to %1 dB.").arg(gainDb, 0, 'f', 1));
}

void Orion::setSdrType(HFSDR::SDRType type)
{
    m_radio.setSdrType(type);
    Logger::info(QString("Requested SDR type set to %1.").arg(sdrTypeToString(type)));
}
}
