#ifndef VFO_H
#define VFO_H

#include <QObject>
#include <QString>

class VFO : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint64 frequencyHz READ frequencyHz WRITE setFrequencyHz NOTIFY frequencyChanged)
    Q_PROPERTY(QString frequencyText READ frequencyText NOTIFY frequencyChanged)

    Q_PROPERTY(quint64 ifFrequencyHz READ ifFrequencyHz WRITE setIfFrequencyHz NOTIFY ifFrequencyChanged)
    Q_PROPERTY(QString ifFrequencyText READ ifFrequencyText NOTIFY ifFrequencyChanged)

    Q_PROPERTY(QString mode READ mode WRITE setMode NOTIFY modeChanged)

public:
    explicit VFO(QObject *parent = nullptr);

    quint64 frequencyHz() const;
    void setFrequencyHz(quint64 frequencyHz);

    QString frequencyText() const;

    quint64 ifFrequencyHz() const;
    void setIfFrequencyHz(quint64 ifFrequencyHz);

    QString ifFrequencyText() const;

    QString mode() const;
    void setMode(const QString &mode);

signals:
    void frequencyChanged();
    void ifFrequencyChanged();
    void modeChanged();

private:
    QString formatFrequency(quint64 frequencyHz) const;

private:
    quint64 m_frequencyHz = 14074000;    // Display/RF frequency
    quint64 m_ifFrequencyHz = 60000000;  // RTL-SDR centre frequency
    QString m_mode = "USB";
};

#endif