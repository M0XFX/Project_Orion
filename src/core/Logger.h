#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

namespace HFSDR
{

class Logger
{
public:
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);
};

} // namespace HFSDR

#endif
