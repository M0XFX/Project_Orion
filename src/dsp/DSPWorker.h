#ifndef DSPWORKER_H
#define DSPWORKER_H

#include <QObject>
#include <QElapsedTimer>
#include <QVariantList>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

#include "AudioOutput.h"
#include "DigitalDownConverter.h"
#include "DSPProcessor.h"
#include "FFTProcessor.h"
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

    // Thread-safe. Receiver may call this from
    // Orion's GUI/control thread.
    void setConfiguration(
        const ReceiverConfiguration& configuration
        );

    void setSpectrumSpanHz(
        int spanHz
        );



public slots:
    void start();
    void stop();

signals:
    void started();
    void stopped();

    void spectrumReady(
        const QVariantList& spectrumBins
        );

    void errorOccurred(
        const QString& message
        );

private:
    bool receiveNextBlock();

    void applyPendingConfiguration();
    void applyPendingSpectrumSpan();
    void processReceiverPath();
    void processDisplayPath();

    bool spectrumUpdateDue();
    void publishSpectrum();

    bool startAudio();
    void stopAudio();

    IQBlockRingBuffer* m_inputBuffer = nullptr;

    std::atomic_bool m_running = false;

    DigitalDownConverter m_displayDdc;
    DigitalDownConverter m_receiverDdc;

    DSPProcessor m_receiverProcessor;
    FFTProcessor m_fftProcessor;

    IQBuffer m_inputBlock;
    IQBuffer m_displayBuffer;
    IQBuffer m_receiverBuffer;

    std::vector<float> m_fftBins;
    std::vector<float> m_audioBuffer;

    std::unique_ptr<AudioOutput> m_audioOutput;

    QElapsedTimer m_spectrumTimer;
    qint64 m_spectrumIntervalMs = 33;

    mutable std::mutex m_configurationMutex;

    ReceiverConfiguration
        m_pendingConfiguration;

    bool m_configurationDirty = true;

    mutable std::mutex
        m_spectrumSpanMutex;

    int m_pendingSpectrumSpanHz =
        250000;

    int m_spectrumSpanHz =
        250000;

    bool m_spectrumSpanDirty =
        true;

    //float m_baseFrequencyShiftHz =
        //100000.0f;
    float m_baseFrequencyShiftHz = 0.0f;





};

} // namespace HFSDR

#endif