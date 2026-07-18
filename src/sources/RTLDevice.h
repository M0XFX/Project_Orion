#ifndef RTLDEVICE_H
#define RTLDEVICE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QtGlobal>
#include <rtl-sdr.h>

#include "IQBuffer.h"
#include "IQSource.h"
#include "IQSourceCapabilities.h"

class RTLDevice : public QObject, public HFSDR::IQSource
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(quint64 centerFrequencyHz READ centerFrequencyHz NOTIFY centerFrequencyChanged)
    Q_PROPERTY(quint32 sampleRate READ sampleRate NOTIFY sampleRateChanged)

public:
    explicit RTLDevice(QObject* parent = nullptr);
    ~RTLDevice() override;

    QString status() const;
    bool connected() const override;
    quint64 centerFrequencyHz() const override;
    quint32 sampleRate() const override;
    bool readSamples(HFSDR::IQBuffer& buffer) override;
    HFSDR::IQSourceCapabilities capabilities() const override;

    bool setCenterFrequencyHz(quint64 frequencyHz) override;
    bool setAutomaticGain(bool enabled) override;
    bool setGainDb(double gainDb) override;
    bool automaticGain() const override;
    double gainDb() const override;

public slots:
    void open() override;
    void close() override;

signals:
    void statusChanged();
    void connectedChanged();
    void centerFrequencyChanged();
    void sampleRateChanged();
    void gainModeChanged();
    void gainChanged();

private:
    void setStatus(const QString& status);
    void setConnected(bool connected);
    void querySupportedGains();
    int nearestSupportedGainTenthsDb(int requestedGainTenthsDb) const;

    QString m_status = "Not Connected";
    bool m_connected = false;

    // Applied hardware state only; Radio owns requested state.
    quint64 m_appliedCenterFrequencyHz = 0;
    quint32 m_sampleRate = 2048000;
    bool m_automaticGain = true;
    int m_appliedGainTenthsDb = 0;
    QVector<int> m_supportedGainsTenthsDb;

    rtlsdr_dev_t* m_device = nullptr;
};

#endif
