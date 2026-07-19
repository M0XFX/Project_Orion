#ifndef SPECTRUMFREQUENCYSMOOTHER_H
#define SPECTRUMFREQUENCYSMOOTHER_H

#include <vector>

namespace HFSDR
{

class SpectrumFrequencySmoother
{
public:
    bool enabled() const;
    void setEnabled(bool enabled);

    int radius() const;
    void setRadius(int radius);

    float strength() const;
    void setStrength(float strength);

    void process(
        const std::vector<float>& inputDbfs,
        std::vector<float>& outputDbfs
        ) const;

private:
    std::vector<float> makeKernel() const;

    bool m_enabled = true;
    int m_radius = 2;
    float m_strength = 0.65f;
};

} // namespace HFSDR

#endif
