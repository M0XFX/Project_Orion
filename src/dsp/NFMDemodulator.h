#ifndef NFMDEMODULATOR_H
#define NFMDEMODULATOR_H

#include <complex>
#include <vector>

#include "IQBuffer.h"

namespace HFSDR
{

class NFMDemodulator
{
public:
    NFMDemodulator() = default;

    void process(
        const IQBuffer& input,
        std::vector<float>& outputAudio
        );

    void reset();

private:
    IQBuffer::Sample m_previousSample =
        IQBuffer::Sample(1.0f, 0.0f);
};

} // namespace HFSDR

#endif
