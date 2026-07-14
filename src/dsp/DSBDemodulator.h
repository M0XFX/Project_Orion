#ifndef DSBDEMODULATOR_H
#define DSBDEMODULATOR_H

#include <vector>

#include "IQBuffer.h"

namespace HFSDR
{

class DSBDemodulator
{
public:
    DSBDemodulator() = default;

    void process(
        const IQBuffer& input,
        std::vector<float>& outputAudio
        );
};

} // namespace HFSDR

#endif
