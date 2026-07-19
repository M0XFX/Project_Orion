#include "ThemeManager.h"

namespace HFSDR::Graphics
{
namespace
{
const SpectrumTheme classicGreenTheme = makeClassicGreenTheme();
const SpectrumTheme sdrSharpTheme = makeSdrSharpTheme();
}

ThemeManager::ThemeManager() = default;

SpectrumThemeId ThemeManager::activeSpectrumThemeId() const noexcept
{
    return m_activeSpectrumThemeId;
}

const SpectrumTheme& ThemeManager::activeSpectrumTheme() const noexcept
{
    return spectrumTheme(m_activeSpectrumThemeId);
}

void ThemeManager::setActiveSpectrumTheme(SpectrumThemeId themeId) noexcept
{
    m_activeSpectrumThemeId = themeId;
}

const SpectrumTheme& ThemeManager::spectrumTheme(SpectrumThemeId themeId) noexcept
{
    switch (themeId) {
    case SpectrumThemeId::SDRSharp:
        return sdrSharpTheme;

    case SpectrumThemeId::ClassicGreen:
    default:
        return classicGreenTheme;
    }
}
}
