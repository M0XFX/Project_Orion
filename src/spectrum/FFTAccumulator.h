#ifndef FFTACCUMULATOR_H
#define FFTACCUMULATOR_H

#include <cstddef>

#include "IQBuffer.h"

namespace HFSDR
{

class FFTAccumulator
{
public:
    explicit FFTAccumulator(std::size_t frameSize = 4096);

    std::size_t frameSize() const;
    std::size_t bufferedSampleCount() const;

    void reset();

    // Appends samples to the current FFT frame. Returns true when a
    // complete frame is available in outputFrame.
    bool append(const IQBuffer& input, IQBuffer& outputFrame);

private:
    IQBuffer m_frame;
    std::size_t m_writeIndex = 0;
};

} // namespace HFSDR

#endif
