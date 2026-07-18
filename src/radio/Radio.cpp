#include "Radio.h"

namespace HFSDR
{

QString demodulationModeToString(
    DemodulationMode mode)
{
    switch (mode) {
    case DemodulationMode::AM:
        return "AM";

    case DemodulationMode::DSB:
        return "DSB";

    case DemodulationMode::NFM:
        return "NFM";

    case DemodulationMode::WFM:
        return "WFM";

    case DemodulationMode::USB:
        return "USB";

    case DemodulationMode::LSB:
        return "LSB";

    case DemodulationMode::CW:
        return "CW";
    }

    return "AM";
}

bool stringToDemodulationMode(
    const QString& text,
    DemodulationMode& mode)
{
    const QString normalised =
        text.trimmed().toUpper();

    if (normalised == "AM") {
        mode = DemodulationMode::AM;
        return true;
    }

    if (normalised == "DSB") {
        mode = DemodulationMode::DSB;
        return true;
    }

    if (normalised == "NFM" ||
        normalised == "FM") {
        mode = DemodulationMode::NFM;
        return true;
    }

    if (normalised == "WFM") {
        mode = DemodulationMode::WFM;
        return true;
    }

    if (normalised == "USB") {
        mode = DemodulationMode::USB;
        return true;
    }

    if (normalised == "LSB") {
        mode = DemodulationMode::LSB;
        return true;
    }

    if (normalised == "CW") {
        mode = DemodulationMode::CW;
        return true;
    }

    return false;
}

int defaultBandwidthForMode(
    DemodulationMode mode)
{
    switch (mode) {
    case DemodulationMode::AM:
        return 10000;

    case DemodulationMode::DSB:
        return 6000;

    case DemodulationMode::USB:
    case DemodulationMode::LSB:
        return 2700;

    case DemodulationMode::CW:
        return 500;

    case DemodulationMode::NFM:
        return 12500;

    case DemodulationMode::WFM:
        return 180000;
    }

    return 10000;
}

} // namespace HFSDR

Radio::Radio(QObject* parent)
    : QObject(parent)
{
}

quint64 Radio::frequencyHz() const
{
    return m_frequencyHz;
}

void Radio::setFrequencyHz(
    quint64 frequencyHz)
{
    if (m_frequencyHz == frequencyHz)
        return;

    m_frequencyHz = frequencyHz;
    emit frequencyChanged();
}

QString Radio::frequency() const
{
    return formatFrequency(
        m_frequencyHz
        );
}

void Radio::setFrequency(
    const QString& frequency)
{
    QString digits;
    digits.reserve(frequency.size());

    for (const QChar character : frequency) {
        if (character.isDigit())
            digits.append(character);
    }

    bool conversionOk = false;

    const quint64 value =
        digits.toULongLong(
            &conversionOk
            );

    if (!conversionOk)
        return;

    setFrequencyHz(value);
}

HFSDR::DemodulationMode
Radio::mode() const
{
    return m_receiverConfiguration.mode;
}

void Radio::setMode(
    HFSDR::DemodulationMode mode)
{
    if (m_receiverConfiguration.mode ==
        mode) {
        return;
    }

    m_receiverConfiguration.mode = mode;

    emit modeChanged();

    setRxBandwidthHz(
        HFSDR::defaultBandwidthForMode(
            mode
            )
        );

    publishReceiverConfiguration();
}

QString Radio::modeName() const
{
    return HFSDR::demodulationModeToString(
        m_receiverConfiguration.mode
        );
}

void Radio::setModeFromString(
    const QString& modeName)
{
    HFSDR::DemodulationMode mode;

    if (!HFSDR::stringToDemodulationMode(
            modeName,
            mode)) {
        return;
    }

    setMode(mode);
}

int Radio::rxBandwidthHz() const
{
    return
        m_receiverConfiguration.bandwidthHz;
}

void Radio::setRxBandwidthHz(
    int bandwidthHz)
{
    if (bandwidthHz < 50)
        bandwidthHz = 50;

    if (bandwidthHz > 250000)
        bandwidthHz = 250000;

    if (m_receiverConfiguration.bandwidthHz ==
        bandwidthHz) {
        return;
    }

    m_receiverConfiguration.bandwidthHz =
        bandwidthHz;

    emit rxBandwidthHzChanged();

    publishReceiverConfiguration();
}

quint32 Radio::spectrumSpanHz() const
{
    return m_spectrumSpanHz;
}

void Radio::setSpectrumSpanHz(quint32 spanHz)
{
    if (spanHz < 10000)
        spanHz = 10000;
    if (m_spectrumSpanHz == spanHz)
        return;
    m_spectrumSpanHz = spanHz;
    emit spectrumSpanHzChanged();
}

bool Radio::automaticRfGain() const
{
    return m_automaticRfGain;
}

void Radio::setAutomaticRfGain(bool enabled)
{
    if (m_automaticRfGain == enabled)
        return;
    m_automaticRfGain = enabled;
    emit automaticRfGainChanged();
}

double Radio::rfGainDb() const
{
    return m_rfGainDb;
}

void Radio::setRfGainDb(double gainDb)
{
    if (qFuzzyCompare(m_rfGainDb + 1.0, gainDb + 1.0))
        return;
    m_rfGainDb = gainDb;
    emit rfGainDbChanged();
}

HFSDR::SDRType Radio::sdrType() const
{
    return m_sdrType;
}

void Radio::setSdrType(HFSDR::SDRType type)
{
    if (m_sdrType == type)
        return;
    m_sdrType = type;
    emit sdrTypeChanged();
}

HFSDR::ReceiverConfiguration
Radio::receiverConfiguration() const
{
    return m_receiverConfiguration;
}

QString Radio::formatFrequency(
    quint64 frequencyHz)
{
    const quint64 megahertz =
        frequencyHz / 1000000ULL;

    const quint64 kilohertz =
        (frequencyHz / 1000ULL) %
        1000ULL;

    const quint64 hertz =
        frequencyHz % 1000ULL;

    return QString("%1.%2.%3 MHz")
        .arg(megahertz)
        .arg(
            kilohertz,
            3,
            10,
            QChar('0')
            )
        .arg(
            hertz,
            3,
            10,
            QChar('0')
            );
}

void Radio::publishReceiverConfiguration()
{
    emit receiverConfigurationChanged(
        m_receiverConfiguration
        );
}