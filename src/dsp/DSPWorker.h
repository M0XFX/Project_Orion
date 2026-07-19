#ifndef DSPWORKER_H
#define DSPWORKER_H

#include <QObject>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "AudioOutput.h"
#include "DigitalDownConverter.h"
#include "DSPProcessor.h"
#include "IQBlockRingBuffer.h"
#include "IQBuffer.h"
#include "ReceiverConfiguration.h"

namespace HFSDR
{

class DSPWorker : public QObject
{
    Q_OBJECT

public:
    explicit DSPWorker(
        IQBlockRingBuffer* inputBuffer,
        QObject* parent = nullptr
        );

    bool running() const;

    // Thread-safe. Receiver may call this from Orion's control thread.
    void setConfiguration(
        const ReceiverConfiguration& configuration
        );

public slots:
    void start();
    void stop();

signals:
    void started();
    void stopped();

    void errorOccurred(
        const QString& message
        );

private:
    bool receiveNextBlock();
    void applyPendingConfiguration();
    void processReceiverPath();

    bool startAudio();
    void stopAudio();

    IQBlockRingBuffer* m_inputBuffer = nullptr;
    std::atomic_bool m_running = false;

    DigitalDownConverter m_receiverDdc;
    DSPProcessor m_receiverProcessor;

    IQBuffer m_inputBlock;
    IQBuffer m_receiverBuffer;
    std::vector<float> m_audioBuffer;

    std::unique_ptr<AudioOutput> m_audioOutput;

    mutable std::mutex m_configurationMutex;
    ReceiverConfiguration m_pendingConfiguration;
    bool m_configurationDirty = true;

    float m_baseFrequencyShiftHz = 0.0f;
};

} // namespace HFSDR

#endif
