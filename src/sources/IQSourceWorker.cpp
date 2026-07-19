#include "IQSourceWorker.h"
#include "Logger.h"

IQSourceWorker::IQSourceWorker(
    HFSDR::IQSource* source,
    HFSDR::IQBlockRingBuffer* receiverBuffer,
    HFSDR::IQBlockRingBuffer* spectrumBuffer,
    QObject* parent)
    : QObject(parent),
      m_source(source),
      m_receiverBuffer(receiverBuffer),
      m_spectrumBuffer(spectrumBuffer)
{
}

bool IQSourceWorker::running() const
{
    return m_running.load();
}

void IQSourceWorker::start()
{
    if (m_running.exchange(true))
        return;

    if (!m_source) {
        m_running.store(false);
        emit errorOccurred(
            "IQSourceWorker has no IQ source."
            );
        return;
    }

    if (!m_receiverBuffer || !m_spectrumBuffer) {
        m_running.store(false);
        emit errorOccurred(
            "IQSourceWorker does not have both IQ output buffers."
            );
        return;
    }

    if (!m_source->connected()) {
        m_running.store(false);
        emit errorOccurred(
            "The selected IQ source is not connected."
            );
        return;
    }

    m_receiverBuffer->reset();
    m_spectrumBuffer->reset();

    HFSDR::Logger::info(
        "IQ source streaming started with independent receiver and spectrum paths."
        );

    emit started();

    HFSDR::IQBuffer block(m_blockSize);

    while (m_running.load()) {
        if (!m_source->readSamples(block)) {
            if (m_running.load()) {
                HFSDR::Logger::warning(
                    "IQSourceWorker failed to read an IQ block."
                    );

                emit errorOccurred(
                    "The IQ source failed to provide samples."
                    );
            }

            break;
        }

        // Each hardware block is read once and then copied into two
        // independent queues. A full spectrum queue may drop a display
        // block without delaying the receiver/audio path.
        m_receiverBuffer->push(block);
        m_spectrumBuffer->push(block);
    }

    m_running.store(false);

    HFSDR::Logger::info(
        "IQ source streaming stopped."
        );

    emit stopped();
}

void IQSourceWorker::stop()
{
    if (!m_running.exchange(false))
        return;

    if (m_receiverBuffer)
        m_receiverBuffer->stop();

    if (m_spectrumBuffer)
        m_spectrumBuffer->stop();
}
