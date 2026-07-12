#include "AudioDecimator.h"

namespace HFSDR
{

AudioDecimator::AudioDecimator()
{
}

void AudioDecimator::setFactor(int factor)
{
    if (factor < 1)
        factor = 1;

    m_factor = factor;
}

int AudioDecimator::factor() const
{
    return m_factor;
}

void AudioDecimator::process(const std::vector<float>& input,
                             std::vector<float>& output)
{
    if (input.empty()) {
        output.clear();
        return;
    }

    const std::size_t outputSize = input.size() /
                                   static_cast<std::size_t>(m_factor);

    output.resize(outputSize);

    for (std::size_t i = 0; i < outputSize; ++i)
        output[i] = input[i * static_cast<std::size_t>(m_factor)];
}

} // namespace HFSDR
