#include "SDRManager.h"
#include "Logger.h"

namespace HFSDR
{
SDRManager::SDRManager(QObject* parent)
    : QObject(parent)
{
    m_activeSource = &m_rtlDevice;
    Logger::info("SDRManager initialised with RTL-SDR.");
}

SDRType SDRManager::selectedType() const { return m_selectedType; }
IQSource* SDRManager::activeSource() { return m_activeSource; }
const IQSource* SDRManager::activeSource() const { return m_activeSource; }

bool SDRManager::selectSdrType(SDRType type)
{
    switch (type) {
    case SDRType::RTLSDR:
        m_selectedType = type;
        m_activeSource = &m_rtlDevice;
        Logger::info("SDRManager selected RTL-SDR.");
        return true;
    }
    Logger::error("SDRManager received an unsupported SDR type.");
    return false;
}
}
