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

    // Thread-safe spectrum presentation controls.
    void setSmoothingEnabled(bool enabled);
    void setSmoothingWindowSize(int windowSize);
    void setSmoothingDownwardThresholdDb(float thresholdDb);
    void setSmoothingBlend(float blend);

    void setFrequencySmoothingEnabled(bool enabled);
    void setFrequencySmoothingRadius(int radius);
    void setFrequencySmoothingStrength(float strength);

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
    void applyPendingSmoothingSettings();
    void applyPendingFrequencySmoothingSettings();
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

    mutable std::mutex m_smoothingMutex;
    bool m_pendingSmoothingEnabled = true;
    int m_pendingSmoothingWindowSize = 5;
    float m_pendingSmoothingThresholdDb = 1.5f;
    float m_pendingSmoothingBlend = 0.75f;
    bool m_smoothingDirty = true;

    mutable std::mutex m_frequencySmoothingMutex;
    bool m_pendingFrequencySmoothingEnabled = true;
    int m_pendingFrequencySmoothingRadius = 2;
    float m_pendingFrequencySmoothingStrength = 0.65f;
    bool m_frequencySmoothingDirty = true;
};

} // namespace HFSDR

#endif
