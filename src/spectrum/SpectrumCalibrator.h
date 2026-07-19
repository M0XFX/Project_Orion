#ifndef SPECTRUMCALIBRATOR_H
#define SPECTRUMCALIBRATOR_H

#include <complex>
#include <vector>

namespace HFSDR
{

class SpectrumCalibrator
{
public:
    void normaliseTonePower(
        const std::vector<std::complex<float>>& fftData,
        float windowSum,
        std::vector<float>& outputPower
        ) const;

    void powerToDbfs(
        const std::vector<float>& inputPower,
        std::vector<float>& outputDbfs
        ) const;

    void setMinimumDbfs(float minimumDbfs);
    float minimumDbfs() const;

private:
    float m_minimumDbfs = -160.0f;
};

} // namespace HFSDR

#endif
