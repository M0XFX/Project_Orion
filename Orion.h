#ifndef ORION_H
#define ORION_H

#include <QObject>

#include "Radio.h"
#include "Receiver.h"

#include "DisplaySettings.h"

namespace HFSDR
{

class Orion : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* radio READ radio CONSTANT)
    Q_PROPERTY(QObject* receiver READ receiver CONSTANT)
    Q_PROPERTY(DisplaySettings* displaySettings READ displaySettings CONSTANT)

    DisplaySettings* displaySettings();

public:
    explicit Orion(QObject *parent = nullptr);

    QObject* radio();
    QObject* receiver();

private:
    Radio m_radio;
    Receiver m_receiver;
    DisplaySettings m_displaySettings;
};

} // namespace HFSDR

#endif
