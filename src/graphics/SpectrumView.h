#ifndef SPECTRUMVIEW_H
#define SPECTRUMVIEW_H

#include "GraphicsEngine.h"

#include <QPointer>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include <vector>

class DisplaySettings;
class Receiver;

namespace HFSDR::Graphics
{
class SpectrumView : public QQuickPaintedItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SpectrumView)

    Q_PROPERTY(QObject* receiver READ receiver WRITE setReceiver NOTIFY receiverChanged)
    Q_PROPERTY(QObject* displaySettings READ displaySettings WRITE setDisplaySettings NOTIFY displaySettingsChanged)

public:
    explicit SpectrumView(QQuickItem* parent = nullptr);

    QObject* receiver() const noexcept;
    void setReceiver(QObject* receiverObject);

    QObject* displaySettings() const noexcept;
    void setDisplaySettings(QObject* settingsObject);

    void paint(QPainter* painter) override;

signals:
    void receiverChanged();
    void displaySettingsChanged();

private slots:
    void refreshSpectrum();
    void refreshDisplaySettings();
    void updateAutomaticScale();

private:
    void disconnectReceiver();
    void disconnectDisplaySettings();
    void updatePeakHold();
    void drawGrid(QPainter& painter, const QRectF& bounds, const SpectrumTheme& theme) const;
    void drawSpectrum(QPainter& painter, const QRectF& bounds, const SpectrumTheme& theme);
    void drawCentreLine(QPainter& painter, const QRectF& bounds, const SpectrumTheme& theme) const;

    QPointer<Receiver> m_receiver;
    QPointer<DisplaySettings> m_displaySettings;

    std::vector<float> m_binsDbfs;
    std::vector<float> m_peakBinsDbfs;

    GraphicsEngine m_graphicsEngine;
    QTimer m_autoScaleTimer;
};
}

#endif
