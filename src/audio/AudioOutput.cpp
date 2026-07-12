#include "AudioOutput.h"

#include "Logger.h"

#include <QAudioDevice>
#include <QMediaDevices>

#include <algorithm>
#include <cstring>
#include <limits>

namespace HFSDR
{

AudioOutput::AudioOutput(QObject* parent)
    : QObject(parent)
{
}

AudioOutput::~AudioOutput()
{
    stop();
}

bool AudioOutput::start()
{
    if (m_running)
        return true;

    const QAudioDevice device = QMediaDevices::defaultAudioOutput();

    if (device.isNull()) {
        Logger::error("No audio output device is available.");
        return false;
    }

    QAudioFormat format;
    format.setSampleRate(m_sampleRate);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!device.isFormatSupported(format)) {
        Logger::error(
            QString("Audio format not supported: %1 Hz, mono, Int16")
                .arg(m_sampleRate)
        );
        return false;
    }

    m_audioSink = std::make_unique<QAudioSink>(device, format, this);

    // Give the sink enough buffering for several DSP blocks.
    m_audioSink->setBufferSize(m_sampleRate * 2 / 4);

    m_audioDevice = m_audioSink->start();

    if (!m_audioDevice) {
        Logger::error("Failed to start the audio output.");
        m_audioSink.reset();
        return false;
    }

    m_running = true;

    Logger::info(
        QString("Audio output started: %1 Hz, mono, Int16")
            .arg(m_sampleRate)
    );

    return true;
}

void AudioOutput::stop()
{
    if (m_audioSink)
        m_audioSink->stop();

    m_audioDevice = nullptr;
    m_audioSink.reset();
    m_running = false;
}

bool AudioOutput::isRunning() const
{
    return m_running;
}

void AudioOutput::writeSamples(const std::vector<float>& samples)
{
    if (!m_running || !m_audioDevice || samples.empty())
        return;

    std::vector<qint16> pcm(samples.size());

    std::transform(
        samples.begin(),
        samples.end(),
        pcm.begin(),
        [](float sample) {
            return floatToInt16(sample);
        }
    );

    const auto* bytes =
        reinterpret_cast<const char*>(pcm.data());

    const qint64 byteCount =
        static_cast<qint64>(pcm.size() * sizeof(qint16));

    const qint64 written = m_audioDevice->write(bytes, byteCount);

    if (written < 0)
        Logger::warning("Failed to write samples to the audio device.");
}

qint16 AudioOutput::floatToInt16(float sample)
{
    sample = std::clamp(sample, -1.0f, 1.0f);

    return static_cast<qint16>(
        sample * static_cast<float>(
            std::numeric_limits<qint16>::max()
        )
    );
}

} // namespace HFSDR
