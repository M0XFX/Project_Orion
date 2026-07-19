#ifndef SPECTRUMENGINE_H
#define SPECTRUMENGINE_H

#include <complex>
#include <cstddef>
#include <vector>

#include "FFTProcessor.h"
#include "IQBuffer.h"
#include "SpectrumCalibrator.h"
#include "SpectrumDetector.h"

namespace HFSDR
{

class SpectrumEngine
{
public:
    explicit SpectrumEngine(std::size_t fftSize = 4096);

    std::size_t fftSize() const;

    void setDetectorMode(SpectrumDetectorMode mode);
    void setAveragingAlpha(float alpha);
    void reset();

    bool processFrame(
        const IQBuffer& frame,
        std::vector<float>& outputDbfs
        );

private:
    FFTProcessor m_fftProcessor;
    SpectrumCalibrator m_calibrator;
    SpectrumDetector m_detector;

    std::vector<std::complex<float>> m_fftData;
    std::vector<float> m_currentPower;
    std::vector<float> m_detectedPower;
};

} // namespace HFSDR

#endif
