#ifndef AUDIOFIRFILTER_H
#define AUDIOFIRFILTER_H

#include <vector>

namespace HFSDR
{

class AudioFIRFilter
{
public:
    AudioFIRFilter();

    void setLowPass(float sampleRate,
                    float cutoffHz,
                    int tapCount);

    void process(const std::vector<float>& input,
                 std::vector<float>& output);

private:
    std::vector<float> m_taps;
    std::vector<float> m_delayLine;
};

} // namespace HFSDR

#endif
