#ifndef IQSOURCE_H
#define IQSOURCE_H

#include <QtGlobal>
#include "IQBuffer.h"
#include "IQSourceCapabilities.h"

namespace HFSDR
{
class IQSource
{
public:
    virtual ~IQSource() = default;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual bool connected() const = 0;
    virtual bool readSamples(IQBuffer& buffer) = 0;
    virtual quint32 sampleRate() const = 0;
    virtual quint64 centerFrequencyHz() const = 0;
    virtual IQSourceCapabilities capabilities() const = 0;

    virtual bool setCenterFrequencyHz(quint64 frequencyHz)
    { Q_UNUSED(frequencyHz); return false; }
    virtual bool setAutomaticGain(bool enabled)
    { Q_UNUSED(enabled); return false; }
    virtual bool setGainDb(double gainDb)
    { Q_UNUSED(gainDb); return false; }
    virtual bool automaticGain() const { return true; }
    virtual double gainDb() const { return 0.0; }
};
}

#endif
