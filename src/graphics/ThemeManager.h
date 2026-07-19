#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "SpectrumTheme.h"

namespace HFSDR::Graphics
{
class ThemeManager
{
public:
    ThemeManager();

    SpectrumThemeId activeSpectrumThemeId() const noexcept;
    const SpectrumTheme& activeSpectrumTheme() const noexcept;

    void setActiveSpectrumTheme(SpectrumThemeId themeId) noexcept;

    static const SpectrumTheme& spectrumTheme(SpectrumThemeId themeId) noexcept;

private:
    SpectrumThemeId m_activeSpectrumThemeId = SpectrumThemeId::ClassicGreen;
};
}

#endif
