#ifndef RADIOTYPES_H
#define RADIOTYPES_H

#include <QMetaType>
#include <QString>

namespace HFSDR
{
enum class SDRType { RTLSDR };

inline QString sdrTypeToString(SDRType type)
{
    switch (type) {
    case SDRType::RTLSDR: return "RTL-SDR";
    }
    return "Unknown";
}
}

Q_DECLARE_METATYPE(HFSDR::SDRType)

#endif
