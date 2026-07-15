#ifndef IQSOURCE_H
#define IQSOURCE_H

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

    virtual bool readSamples(
        IQBuffer& buffer
        ) = 0;

    virtual unsigned int sampleRate() const = 0;

    virtual unsigned long long
    centerFrequencyHz() const = 0;

    virtual IQSourceCapabilities
    capabilities() const = 0;
};

} // namespace HFSDR

#endif