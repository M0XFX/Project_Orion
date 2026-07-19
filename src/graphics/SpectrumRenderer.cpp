#include "SpectrumRenderer.h"

#include <algorithm>
#include <cmath>

namespace HFSDR::Graphics
{
SpectrumRenderGeometry SpectrumRenderer::buildGeometry(
    const SpectrumFrame& frame,
    qreal width,
    qreal height,
    float minimumDbfs,
    float maximumDbfs) const
{
    SpectrumRenderGeometry geometry;

    if (frame.empty()
        || width <= 0.0
        || height <= 0.0
        || !std::isfinite(minimumDbfs)
        || !std::isfinite(maximumDbfs)
        || maximumDbfs <= minimumDbfs) {
        return geometry;
    }

    const std::size_t binCount = frame.binsDbfs.size();
    geometry.tracePoints.reserve(binCount);

    for (std::size_t index = 0; index < binCount; ++index) {
        geometry.tracePoints.emplace_back(
            mapBinToX(index, binCount, width),
            mapDbfsToY(frame.binsDbfs[index], minimumDbfs, maximumDbfs, height));
    }

    if (!frame.peakBinsDbfs.empty()) {
        const std::size_t peakCount = frame.peakBinsDbfs.size();
        geometry.peakHoldPoints.reserve(peakCount);

        for (std::size_t index = 0; index < peakCount; ++index) {
            geometry.peakHoldPoints.emplace_back(
                mapBinToX(index, peakCount, width),
                mapDbfsToY(frame.peakBinsDbfs[index], minimumDbfs, maximumDbfs, height));
        }
    }

    geometry.fillPolygon.reserve(geometry.tracePoints.size() + 2U);
    geometry.fillPolygon.emplace_back(0.0, height);
    geometry.fillPolygon.insert(
        geometry.fillPolygon.end(),
        geometry.tracePoints.begin(),
        geometry.tracePoints.end());
    geometry.fillPolygon.emplace_back(width, height);

    return geometry;
}

qreal SpectrumRenderer::mapBinToX(
    std::size_t binIndex,
    std::size_t binCount,
    qreal width) noexcept
{
    if (binCount <= 1U) {
        return width * 0.5;
    }

    const qreal denominator = static_cast<qreal>(binCount - 1U);
    return (static_cast<qreal>(binIndex) / denominator) * width;
}

qreal SpectrumRenderer::mapDbfsToY(
    float valueDbfs,
    float minimumDbfs,
    float maximumDbfs,
    qreal height) noexcept
{
    const float safeValue = std::isfinite(valueDbfs) ? valueDbfs : minimumDbfs;
    const float clampedValue = std::clamp(safeValue, minimumDbfs, maximumDbfs);
    const float normalised =
        (clampedValue - minimumDbfs) / (maximumDbfs - minimumDbfs);

    return height * (1.0 - static_cast<qreal>(normalised));
}
}
