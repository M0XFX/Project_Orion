#include "IQSourceWorker.h"
#include "Logger.h"

IQSourceWorker::IQSourceWorker(
    HFSDR::IQSource* source,
    HFSDR::IQBlockRingBuffer* ringBuffer,
    QObject* parent
    )
    : QObject(parent),
    m_source(source),
    m_ringBuffer(ringBuffer)
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

    if (!m_ringBuffer) {
        m_running.store(false);
        emit errorOccurred(
            "IQSourceWorker has no IQ ring buffer."
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

    m_ringBuffer->reset();

    HFSDR::Logger::info(
        "IQ source streaming started."
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

        if (!m_ringBuffer->push(block)) {
            if (!m_running.load())
                break;

            // Dropped blocks are counted by the ring buffer.
            // Do not flood the application log here.
        }
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

    if (m_ringBuffer)
        m_ringBuffer->stop();
}