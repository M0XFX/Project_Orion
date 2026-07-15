#include "DisplaySettings.h"

#include <algorithm>

DisplaySettings::DisplaySettings(
    QObject* parent)
    : QObject(parent)
{
}

bool DisplaySettings::peakHold() const
{
    return m_peakHold;
}

bool DisplaySettings::gridEnabled() const
{
    return m_gridEnabled;
}

bool DisplaySettings::centerLineEnabled() const
{
    return m_centerLineEnabled;
}

bool DisplaySettings::waterfallEnabled() const
{
    return m_waterfallEnabled;
}

bool DisplaySettings::spectrumFillEnabled() const
{
    return m_spectrumFillEnabled;
}

bool DisplaySettings::averageEnabled() const
{
    return m_averageEnabled;
}

bool DisplaySettings::autoScale() const
{
    return m_autoScale;
}

bool DisplaySettings::showDbScale() const
{
    return m_showDbScale;
}

bool DisplaySettings::showFrequencyScale() const
{
    return m_showFrequencyScale;
}

bool DisplaySettings::showMarkers() const
{
    return m_showMarkers;
}

quint32 DisplaySettings::spectrumSpanHz() const
{
    return m_spectrumSpanHz;
}

void DisplaySettings::setPeakHold(
    bool enabled)
{
    if (m_peakHold == enabled)
        return;

    m_peakHold = enabled;
    emit peakHoldChanged();
}

void DisplaySettings::setGridEnabled(
    bool enabled)
{
    if (m_gridEnabled == enabled)
        return;

    m_gridEnabled = enabled;
    emit gridEnabledChanged();
}

void DisplaySettings::setCenterLineEnabled(
    bool enabled)
{
    if (m_centerLineEnabled == enabled)
        return;

    m_centerLineEnabled = enabled;
    emit centerLineEnabledChanged();
}

void DisplaySettings::setWaterfallEnabled(
    bool enabled)
{
    if (m_waterfallEnabled == enabled)
        return;

    m_waterfallEnabled = enabled;
    emit waterfallEnabledChanged();
}

void DisplaySettings::setSpectrumFillEnabled(
    bool enabled)
{
    if (m_spectrumFillEnabled == enabled)
        return;

    m_spectrumFillEnabled = enabled;
    emit spectrumFillEnabledChanged();
}

void DisplaySettings::setAverageEnabled(
    bool enabled)
{
    if (m_averageEnabled == enabled)
        return;

    m_averageEnabled = enabled;
    emit averageEnabledChanged();
}

void DisplaySettings::setAutoScale(
    bool enabled)
{
    if (m_autoScale == enabled)
        return;

    m_autoScale = enabled;
    emit autoScaleChanged();
}

void DisplaySettings::setShowDbScale(
    bool enabled)
{
    if (m_showDbScale == enabled)
        return;

    m_showDbScale = enabled;
    emit showDbScaleChanged();
}

void DisplaySettings::setShowFrequencyScale(
    bool enabled)
{
    if (m_showFrequencyScale == enabled)
        return;

    m_showFrequencyScale = enabled;
    emit showFrequencyScaleChanged();
}

void DisplaySettings::setShowMarkers(
    bool enabled)
{
    if (m_showMarkers == enabled)
        return;

    m_showMarkers = enabled;
    emit showMarkersChanged();
}

void DisplaySettings::setSpectrumSpanHz(
    quint32 spanHz)
{
    constexpr quint32 minimumSpanHz =
        10000;

    constexpr quint32 temporaryMaximumSpanHz =
        250000;

    spanHz = std::clamp(
        spanHz,
        minimumSpanHz,
        temporaryMaximumSpanHz
        );

    if (m_spectrumSpanHz == spanHz)
        return;

    m_spectrumSpanHz = spanHz;
    emit spectrumSpanHzChanged();
}