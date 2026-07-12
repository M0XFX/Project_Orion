#ifndef IQSOURCEWORKER_H
#define IQSOURCEWORKER_H

#include <QObject>

#include <atomic>
#include <cstddef>

#include "IQBlockRingBuffer.h"
#include "IQSource.h"

class IQSourceWorker : public QObject
{
    Q_OBJECT

public:
    explicit IQSourceWorker(
        HFSDR::IQSource* source,
        HFSDR::IQBlockRingBuffer* ringBuffer,
        QObject* parent = nullptr
        );

    bool running() const;

public slots:
    void start();
    void stop();

signals:
    void started();
    void stopped();
    void errorOccurred(const QString& message);

private:
    HFSDR::IQSource* m_source = nullptr;
    HFSDR::IQBlockRingBuffer* m_ringBuffer = nullptr;

    std::atomic_bool m_running = false;

    std::size_t m_blockSize = 2048;
};

#endif