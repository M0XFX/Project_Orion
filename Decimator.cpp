#include "Decimator.h"

namespace HFSDR
{

Decimator::Decimator()
{
}

void Decimator::setFactor(int factor)
{
    if (factor < 1)
        factor = 1;

    m_factor = factor;
}

int Decimator::factor() const
{
    return m_factor;
}

void Decimator::process(const IQBuffer& input, IQBuffer& output)
{
    if (input.empty()) {
        output.resize(0);
        return;
    }

    const std::size_t outputSize = input.size() / m_factor;
    output.resize(outputSize);

    for (std::size_t i = 0; i < outputSize; ++i) {
        output.samples()[i] = input.samples()[i * m_factor];
    }
}

} // namespace HFSDR