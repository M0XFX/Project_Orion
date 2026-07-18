#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

#include <QObject>
#include "Radio.h"
#include "Receiver.h"
#include "SDRManager.h"

namespace HFSDR
{
class RadioController : public QObject
{
    Q_OBJECT
public:
    explicit RadioController(Radio* radio, Receiver* receiver, SDRManager* sdrManager, QObject* parent = nullptr);
    bool start();
    void applyAllSettings();

private slots:
    void applyFrequency();
    void applyReceiverConfiguration();
    void applySpectrumSpan();
    void applyGainMode();
    void applyManualGain();
    void applySdrSelection();

private:
    Radio* m_radio = nullptr;
    Receiver* m_receiver = nullptr;
    SDRManager* m_sdrManager = nullptr;
    bool m_started = false;
};
}

#endif
