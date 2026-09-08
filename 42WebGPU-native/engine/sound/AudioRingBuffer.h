#pragma once

#include <vector>
#include <atomic>
#include <algorithm>
#include <cstring>

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

    size_t write(const float* data, size_t sampleCount) {
        size_t h = m_head.load(std::memory_order_relaxed);
        size_t t = m_tail.load(std::memory_order_acquire);

        size_t available = (h >= t) ? (m_buffer.size() - 1 - (h - t)) : (t - h - 1);
        if (sampleCount > available) sampleCount = available;

        size_t firstPart = std::min(sampleCount, m_buffer.size() - h);

        std::memcpy(&m_buffer[h], data, firstPart * sizeof(float));
        std::memcpy(&m_buffer[0], data + firstPart, (sampleCount - firstPart) * sizeof(float));

        m_head.store((h + sampleCount) % m_buffer.size(), std::memory_order_release);
        return sampleCount;
    }

    size_t read(float* data, size_t sampleCount) {
        size_t h = m_head.load(std::memory_order_acquire);
        size_t t = m_tail.load(std::memory_order_relaxed);

        size_t available = (h >= t) ? (h - t) : (m_buffer.size() - (t - h));
        if (sampleCount > available) sampleCount = available;

        size_t firstPart = std::min(sampleCount, m_buffer.size() - t);

        std::memcpy(data, &m_buffer[t], firstPart * sizeof(float));
        std::memcpy(data + firstPart, &m_buffer[0], (sampleCount - firstPart) * sizeof(float));

        m_tail.store((t + sampleCount) % m_buffer.size(), std::memory_order_release);
        return sampleCount;
    }

    void clear() {
        m_head.store(0, std::memory_order_seq_cst);
        m_tail.store(0, std::memory_order_seq_cst);
    }

private:
    std::vector<float> m_buffer;
    std::atomic<size_t> m_head{ 0 };
    std::atomic<size_t> m_tail{ 0 };
};