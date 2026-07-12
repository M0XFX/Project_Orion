#include "FFTProcessor.h"

#include <cmath>
#include <algorithm>

FFTProcessor::FFTProcessor()
{
}

void FFTProcessor::process(const HFSDR::IQBuffer& input,
                           std::vector<float>& outputDb)
{
    const std::size_t n = input.size();

    if (n == 0)
        return;

    std::vector<std::complex<float>> data(input.samples().begin(),
                                          input.samples().end());

    applyHannWindow(data);
    fft(data);

    outputDb.clear();
    outputDb.reserve(n);

    // FFT shift: put negative frequencies on the left, positive on the right
    const std::size_t half = n / 2;

    for (std::size_t i = 0; i < n; ++i) {
        const std::size_t index = (i + half) % n;

        const float real = data[index].real();
        const float imag = data[index].imag();

        const float power = real * real + imag * imag;
        const float db = 10.0f * std::log10(power + 1.0e-12f);

        outputDb.push_back(db);
    }
}

void FFTProcessor::applyHannWindow(std::vector<std::complex<float>>& data)
{
    const std::size_t n = data.size();

    if (n < 2)
        return;

    constexpr float pi = 3.14159265358979323846f;

    for (std::size_t i = 0; i < n; ++i) {
        const float w = 0.5f * (1.0f - std::cos(2.0f * pi * i / (n - 1)));
        data[i] *= w;
    }
}

void FFTProcessor::fft(std::vector<std::complex<float>>& data)
{
    const std::size_t n = data.size();

    if (n <= 1)
        return;

    // Bit reversal
    std::size_t j = 0;

    for (std::size_t i = 1; i < n; ++i) {
        std::size_t bit = n >> 1;

        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }

        j ^= bit;

        if (i < j)
            std::swap(data[i], data[j]);
    }

    constexpr float pi = 3.14159265358979323846f;

    for (std::size_t len = 2; len <= n; len <<= 1) {
        const float angle = -2.0f * pi / static_cast<float>(len);
        const std::complex<float> wLen(std::cos(angle), std::sin(angle));

        for (std::size_t i = 0; i < n; i += len) {
            std::complex<float> w(1.0f, 0.0f);

            for (std::size_t k = 0; k < len / 2; ++k) {
                const std::complex<float> u = data[i + k];
                const std::complex<float> v = data[i + k + len / 2] * w;

                data[i + k] = u + v;
                data[i + k + len / 2] = u - v;

                w *= wLen;
            }
        }
    }
}