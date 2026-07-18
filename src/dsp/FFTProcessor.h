#ifndef FFTPROCESSOR_H
#define FFTPROCESSOR_H

#include <complex>
#include <vector>

#include "IQBuffer.h"

class FFTProcessor
{
public:
    FFTProcessor();

    void setAveragingAlpha(
        float alpha
        );

    void process(
        const HFSDR::IQBuffer& input,
        std::vector<float>& outputDb
        );

private:
    void fft(
        std::vector<std::complex<float>>& data
        );

    void applyHannWindow(
        std::vector<std::complex<float>>& data
        );

    std::vector<float> m_averagedPower;

    float m_averagingAlpha = 0.20f;
    bool m_averagingInitialised = false;
};

#endif