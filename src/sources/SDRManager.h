#ifndef SDRMANAGER_H
#define SDRMANAGER_H

#include <QObject>
#include "IQSource.h"
#include "RTLDevice.h"
#include "RadioTypes.h"

namespace HFSDR
{
class SDRManager : public QObject
{
    Q_OBJECT
public:
    explicit SDRManager(QObject* parent = nullptr);
    SDRType selectedType() const;
    IQSource* activeSource();
    const IQSource* activeSource() const;
    bool selectSdrType(SDRType type);

private:
    SDRType m_selectedType = SDRType::RTLSDR;
    RTLDevice m_rtlDevice;
    IQSource* m_activeSource = nullptr;
};
}

#endif
