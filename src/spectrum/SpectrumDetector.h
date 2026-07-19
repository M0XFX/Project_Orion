#ifndef SPECTRUMDETECTOR_H
#define SPECTRUMDETECTOR_H

#include <vector>

namespace HFSDR
{

enum class SpectrumDetectorMode
{
    Sample,
    Average,
    Peak
};

class SpectrumDetector
{
public:
    void setMode(SpectrumDetectorMode mode);
    SpectrumDetectorMode mode() const;

    void setAveragingAlpha(float alpha);
    float averagingAlpha() const;

    void reset();

    void process(
        const std::vector<float>& inputPower,
        std::vector<float>& outputPower
        );

private:
    SpectrumDetectorMode m_mode =
        SpectrumDetectorMode::Average;

    float m_averagingAlpha = 0.20f;
    bool m_initialised = false;
    std::vector<float> m_state;
};

} // namespace HFSDR

#endif
