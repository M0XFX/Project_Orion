#include "Logger.h"

#include <QDateTime>
#include <QDebug>

namespace HFSDR
{

static QString timeStamp()
{
    return QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

void Logger::info(const QString& message)
{
    qInfo().noquote()
    << QString("[%1] [INFO ] %2")
            .arg(timeStamp(), message);
}

void Logger::warning(const QString& message)
{
    qWarning().noquote()
    << QString("[%1] [WARN ] %2")
            .arg(timeStamp(), message);
}

void Logger::error(const QString& message)
{
    qCritical().noquote()
    << QString("[%1] [ERROR] %2")
            .arg(timeStamp(), message);
}

} // namespace HFSDR
