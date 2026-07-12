#include "VFO.h"

VFO::VFO(QObject *parent)
    : QObject{parent}
{
}

quint64 VFO::frequencyHz() const
{
    return m_frequencyHz;
}

void VFO::setFrequencyHz(quint64 frequencyHz)
{
    if (m_frequencyHz == frequencyHz)
        return;

    m_frequencyHz = frequencyHz;
    emit frequencyChanged();
}

QString VFO::frequencyText() const
{
    return formatFrequency(m_frequencyHz);
}

quint64 VFO::ifFrequencyHz() const
{
    return m_ifFrequencyHz;
}

void VFO::setIfFrequencyHz(quint64 ifFrequencyHz)
{
    if (m_ifFrequencyHz == ifFrequencyHz)
        return;

    m_ifFrequencyHz = ifFrequencyHz;
    emit ifFrequencyChanged();
}

QString VFO::ifFrequencyText() const
{
    return formatFrequency(m_ifFrequencyHz);
}

QString VFO::mode() const
{
    return m_mode;
}

void VFO::setMode(const QString &mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged();
}

QString VFO::formatFrequency(quint64 frequencyHz) const
{
    const quint64 mhz = frequencyHz / 1000000;
    const quint64 khz = (frequencyHz / 1000) % 1000;
    const quint64 hz  = frequencyHz % 1000;

    return QString("%1.%2.%3 MHz")
        .arg(mhz)
        .arg(khz, 3, 10, QChar('0'))
        .arg(hz, 3, 10, QChar('0'));
}