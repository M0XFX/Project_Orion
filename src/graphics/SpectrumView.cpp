#include "SpectrumView.h"

#include "DisplaySettings.h"
#include "Receiver.h"

#include <QBrush>
#include <QFont>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>
#include <cmath>

namespace HFSDR::Graphics
{
namespace
{
constexpr int autoScaleIntervalMs = 500;
constexpr int verticalGridDivisions = 10;
constexpr float dbDivision = 10.0f;

QPainterPath pathFromPoints(const std::vector<QPointF>& points)
{
    QPainterPath path;
    if (points.empty())
        return path;

    path.moveTo(points.front());
    for (std::size_t index = 1; index < points.size(); ++index)
        path.lineTo(points[index]);

    return path;
}
}

SpectrumView::SpectrumView(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);

    m_graphicsEngine.setSpectrumTheme(SpectrumThemeId::ClassicGreen);
    m_graphicsEngine.setSpectrumFillEnabled(false);
    m_graphicsEngine.setSpectrumGradientEnabled(true);
    m_graphicsEngine.setPeakHoldEnabled(false);

    m_autoScaleTimer.setInterval(autoScaleIntervalMs);
    m_autoScaleTimer.setSingleShot(false);

    connect(
        &m_autoScaleTimer,
        &QTimer::timeout,
        this,
        &SpectrumView::updateAutomaticScale);
}

QObject* SpectrumView::receiver() const noexcept
{
    return m_receiver.data();
}

void SpectrumView::setReceiver(QObject* receiverObject)
{
    Receiver* newReceiver = qobject_cast<Receiver*>(receiverObject);
    if (m_receiver == newReceiver)
        return;

    disconnectReceiver();
    m_receiver = newReceiver;

    if (m_receiver) {
        connect(
            m_receiver,
            &Receiver::spectrumBinsChanged,
            this,
            &SpectrumView::refreshSpectrum);

        connect(
            m_receiver,
            &QObject::destroyed,
            this,
            [this]() {
                m_receiver = nullptr;
                m_binsDbfs.clear();
                m_peakBinsDbfs.clear();
                update();
            });
    }

    refreshSpectrum();
    emit receiverChanged();
}

QObject* SpectrumView::displaySettings() const noexcept
{
    return m_displaySettings.data();
}

void SpectrumView::setDisplaySettings(QObject* settingsObject)
{
    DisplaySettings* newSettings = qobject_cast<DisplaySettings*>(settingsObject);
    if (m_displaySettings == newSettings)
        return;

    disconnectDisplaySettings();
    m_displaySettings = newSettings;

    if (m_displaySettings) {
        connect(m_displaySettings, &DisplaySettings::peakHoldChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::gridEnabledChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::centerLineEnabledChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::spectrumFillEnabledChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::autoScaleChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::showDbScaleChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::spectrumOffsetDbChanged,
                this, &SpectrumView::refreshDisplaySettings);
        connect(m_displaySettings, &DisplaySettings::spectrumRangeDbChanged,
                this, &SpectrumView::refreshDisplaySettings);

        connect(
            m_displaySettings,
            &QObject::destroyed,
            this,
            [this]() {
                m_displaySettings = nullptr;
                m_autoScaleTimer.stop();
                update();
            });
    }

    refreshDisplaySettings();
    emit displaySettingsChanged();
}

void SpectrumView::disconnectReceiver()
{
    if (m_receiver)
        disconnect(m_receiver, nullptr, this, nullptr);
}

void SpectrumView::disconnectDisplaySettings()
{
    if (m_displaySettings)
        disconnect(m_displaySettings, nullptr, this, nullptr);
}

void SpectrumView::refreshSpectrum()
{
    m_binsDbfs.clear();

    if (m_receiver) {
        const QVariantList bins = m_receiver->spectrumBins();
        m_binsDbfs.reserve(static_cast<std::size_t>(bins.size()));

        for (const QVariant& bin : bins) {
            bool converted = false;
            const float value = bin.toFloat(&converted);
            if (converted && std::isfinite(value))
                m_binsDbfs.push_back(value);
            else
                m_binsDbfs.push_back(-160.0f);
        }
    }

    updatePeakHold();
    update();
}

