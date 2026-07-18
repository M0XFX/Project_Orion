#ifndef RTLDEVICE_H
#define RTLDEVICE_H

#include <QObject>
#include <QString>
#include <QtGlobal>

#include <rtl-sdr.h>

#include "IQBuffer.h"
#include "IQSource.h"
#include "IQSourceCapabilities.h"

class RTLDevice : public QObject,
                  public HFSDR::IQSource
{
    Q_OBJECT

    Q_PROPERTY(
        QString status
            READ status
                NOTIFY statusChanged
        )

    Q_PROPERTY(
        bool connected
            READ connected
                NOTIFY connectedChanged
        )

    Q_PROPERTY(
        quint64 centerFrequencyHz
            READ centerFrequencyHz
                NOTIFY centerFrequencyChanged
        )

    Q_PROPERTY(
        quint32 sampleRate
            READ sampleRate
                NOTIFY sampleRateChanged
        )

public:
    explicit RTLDevice(
        QObject* parent = nullptr
        );

    ~RTLDevice() override;

    QString status() const;

    bool connected() const override;

    quint64 centerFrequencyHz() const override;
    quint32 sampleRate() const override;

    bool readSamples(
        HFSDR::IQBuffer& buffer
        ) override;

    HFSDR::IQSourceCapabilities
    capabilities() const override;

public slots:
    void open() override;
    void close() override;

signals:
    void statusChanged();
    void connectedChanged();
    void centerFrequencyChanged();
    void sampleRateChanged();

private:
    void setStatus(
        const QString& status
        );

    void setConnected(
        bool connected
        );

    QString m_status = "Not Connected";
    bool m_connected = false;

    // Normal Project Orion fixed-IF setting.
    quint64 m_centerFrequencyHz = 103600000;

    quint32 m_sampleRate = 2048000;

    rtlsdr_dev_t* m_device = nullptr;
};

#endif