#ifndef IQBUFFER_H
#define IQBUFFER_H

#include <complex>
#include <vector>
#include <cstddef>

namespace HFSDR
{

class IQBuffer
{
public:
    using Sample = std::complex<float>;

    IQBuffer() = default;
    explicit IQBuffer(std::size_t sampleCount);

    void resize(std::size_t sampleCount);
    void clear();

    Sample* data();
    const Sample* data() const;

    std::vector<Sample>& samples();
    const std::vector<Sample>& samples() const;

    std::size_t size() const;
    bool empty() const;

private:
    std::vector<Sample> m_samples;
};

} // namespace HFSDR

#endif
