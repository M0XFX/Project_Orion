#ifndef SPECTRUMRENDERER_H
#define SPECTRUMRENDERER_H

#include "GraphicsTypes.h"

namespace HFSDR::Graphics
{
class SpectrumRenderer
{
public:
    SpectrumRenderer() = default;

    [[nodiscard]] SpectrumRenderGeometry buildGeometry(
        const SpectrumFrame& frame,
        qreal width,
        qreal height,
        float minimumDbfs,
        float maximumDbfs) const;

private:
    [[nodiscard]] static qreal mapBinToX(
        std::size_t binIndex,
        std::size_t binCount,
        qreal width) noexcept;

    [[nodiscard]] static qreal mapDbfsToY(
        float valueDbfs,
        float minimumDbfs,
        float maximumDbfs,
        qreal height) noexcept;
};
}

#endif
