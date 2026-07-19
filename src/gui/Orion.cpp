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
    setSpectrumSpanHz(1800000);

    // Change these two lines to test RTL tuner gain.
    setAutomaticRfGain(true);
    setRfGainDb(28.0); //0.0 - 49.6 dB for RTL // -3 - +71 dB for PlutoSDR (AD9363)

    // Frequency-domain smoothing removes isolated downward FFT spikes
    // without adding the sluggish response of heavier time averaging.
    setSpectrumSmoothingEnabled(true);
    setSpectrumSmoothingWindowSize(3); //5
    setSpectrumSmoothingDownwardThresholdDb(2.0); //1.5
    setSpectrumSmoothingBlend(0.5); //0.75

    // Symmetric frequency-domain convolution smoothing reduces general
    // FFT bin-to-bin grass without increasing time averaging or latency.
    setSpectrumFrequencySmoothingEnabled(true);
    setSpectrumFrequencySmoothingRadius(2);
    setSpectrumFrequencySmoothingStrength(0.65);

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


void Orion::setSpectrumSmoothingEnabled(bool enabled)
{
    m_receiver.setSpectrumSmoothingEnabled(enabled);
    Logger::info(
        QString("Spectrum downward-spike smoothing %1.")
            .arg(enabled ? "enabled" : "disabled")
        );
}

void Orion::setSpectrumSmoothingWindowSize(int windowSize)
{
    m_receiver.setSpectrumSmoothingWindowSize(windowSize);
    Logger::info(
        QString("Requested spectrum smoothing window set to %1 bins.")
            .arg(windowSize)
        );
}

void Orion::setSpectrumSmoothingDownwardThresholdDb(double thresholdDb)
{
    m_receiver.setSpectrumSmoothingDownwardThresholdDb(thresholdDb);
    Logger::info(
        QString("Requested spectrum smoothing threshold set to %1 dB.")
            .arg(thresholdDb, 0, 'f', 2)
        );
}

void Orion::setSpectrumSmoothingBlend(double blend)
{
    m_receiver.setSpectrumSmoothingBlend(blend);
    Logger::info(
        QString("Requested spectrum smoothing blend set to %1.")
            .arg(blend, 0, 'f', 2)
        );
}


void Orion::setSpectrumFrequencySmoothingEnabled(bool enabled)
{
    m_receiver.setSpectrumFrequencySmoothingEnabled(enabled);
    Logger::info(
        QString("Spectrum frequency smoothing %1.")
            .arg(enabled ? "enabled" : "disabled")
        );
}

void Orion::setSpectrumFrequencySmoothingRadius(int radius)
{
    m_receiver.setSpectrumFrequencySmoothingRadius(radius);
    Logger::info(
        QString("Requested spectrum frequency smoothing radius set to %1 bins.")
            .arg(radius)
        );
}

void Orion::setSpectrumFrequencySmoothingStrength(double strength)
{
    m_receiver.setSpectrumFrequencySmoothingStrength(strength);
    Logger::info(
        QString("Requested spectrum frequency smoothing strength set to %1.")
            .arg(strength, 0, 'f', 2)
        );
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
