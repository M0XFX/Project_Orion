#ifndef SPECTRUMWORKER_H
#define SPECTRUMWORKER_H

#include <QObject>
#include <QElapsedTimer>
#include <QVariantList>

#include <atomic>
#include <mutex>
#include <vector>

#include "DigitalDownConverter.h"
#include "FFTAccumulator.h"
#include "IQBlockRingBuffer.h"
#include "IQBuffer.h"
#include "SpectrumEngine.h"

namespace HFSDR
{

class SpectrumWorker : public QObject
{
    Q_OBJECT

public:
    explicit SpectrumWorker(
        IQBlockRingBuffer* inputBuffer,
        QObject* parent = nullptr
        );

    bool running() const;

    // Thread-safe. May be called by Receiver's control thread.
    void setSpectrumSpanHz(int spanHz);

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
    void applyPendingSpectrumSpan();
    bool spectrumUpdateDue();
    void publishSpectrum();

    IQBlockRingBuffer* m_inputBuffer = nullptr;
    std::atomic_bool m_running = false;

    DigitalDownConverter m_displayDdc;
    FFTAccumulator m_accumulator;
    SpectrumEngine m_engine;

    IQBuffer m_inputBlock;
    IQBuffer m_decimatedBlock;
    IQBuffer m_fftFrame;
    std::vector<float> m_fftBins;

    QElapsedTimer m_publishTimer;
    qint64 m_publishIntervalMs = 40;

    mutable std::mutex m_spectrumSpanMutex;
    int m_pendingSpectrumSpanHz = 250000;
    int m_spectrumSpanHz = 250000;
    bool m_spectrumSpanDirty = true;

    float m_baseFrequencyShiftHz = 0.0f;
};

} // namespace HFSDR

#endif
