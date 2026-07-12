#ifndef IQBLOCKRINGBUFFER_H
#define IQBLOCKRINGBUFFER_H

#include "IQBuffer.h"

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <vector>

namespace HFSDR
{

class IQBlockRingBuffer
{
public:
    explicit IQBlockRingBuffer(std::size_t blockCapacity = 64);

    bool push(const IQBuffer& block);
    bool pop(IQBuffer& block);

    bool waitAndPop(IQBuffer& block);

    void stop();
    void reset();
    void clear();

    bool empty() const;
    bool full() const;

    std::size_t size() const;
    std::size_t capacity() const;

    std::size_t droppedBlockCount() const;

private:
    std::vector<IQBuffer> m_blocks;

    std::size_t m_readIndex = 0;
    std::size_t m_writeIndex = 0;
    std::size_t m_blockCount = 0;
    std::size_t m_droppedBlockCount = 0;

    bool m_stopping = false;

    mutable std::mutex m_mutex;
    std::condition_variable m_dataAvailable;
};

} // namespace HFSDR

#endif
