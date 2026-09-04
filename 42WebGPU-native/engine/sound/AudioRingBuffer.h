#pragma once

class AudioRingBuffer {

public:
    void init(size_t capacity) {
        m_buffer.resize(capacity + 1);
        m_head.store(0, std::memory_order_relaxed);
        m_tail.store(0, std::memory_order_relaxed);
    }

    size_t getAvailableWrite() const {
        size_t h = m_head.load(std::memory_order_relaxed);
        size_t t = m_tail.load(std::memory_order_relaxed);
        if (h >= t) {
            return m_buffer.size() - 1 - (h - t);
        }
        return t - h - 1;
    }

    size_t getAvailableRead() const {
        size_t h = m_head.load(std::memory_order_acquire);
        size_t t = m_tail.load(std::memory_order_relaxed);
        if (h >= t) {
            return h - t;
        }
        return m_buffer.size() - (t - h);
    }

    size_t write(const uint8_t* data, size_t size) {
        size_t h = m_head.load(std::memory_order_relaxed);
        size_t t = m_tail.load(std::memory_order_acquire);

        size_t available = (h >= t) ? (m_buffer.size() - 1 - (h - t)) : (t - h - 1);
        if (size > available) size = available;

        size_t firstPart = std::min(size, m_buffer.size() - h);
        std::memcpy(&m_buffer[h], data, firstPart);
        std::memcpy(&m_buffer[0], data + firstPart, size - firstPart);

        m_head.store((h + size) % m_buffer.size(), std::memory_order_release);
        return size;
    }

    size_t read(uint8_t* data, size_t size) {
        size_t h = m_head.load(std::memory_order_acquire);
        size_t t = m_tail.load(std::memory_order_relaxed);

        size_t available = (h >= t) ? (h - t) : (m_buffer.size() - (t - h));
        if (size > available) size = available;

        size_t firstPart = std::min(size, m_buffer.size() - t);
        std::memcpy(data, &m_buffer[t], firstPart);
        std::memcpy(data + firstPart, &m_buffer[0], size - firstPart);

        m_tail.store((t + size) % m_buffer.size(), std::memory_order_release);
        return size;
    }

    void clear() {
        m_head.store(0, std::memory_order_seq_cst);
        m_tail.store(0, std::memory_order_seq_cst);
    }

private:
    std::vector<uint8_t> m_buffer;
    std::atomic<size_t> m_head{ 0 };
    std::atomic<size_t> m_tail{ 0 };
};