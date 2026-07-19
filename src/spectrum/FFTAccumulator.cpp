#include "FFTAccumulator.h"

#include <algorithm>

namespace HFSDR
{

FFTAccumulator::FFTAccumulator(std::size_t frameSize)
    : m_frame(std::max<std::size_t>(frameSize, 2))
{
}

std::size_t FFTAccumulator::frameSize() const
{
    return m_frame.size();
}

std::size_t FFTAccumulator::bufferedSampleCount() const
{
    return m_writeIndex;
}

void FFTAccumulator::reset()
{
    m_writeIndex = 0;
}

bool FFTAccumulator::append(
    const IQBuffer& input,
    IQBuffer& outputFrame)
{
    if (input.empty())
        return false;

    std::size_t inputIndex = 0;

    while (inputIndex < input.size()) {
        const std::size_t available =
            m_frame.size() - m_writeIndex;

        const std::size_t copyCount =
            std::min(available, input.size() - inputIndex);

        std::copy_n(
            input.samples().begin() +
                static_cast<std::ptrdiff_t>(inputIndex),
            static_cast<std::ptrdiff_t>(copyCount),
            m_frame.samples().begin() +
                static_cast<std::ptrdiff_t>(m_writeIndex)
            );

        inputIndex += copyCount;
        m_writeIndex += copyCount;

        if (m_writeIndex == m_frame.size()) {
            outputFrame = m_frame;
            m_writeIndex = 0;
            return true;
        }
    }

    return false;
}

} // namespace HFSDR
