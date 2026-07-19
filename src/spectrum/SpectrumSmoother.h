#ifndef SPECTRUMSMOOTHER_H
#define SPECTRUMSMOOTHER_H

#include <cstddef>
#include <vector>

namespace HFSDR
{

class SpectrumSmoother
{
public:
    bool enabled() const;
    void setEnabled(bool enabled);

    int windowSize() const;
    void setWindowSize(int windowSize);

    float downwardThresholdDb() const;
    void setDownwardThresholdDb(float thresholdDb);

    float blend() const;
    void setBlend(float blend);

    void process(
        const std::vector<float>& inputDbfs,
        std::vector<float>& outputDbfs
        ) const;

private:
    bool m_enabled = true;
    int m_windowSize = 5;
    float m_downwardThresholdDb = 1.5f;
    float m_blend = 0.75f;
};

} // namespace HFSDR

#endif
