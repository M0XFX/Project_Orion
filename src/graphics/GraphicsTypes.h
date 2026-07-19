#ifndef GRAPHICSTYPES_H
#define GRAPHICSTYPES_H

#include <QPointF>

#include <cstdint>
#include <vector>

namespace HFSDR::Graphics
{
struct SpectrumFrame
{
    std::vector<float> binsDbfs;
    std::vector<float> peakBinsDbfs;

    double centreFrequencyHz = 0.0;
    double spanHz = 0.0;
    float referenceLevelDbfs = 0.0f;
    float noiseFloorDbfs = -120.0f;
    std::uint64_t sequenceNumber = 0;

    [[nodiscard]] bool empty() const noexcept
    {
        return binsDbfs.empty();
    }
};

struct SpectrumRenderGeometry
{
    std::vector<QPointF> tracePoints;
    std::vector<QPointF> peakHoldPoints;
    std::vector<QPointF> fillPolygon;

    void clear()
    {
        tracePoints.clear();
        peakHoldPoints.clear();
        fillPolygon.clear();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return tracePoints.empty();
    }
};
}

#endif
