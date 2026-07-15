#ifndef IQSOURCECAPABILITIES_H
#define IQSOURCECAPABILITIES_H

#include <QMetaType>
#include <QString>
#include <QVector>
#include <QtGlobal>


namespace HFSDR
{

struct IQSourceCapabilities
{
    QString deviceName;
    QString manufacturer;

    quint32 sampleRateHz = 0;

    // Recommended spectrum bandwidth with reliable
    // amplitude response near the displayed edges.
    quint32 usableBandwidthHz = 0;

    // Absolute bandwidth represented by the current
    // complex sample rate.
    quint32 maximumBandwidthHz = 0;

    quint32 defaultSpectrumSpanHz = 0;

    QVector<quint32> supportedSpectrumSpansHz;

    bool supportsAutomaticGain = false;
    bool supportsManualGain = false;
    bool supportsDirectSampling = false;
    bool supportsBiasTee = false;
    bool supportsTransmit = false;
    bool supportsFullDuplex = false;

    int receiveChannelCount = 1;
    int transmitChannelCount = 0;
};

} // namespace HFSDR

Q_DECLARE_METATYPE(HFSDR::IQSourceCapabilities)

#endif