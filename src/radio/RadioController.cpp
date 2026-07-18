#include "RadioController.h"
#include "Logger.h"

namespace HFSDR
{
RadioController::RadioController(Radio* radio, Receiver* receiver, SDRManager* sdrManager, QObject* parent)
    : QObject(parent), m_radio(radio), m_receiver(receiver), m_sdrManager(sdrManager)
{
    Q_ASSERT(m_radio && m_receiver && m_sdrManager);
    connect(m_radio, &Radio::frequencyChanged, this, &RadioController::applyFrequency);
    connect(m_radio, &Radio::receiverConfigurationChanged, this, &RadioController::applyReceiverConfiguration);
    connect(m_radio, &Radio::spectrumSpanHzChanged, this, &RadioController::applySpectrumSpan);
    connect(m_radio, &Radio::automaticRfGainChanged, this, &RadioController::applyGainMode);
    connect(m_radio, &Radio::rfGainDbChanged, this, &RadioController::applyManualGain);
    connect(m_radio, &Radio::sdrTypeChanged, this, &RadioController::applySdrSelection);
    Logger::info("RadioController initialised.");
}

bool RadioController::start()
{
    applySdrSelection();
    if (!m_receiver->openActiveSource()) {
        Logger::error("RadioController failed to start the selected SDR.");
        return false;
    }
    m_started = true;
    applyAllSettings();
    Logger::info("RadioController started.");
    return true;
}

void RadioController::applyAllSettings()
{
    applyReceiverConfiguration();
    applySpectrumSpan();
    applyFrequency();
    applyGainMode();
}

void RadioController::applyFrequency()
{
    if (!m_started)
        return;
    if (!m_receiver->setCenterFrequencyHz(m_radio->frequencyHz()))
        Logger::warning(QString("Failed to apply %1 Hz to active SDR.").arg(m_radio->frequencyHz()));
}

void RadioController::applyReceiverConfiguration()
{
    if (!m_started)
        return;
    m_receiver->setReceiverConfiguration(m_radio->receiverConfiguration());
}

void RadioController::applySpectrumSpan()
{
    if (!m_started)
        return;
    m_receiver->setSpectrumSpanHz(static_cast<int>(m_radio->spectrumSpanHz()));
}

void RadioController::applyGainMode()
{
    if (!m_started)
        return;
    if (!m_receiver->setAutomaticGain(m_radio->automaticRfGain())) {
        Logger::warning("Failed to apply RF gain mode.");
        return;
    }
    if (!m_radio->automaticRfGain())
        applyManualGain();
}

void RadioController::applyManualGain()
{
    if (!m_started)
        return;
    if (m_radio->automaticRfGain())
        return;
    if (!m_receiver->setRfGainDb(m_radio->rfGainDb()))
        Logger::warning(QString("Failed to apply RF gain %1 dB.").arg(m_radio->rfGainDb(), 0, 'f', 1));
}

void RadioController::applySdrSelection()
{
    if (!m_sdrManager->selectSdrType(m_radio->sdrType()))
        Logger::error("Failed to select requested SDR.");
}
}
