#ifndef SPECTRUMTHEME_H
#define SPECTRUMTHEME_H

#include <QColor>
#include <QGradient>
#include <QString>

namespace HFSDR::Graphics
{
enum class SpectrumThemeId
{
    ClassicGreen,
    SDRSharp
};

struct SpectrumTheme
{
    QString name;

    QColor backgroundColour;
    QColor gridColour;
    QColor traceColour;
    QColor peakHoldColour;
    QColor centreLineColour;
    QColor scaleTextColour;

    QGradientStops fillGradient;

    qreal traceWidth = 1.0;
    qreal peakHoldWidth = 1.0;
    qreal centreLineWidth = 1.0;
};

SpectrumTheme makeClassicGreenTheme();
SpectrumTheme makeSdrSharpTheme();
}

#endif
