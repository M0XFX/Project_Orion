#ifndef RECEIVERCONFIGURATION_H
#define RECEIVERCONFIGURATION_H

#include <QMetaType>
#include <QString>

namespace HFSDR
{

enum class DemodulationMode
{
    AM,
    DSB,
    NFM,
    WFM,
    USB,
    LSB,
    CW
};

struct ReceiverConfiguration
{
    DemodulationMode mode =
        DemodulationMode::AM;

    int bandwidthHz = 10000;

    // Fine tuning within the fixed 60 MHz IF.
    int frequencyOffsetHz = 0;

    bool agcEnabled = true;
    bool noiseBlankerEnabled = false;
    bool noiseReductionEnabled = false;

    bool operator==(
        const ReceiverConfiguration& other) const
    {
        return mode ==
                   other.mode &&
               bandwidthHz ==
                   other.bandwidthHz &&
               frequencyOffsetHz ==
                   other.frequencyOffsetHz &&
               agcEnabled ==
                   other.agcEnabled &&
               noiseBlankerEnabled ==
                   other.noiseBlankerEnabled &&
               noiseReductionEnabled ==
                   other.noiseReductionEnabled;
    }

    bool operator!=(
        const ReceiverConfiguration& other) const
    {
        return !(*this == other);
    }
};

QString demodulationModeToString(
    DemodulationMode mode
    );

bool stringToDemodulationMode(
    const QString& text,
    DemodulationMode& mode
    );

int defaultBandwidthForMode(
    DemodulationMode mode
    );

} // namespace HFSDR

Q_DECLARE_METATYPE(
    HFSDR::DemodulationMode
    )

Q_DECLARE_METATYPE(
    HFSDR::ReceiverConfiguration
    )

#endif