void SpectrumView::refreshDisplaySettings()
{
    if (!m_displaySettings) {
        m_autoScaleTimer.stop();
        update();
        return;
    }

    const bool peakHoldEnabled = m_displaySettings->peakHold();
    m_graphicsEngine.setPeakHoldEnabled(peakHoldEnabled);
    m_graphicsEngine.setSpectrumFillEnabled(
        m_displaySettings->spectrumFillEnabled());

    if (!peakHoldEnabled)
        m_peakBinsDbfs.clear();
    else
        updatePeakHold();

    if (m_displaySettings->autoScale()) {
        if (!m_autoScaleTimer.isActive())
            m_autoScaleTimer.start();
    } else {
        m_autoScaleTimer.stop();
    }

    update();
}

void SpectrumView::updatePeakHold()
{
    if (!m_graphicsEngine.peakHoldEnabled() || m_binsDbfs.empty())
        return;

    if (m_peakBinsDbfs.size() != m_binsDbfs.size()) {
        m_peakBinsDbfs = m_binsDbfs;
        return;
    }

    for (std::size_t index = 0; index < m_binsDbfs.size(); ++index)
        m_peakBinsDbfs[index] = std::max(m_peakBinsDbfs[index], m_binsDbfs[index]);
}

void SpectrumView::paint(QPainter* painter)
{
    if (!painter)
        return;

    const QRectF bounds(0.0, 0.0, width(), height());
    const SpectrumTheme& theme = m_graphicsEngine.spectrumTheme();

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->fillRect(bounds, theme.backgroundColour);

    if (!m_displaySettings)
        return;

    if (m_displaySettings->gridEnabled())
        drawGrid(*painter, bounds, theme);

    drawSpectrum(*painter, bounds, theme);

    if (m_displaySettings->centerLineEnabled())
        drawCentreLine(*painter, bounds, theme);
}

void SpectrumView::drawGrid(
    QPainter& painter,
    const QRectF& bounds,
    const SpectrumTheme& theme) const
{
    QPen gridPen(theme.gridColour);
    gridPen.setWidthF(1.0);
    painter.setPen(gridPen);

    for (int division = 1; division < verticalGridDivisions; ++division) {
        const qreal x = bounds.width()
            * static_cast<qreal>(division)
            / static_cast<qreal>(verticalGridDivisions);
        painter.drawLine(QPointF(x, 0.0), QPointF(x, bounds.height()));
    }

    const double maximumDbfs = m_displaySettings->spectrumOffsetDb();
    const double minimumDbfs = maximumDbfs - m_displaySettings->spectrumRangeDb();
    const int horizontalDivisions =
        std::max(1, static_cast<int>(std::lround((maximumDbfs - minimumDbfs) / dbDivision)));

    for (int division = 1; division < horizontalDivisions; ++division) {
        const qreal y = bounds.height()
            * static_cast<qreal>(division)
            / static_cast<qreal>(horizontalDivisions);
        painter.drawLine(QPointF(0.0, y), QPointF(bounds.width(), y));
    }

    if (!m_displaySettings->showDbScale())
        return;

    painter.setPen(theme.scaleTextColour);
    painter.setFont(QFont(QStringLiteral("Consolas"), 9));

    for (double value = maximumDbfs; value >= minimumDbfs - 0.1; value -= dbDivision) {
        const double normalised = (value - minimumDbfs) / (maximumDbfs - minimumDbfs);
        const qreal y = bounds.height() * (1.0 - normalised);
        const qreal labelY = std::clamp(y + 13.0, 13.0, bounds.height() - 3.0);

        painter.drawText(
            QPointF(8.0, labelY),
            QStringLiteral("%1 dBFS").arg(qRound(value)));
    }
}

