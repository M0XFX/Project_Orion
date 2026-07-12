#include "Radio.h"

Radio::Radio(QObject *parent)
    : QObject{parent}
{
}

QString Radio::frequency() const
{
    return m_frequency;
}

void Radio::setFrequency(const QString &frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged();
}

QString Radio::mode() const
{
    return m_mode;
}

void Radio::setMode(const QString &mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    emit modeChanged();
}
