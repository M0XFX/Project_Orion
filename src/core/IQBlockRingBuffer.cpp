#include "IQBlockRingBuffer.h"

#include <utility>

namespace HFSDR
{

IQBlockRingBuffer::IQBlockRingBuffer(
    std::size_t blockCapacity)
{
    if (blockCapacity == 0)
        blockCapacity = 1;

    m_blocks.resize(blockCapacity);
}

bool IQBlockRingBuffer::push(
    const IQBuffer& block)
{
    if (block.empty())
        return false;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_stopping)
            return false;

        if (m_blockCount == m_blocks.size()) {
            ++m_droppedBlockCount;
            return false;
        }

        m_blocks[m_writeIndex] = block;

        m_writeIndex =
            (m_writeIndex + 1) % m_blocks.size();

        ++m_blockCount;
    }

    m_dataAvailable.notify_one();
    return true;
}

bool IQBlockRingBuffer::pop(
    IQBuffer& block)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_blockCount == 0)
        return false;

    block = std::move(m_blocks[m_readIndex]);

    m_readIndex =
        (m_readIndex + 1) % m_blocks.size();

    --m_blockCount;

    return true;
}

bool IQBlockRingBuffer::waitAndPop(
    IQBuffer& block)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_dataAvailable.wait(
        lock,
        [this]() {
            return m_stopping || m_blockCount > 0;
        }
        );

    if (m_stopping && m_blockCount == 0)
        return false;

    block = std::move(m_blocks[m_readIndex]);

    m_readIndex =
        (m_readIndex + 1) % m_blocks.size();

    --m_blockCount;

    return true;
}

void IQBlockRingBuffer::stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopping = true;
    }

    m_dataAvailable.notify_all();
}

void IQBlockRingBuffer::reset()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_readIndex = 0;
        m_writeIndex = 0;
        m_blockCount = 0;
        m_droppedBlockCount = 0;
        m_stopping = false;
    }

    m_dataAvailable.notify_all();
}

void IQBlockRingBuffer::clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_readIndex = 0;
    m_writeIndex = 0;
    m_blockCount = 0;
}

bool IQBlockRingBuffer::empty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blockCount == 0;
}

bool IQBlockRingBuffer::full() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blockCount == m_blocks.size();
}

std::size_t IQBlockRingBuffer::size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blockCount;
}

std::size_t IQBlockRingBuffer::capacity() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_blocks.size();
}

std::size_t IQBlockRingBuffer::droppedBlockCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_droppedBlockCount;
}

} // namespace HFSDR
