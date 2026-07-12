#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>

#include <memory>
#include <vector>

namespace HFSDR
{

class AudioOutput : public QObject
{
    Q_OBJECT

public:
    explicit AudioOutput(QObject* parent = nullptr);
    ~AudioOutput() override;

    bool start();
    void stop();

    bool isRunning() const;

    void writeSamples(const std::vector<float>& samples);

private:
    static qint16 floatToInt16(float sample);

    std::unique_ptr<QAudioSink> m_audioSink;
    QIODevice* m_audioDevice = nullptr;

    bool m_running = false;
    int m_sampleRate = 32000;
};

} // namespace HFSDR

#endif
