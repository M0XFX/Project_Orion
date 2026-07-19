#ifndef SPECTRUM_FFTPROCESSOR_H
#define SPECTRUM_FFTPROCESSOR_H

#include <complex>
#include <cstddef>
#include <vector>

#include "IQBuffer.h"

namespace HFSDR
{

class FFTProcessor
{
public:
    explicit FFTProcessor(std::size_t fftSize = 4096);

    std::size_t fftSize() const;
    float windowSum() const;
    float equivalentNoiseBandwidthBins() const;

    bool process(
        const IQBuffer& input,
        std::vector<std::complex<float>>& outputFft
        );

private:
    void rebuildWindow();
    void transform(std::vector<std::complex<float>>& data) const;

    std::size_t m_fftSize = 4096;
    std::vector<float> m_window;
    float m_windowSum = 1.0f;
    float m_windowPowerSum = 1.0f;
};

} // namespace HFSDR

#endif
