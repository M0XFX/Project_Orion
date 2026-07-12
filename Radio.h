#ifndef RADIO_H
#define RADIO_H

#include <QObject>

class Radio : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)

public:
    explicit Radio(QObject *parent = nullptr);

    QString frequency() const;
    void setFrequency(const QString &frequency);

    QString mode() const;
    void setMode(const QString &mode);

signals:
    void frequencyChanged();
    void modeChanged();

private:
    QString m_frequency = "14.074.000 MHz";
    QString m_mode = "USB";
};

#endif
