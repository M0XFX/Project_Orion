#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <QObject>
#include <QtGlobal>

class DisplaySettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool peakHold
            READ peakHold
                WRITE setPeakHold
                    NOTIFY peakHoldChanged
        )

    Q_PROPERTY(
        bool gridEnabled
            READ gridEnabled
                WRITE setGridEnabled
                    NOTIFY gridEnabledChanged
        )

    Q_PROPERTY(
        bool centerLineEnabled
            READ centerLineEnabled
                WRITE setCenterLineEnabled
                    NOTIFY centerLineEnabledChanged
        )

    Q_PROPERTY(
        bool waterfallEnabled
            READ waterfallEnabled
                WRITE setWaterfallEnabled
                    NOTIFY waterfallEnabledChanged
        )

    Q_PROPERTY(
        bool spectrumFillEnabled
            READ spectrumFillEnabled
                WRITE setSpectrumFillEnabled
                    NOTIFY spectrumFillEnabledChanged
        )

    Q_PROPERTY(
        bool averageEnabled
            READ averageEnabled
                WRITE setAverageEnabled
                    NOTIFY averageEnabledChanged
        )

    Q_PROPERTY(
        bool autoScale
            READ autoScale
                WRITE setAutoScale
                    NOTIFY autoScaleChanged
        )

    Q_PROPERTY(
        bool showDbScale
            READ showDbScale
                WRITE setShowDbScale
                    NOTIFY showDbScaleChanged
        )

    Q_PROPERTY(
        bool showFrequencyScale
            READ showFrequencyScale
                WRITE setShowFrequencyScale
                    NOTIFY showFrequencyScaleChanged
        )

    Q_PROPERTY(
        bool showMarkers
            READ showMarkers
                WRITE setShowMarkers
                    NOTIFY showMarkersChanged
        )

    Q_PROPERTY(
        quint32 spectrumSpanHz
            READ spectrumSpanHz
                WRITE setSpectrumSpanHz
                    NOTIFY spectrumSpanHzChanged
        )

public:
    explicit DisplaySettings(
        QObject* parent = nullptr
        );

    bool peakHold() const;
    bool gridEnabled() const;
    bool centerLineEnabled() const;
    bool waterfallEnabled() const;
    bool spectrumFillEnabled() const;
    bool averageEnabled() const;
    bool autoScale() const;
    bool showDbScale() const;
    bool showFrequencyScale() const;
    bool showMarkers() const;

    quint32 spectrumSpanHz() const;

    void setPeakHold(bool enabled);
    void setGridEnabled(bool enabled);
    void setCenterLineEnabled(bool enabled);
    void setWaterfallEnabled(bool enabled);
    void setSpectrumFillEnabled(bool enabled);
    void setAverageEnabled(bool enabled);
    void setAutoScale(bool enabled);
    void setShowDbScale(bool enabled);
    void setShowFrequencyScale(bool enabled);
    void setShowMarkers(bool enabled);

    void setSpectrumSpanHz(
        quint32 spanHz
        );

signals:
    void peakHoldChanged();
    void gridEnabledChanged();
    void centerLineEnabledChanged();
    void waterfallEnabledChanged();
    void spectrumFillEnabledChanged();
    void averageEnabledChanged();
    void autoScaleChanged();
    void showDbScaleChanged();
    void showFrequencyScaleChanged();
    void showMarkersChanged();

    void spectrumSpanHzChanged();

private:
    bool m_peakHold = true;
    bool m_gridEnabled = true;
    bool m_centerLineEnabled = true;
    bool m_waterfallEnabled = true;
    bool m_spectrumFillEnabled = false;
    bool m_averageEnabled = false;
    bool m_autoScale = false;
    bool m_showDbScale = true;
    bool m_showFrequencyScale = false;
    bool m_showMarkers = false;

    quint32 m_spectrumSpanHz = 250000;
};

#endif