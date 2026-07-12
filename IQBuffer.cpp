#include "IQBuffer.h"

namespace HFSDR
{

IQBuffer::IQBuffer(std::size_t sampleCount)
    : m_samples(sampleCount)
{
}

void IQBuffer::resize(std::size_t sampleCount)
{
    m_samples.resize(sampleCount);
}

void IQBuffer::clear()
{
    m_samples.clear();
}

IQBuffer::Sample* IQBuffer::data()
{
    return m_samples.data();
}

const IQBuffer::Sample* IQBuffer::data() const
{
    return m_samples.data();
}

std::vector<IQBuffer::Sample>& IQBuffer::samples()
{
    return m_samples;
}

const std::vector<IQBuffer::Sample>& IQBuffer::samples() const
{
    return m_samples;
}

std::size_t IQBuffer::size() const
{
    return m_samples.size();
}

bool IQBuffer::empty() const
{
    return m_samples.empty();
}

} // namespace HFSDR