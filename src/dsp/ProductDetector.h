#ifndef PRODUCTDETECTOR_H
#define PRODUCTDETECTOR_H

#include <vector>

#include "IQBuffer.h"
#include "ReceiverConfiguration.h"

namespace HFSDR
{

class ProductDetector
{
public:
    ProductDetector() = default;

    void setMode(DemodulationMode mode);

    void process(
        const IQBuffer& input,
        std::vector<float>& outputAudio
        );

    void reset();

private:
    DemodulationMode m_mode =
        DemodulationMode::USB;
};

} // namespace HFSDR

#endif