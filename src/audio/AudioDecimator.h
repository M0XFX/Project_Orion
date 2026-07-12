#ifndef AUDIODECIMATOR_H
#define AUDIODECIMATOR_H

#include <vector>

namespace HFSDR
{

class AudioDecimator
{
public:
    AudioDecimator();

    void setFactor(int factor);
    int factor() const;

    void process(const std::vector<float>& input,
                 std::vector<float>& output);

private:
    int m_factor = 8;
};

} // namespace HFSDR

#endif
