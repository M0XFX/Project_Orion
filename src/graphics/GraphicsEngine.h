#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

#include "SpectrumRenderer.h"
#include "ThemeManager.h"

namespace HFSDR::Graphics
{
class GraphicsEngine
{
public:
    GraphicsEngine();

    [[nodiscard]] ThemeManager& themeManager() noexcept;
    [[nodiscard]] const ThemeManager& themeManager() const noexcept;

    [[nodiscard]] SpectrumRenderer& spectrumRenderer() noexcept;
    [[nodiscard]] const SpectrumRenderer& spectrumRenderer() const noexcept;

    void setSpectrumTheme(SpectrumThemeId themeId) noexcept;
    [[nodiscard]] SpectrumThemeId spectrumThemeId() const noexcept;
    [[nodiscard]] const SpectrumTheme& spectrumTheme() const noexcept;

    void setSpectrumFillEnabled(bool enabled) noexcept;
    [[nodiscard]] bool spectrumFillEnabled() const noexcept;

    void setSpectrumGradientEnabled(bool enabled) noexcept;
    [[nodiscard]] bool spectrumGradientEnabled() const noexcept;

    void setPeakHoldEnabled(bool enabled) noexcept;
    [[nodiscard]] bool peakHoldEnabled() const noexcept;

private:
    ThemeManager m_themeManager;
    SpectrumRenderer m_spectrumRenderer;

    bool m_spectrumFillEnabled = true;
    bool m_spectrumGradientEnabled = true;
    bool m_peakHoldEnabled = false;
};
}

#endif
