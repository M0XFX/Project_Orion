#ifndef ORION_H
#define ORION_H

#include <QObject>
#include <QString>
#include <QtGlobal>
#include "DisplaySettings.h"
#include "Radio.h"
#include "RadioController.h"
#include "Receiver.h"
#include "SDRManager.h"

namespace HFSDR
{
class Orion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* radio READ radio CONSTANT)
    Q_PROPERTY(QObject* receiver READ receiver CONSTANT)
    Q_PROPERTY(DisplaySettings* displaySettings READ displaySettings CONSTANT)

public:
    explicit Orion(QObject* parent = nullptr);
    QObject* radio();
    QObject* receiver();
    DisplaySettings* displaySettings();
    Q_INVOKABLE void setMode(const QString& modeName);
    void setMode(DemodulationMode mode);
    Q_INVOKABLE void setFrequencyHz(quint64 frequencyHz);
    Q_INVOKABLE void setRxBandwidthHz(int bandwidthHz);
    Q_INVOKABLE void setSpectrumSpanHz(quint32 spanHz);
    Q_INVOKABLE void setSpectrumSmoothingEnabled(bool enabled);
    Q_INVOKABLE void setSpectrumSmoothingWindowSize(int windowSize);
    Q_INVOKABLE void setSpectrumSmoothingDownwardThresholdDb(double thresholdDb);
    Q_INVOKABLE void setSpectrumSmoothingBlend(double blend);

    Q_INVOKABLE void setSpectrumFrequencySmoothingEnabled(bool enabled);
    Q_INVOKABLE void setSpectrumFrequencySmoothingRadius(int radius);
    Q_INVOKABLE void setSpectrumFrequencySmoothingStrength(double strength);

    Q_INVOKABLE void setSpectrumTemporalAveragingEnabled(bool enabled);
    Q_INVOKABLE void setSpectrumTemporalAveragingAlpha(double alpha);
    Q_INVOKABLE void setSpectrumFramesPerSecond(int framesPerSecond);
    void setAutomaticRfGain(bool enabled);
    void setRfGainDb(double gainDb);
    void setSdrType(HFSDR::SDRType type);

private:
    Radio m_radio;
    SDRManager m_sdrManager;
    Receiver m_receiver;
    RadioController m_radioController;
    DisplaySettings m_displaySettings;
};
}

#endif
