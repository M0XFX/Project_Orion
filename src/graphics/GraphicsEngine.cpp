#include "GraphicsEngine.h"

namespace HFSDR::Graphics
{
GraphicsEngine::GraphicsEngine() = default;

ThemeManager& GraphicsEngine::themeManager() noexcept
{
    return m_themeManager;
}

const ThemeManager& GraphicsEngine::themeManager() const noexcept
{
    return m_themeManager;
}

SpectrumRenderer& GraphicsEngine::spectrumRenderer() noexcept
{
    return m_spectrumRenderer;
}

const SpectrumRenderer& GraphicsEngine::spectrumRenderer() const noexcept
{
    return m_spectrumRenderer;
}

void GraphicsEngine::setSpectrumTheme(SpectrumThemeId themeId) noexcept
{
    m_themeManager.setActiveSpectrumTheme(themeId);
}

SpectrumThemeId GraphicsEngine::spectrumThemeId() const noexcept
{
    return m_themeManager.activeSpectrumThemeId();
}

const SpectrumTheme& GraphicsEngine::spectrumTheme() const noexcept
{
    return m_themeManager.activeSpectrumTheme();
}

void GraphicsEngine::setSpectrumFillEnabled(bool enabled) noexcept
{
    m_spectrumFillEnabled = enabled;
}

bool GraphicsEngine::spectrumFillEnabled() const noexcept
{
    return m_spectrumFillEnabled;
}

void GraphicsEngine::setSpectrumGradientEnabled(bool enabled) noexcept
{
    m_spectrumGradientEnabled = enabled;
}

bool GraphicsEngine::spectrumGradientEnabled() const noexcept
{
    return m_spectrumGradientEnabled;
}

void GraphicsEngine::setPeakHoldEnabled(bool enabled) noexcept
{
    m_peakHoldEnabled = enabled;
}

bool GraphicsEngine::peakHoldEnabled() const noexcept
{
    return m_peakHoldEnabled;
}
}
