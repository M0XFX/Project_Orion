#include "SpectrumTheme.h"

namespace HFSDR::Graphics
{
SpectrumTheme makeClassicGreenTheme()
{
    SpectrumTheme theme;
    theme.name = QStringLiteral("Classic Green");

    theme.backgroundColour = QColor(0, 0, 0);
    theme.gridColour = QColor(55, 70, 55);
    theme.traceColour = QColor(0, 255, 0);
    theme.peakHoldColour = QColor(255, 170, 0);
    theme.centreLineColour = QColor(255, 64, 64);
    theme.scaleTextColour = QColor(190, 210, 190);

    theme.fillGradient = {
        {0.00, QColor(0, 255, 0, 150)},
        {0.45, QColor(0, 150, 0, 105)},
        {1.00, QColor(0, 25, 0, 25)}
    };

    theme.traceWidth = 0.75; //1.25
    theme.peakHoldWidth = 1.0;
    theme.centreLineWidth = 1.0;

    return theme;
}

SpectrumTheme makeSdrSharpTheme()
{
    SpectrumTheme theme;
    theme.name = QStringLiteral("SDRSharp");

    theme.backgroundColour = QColor(0, 0, 0);
    theme.gridColour = QColor(78, 78, 78);
    theme.traceColour = QColor(255, 255, 255);
    theme.peakHoldColour = QColor(255, 153, 0);
    theme.centreLineColour = QColor(255, 32, 32);
    theme.scaleTextColour = QColor(210, 210, 210);

    theme.fillGradient = {
        {0.00, QColor(235, 248, 255, 210)},
        {0.18, QColor(115, 195, 255, 190)},
        {0.48, QColor(28, 110, 210, 165)},
        {0.76, QColor(8, 42, 115, 130)},
        {1.00, QColor(1, 8, 28, 45)}
    };

    theme.traceWidth = 1.5;
    theme.peakHoldWidth = 1.0;
    theme.centreLineWidth = 1.0;

    return theme;
}
}
