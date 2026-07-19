#include "SpectrumEngine.h"

namespace HFSDR
{

SpectrumEngine::SpectrumEngine(std::size_t fftSize)
    : m_fftProcessor(fftSize)
{
    m_detector.setMode(
        SpectrumDetectorMode::Average
        );

    m_detector.setAveragingAlpha(0.20f);
    m_calibrator.setMinimumDbfs(-160.0f);
}

std::size_t SpectrumEngine::fftSize() const
{
    return m_fftProcessor.fftSize();
}

void SpectrumEngine::setDetectorMode(
    SpectrumDetectorMode mode)
{
    m_detector.setMode(mode);
}

void SpectrumEngine::setAveragingAlpha(float alpha)
{
    m_detector.setAveragingAlpha(alpha);
}

void SpectrumEngine::setSmoothingEnabled(bool enabled)
{
    m_smoother.setEnabled(enabled);
}

void SpectrumEngine::setSmoothingWindowSize(int windowSize)
{
    m_smoother.setWindowSize(windowSize);
}

void SpectrumEngine::setSmoothingDownwardThresholdDb(float thresholdDb)
{
    m_smoother.setDownwardThresholdDb(thresholdDb);
}

void SpectrumEngine::setSmoothingBlend(float blend)
{
    m_smoother.setBlend(blend);
}

void SpectrumEngine::reset()
{
    m_detector.reset();
}

bool SpectrumEngine::processFrame(
    const IQBuffer& frame,
    std::vector<float>& outputDbfs)
{
    if (!m_fftProcessor.process(frame, m_fftData)) {
        outputDbfs.clear();
        return false;
    }

    m_calibrator.normaliseTonePower(
        m_fftData,
        m_fftProcessor.windowSum(),
        m_currentPower
        );

    m_detector.process(
        m_currentPower,
        m_detectedPower
        );

    m_calibrator.powerToDbfs(
        m_detectedPower,
        m_unsmoothedDbfs
        );

    m_smoother.process(
        m_unsmoothedDbfs,
        outputDbfs
        );

    return true;
}

} // namespace HFSDR