void SpectrumView::drawSpectrum(
    QPainter& painter,
    const QRectF& bounds,
    const SpectrumTheme& theme)
{
    if (m_binsDbfs.size() < 2U)
        return;

    SpectrumFrame frame;
    frame.binsDbfs = m_binsDbfs;

    if (m_graphicsEngine.peakHoldEnabled())
        frame.peakBinsDbfs = m_peakBinsDbfs;

    const float maximumDbfs =
        static_cast<float>(m_displaySettings->spectrumOffsetDb());
    const float minimumDbfs = maximumDbfs
        - static_cast<float>(m_displaySettings->spectrumRangeDb());

    const SpectrumRenderGeometry geometry =
        m_graphicsEngine.spectrumRenderer().buildGeometry(
            frame,
            bounds.width(),
            bounds.height(),
            minimumDbfs,
            maximumDbfs);

    if (geometry.empty())
        return;

    if (m_graphicsEngine.spectrumFillEnabled()
        && geometry.fillPolygon.size() >= 3U) {
        const QPainterPath fillPath = pathFromPoints(geometry.fillPolygon);

        if (m_graphicsEngine.spectrumGradientEnabled()) {
            QLinearGradient gradient(0.0, 0.0, 0.0, bounds.height());
            gradient.setStops(theme.fillGradient);
            painter.fillPath(fillPath, QBrush(gradient));
        } else {
            painter.fillPath(fillPath, QBrush(theme.traceColour));
        }
    }

    if (m_graphicsEngine.peakHoldEnabled()
        && !geometry.peakHoldPoints.empty()) {
        QPen peakPen(theme.peakHoldColour);
        peakPen.setWidthF(theme.peakHoldWidth);
        peakPen.setCosmetic(true);
        painter.setPen(peakPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(pathFromPoints(geometry.peakHoldPoints));
    }

    QPen tracePen(theme.traceColour);
    tracePen.setWidthF(theme.traceWidth);
    tracePen.setCosmetic(true);
    tracePen.setJoinStyle(Qt::RoundJoin);
    tracePen.setCapStyle(Qt::RoundCap);

    painter.setPen(tracePen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(pathFromPoints(geometry.tracePoints));
}

void SpectrumView::drawCentreLine(
    QPainter& painter,
    const QRectF& bounds,
    const SpectrumTheme& theme) const
{
    const qreal centreX = bounds.width() * 0.5;

    QPen centrePen(theme.centreLineColour);
    centrePen.setWidthF(theme.centreLineWidth);
    centrePen.setCosmetic(true);
    painter.setPen(centrePen);
    painter.drawLine(QPointF(centreX, 0.0), QPointF(centreX, bounds.height()));

    painter.setPen(theme.scaleTextColour);
    painter.setFont(QFont(QStringLiteral("Consolas"), 9));
    painter.drawText(QPointF(centreX + 6.0, 14.0), QStringLiteral("CENTER"));
}

void SpectrumView::updateAutomaticScale()
{
    if (!m_displaySettings
        || !m_displaySettings->autoScale()
        || m_binsDbfs.size() < 16U) {
        return;
    }

    std::vector<float> sampled;
    const std::size_t step = std::max<std::size_t>(
        1U,
        m_binsDbfs.size() / 512U);

    sampled.reserve((m_binsDbfs.size() / step) + 1U);
    for (std::size_t index = 0; index < m_binsDbfs.size(); index += step) {
        if (std::isfinite(m_binsDbfs[index]))
            sampled.push_back(m_binsDbfs[index]);
    }

    if (sampled.size() < 16U)
        return;

    std::sort(sampled.begin(), sampled.end());

    const std::size_t noiseIndex = static_cast<std::size_t>(
        static_cast<double>(sampled.size() - 1U) * 0.20);
    const std::size_t peakIndex = static_cast<std::size_t>(
        static_cast<double>(sampled.size() - 1U) * 0.995);

    const double noiseFloorDb = sampled[noiseIndex];
    const double peakDb = sampled[peakIndex];

    double targetTopDb = std::ceil((peakDb + 8.0) / 10.0) * 10.0;
    double targetBottomDb = std::floor((noiseFloorDb - 12.0) / 10.0) * 10.0;
    double targetRangeDb = std::ceil((targetTopDb - targetBottomDb) / 10.0) * 10.0;

    targetTopDb = std::clamp(targetTopDb, -100.0, 20.0);
    targetRangeDb = std::clamp(targetRangeDb, 40.0, 140.0);

    if (std::abs(targetTopDb - m_displaySettings->spectrumOffsetDb()) >= 10.0)
        m_displaySettings->setSpectrumOffsetDb(targetTopDb);

    if (std::abs(targetRangeDb - m_displaySettings->spectrumRangeDb()) >= 10.0)
        m_displaySettings->setSpectrumRangeDb(targetRangeDb);
}
}
