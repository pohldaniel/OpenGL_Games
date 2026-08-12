#pragma once
#include <oboe/Oboe.h>
#include "AudioDecoder.h"
#include <vector>
#include <atomic>

// Einfacher, lock-freier Ringbuffer für Single-Producer (Gameloop) und Single-Consumer (Oboe)
class AudioRingBuffer {
public:
    void init(size_t capacity) {
        buffer.resize(capacity);
        head.store(0);
        tail.store(0);
    }

    size_t write(const uint8_t* data, size_t size) {
        size_t h = head.load(std::memory_order_relaxed);
        size_t t = tail.load(std::memory_order_acquire);
        size_t available = buffer.size() - (h - t);
        if (available < size) size = available;

        for (size_t i = 0; i < size; ++i) {
            buffer[(h + i) % buffer.size()] = data[i];
        }
        head.store(h + size, std::memory_order_release);
        return size;
    }

    size_t read(uint8_t* data, size_t size) {
        size_t h = head.load(std::memory_order_acquire);
        size_t t = tail.load(std::memory_order_relaxed);
        size_t available = h - t;
        if (available < size) size = available;

        for (size_t i = 0; i < size; ++i) {
            data[i] = buffer[(t + i) % buffer.size()];
        }
        tail.store(t + size, std::memory_order_release);
        return size;
    }

    size_t getAvailableWrite() const {
        return buffer.size() - (head.load(std::memory_order_relaxed) - tail.load(std::memory_order_relaxed));
    }

private:
    std::vector<uint8_t> buffer;
    std::atomic<size_t> head{0};
    std::atomic<size_t> tail{0};
};

class OboePlayer : public oboe::AudioStreamDataCallback {
public:
    OboePlayer();
    ~OboePlayer();

    bool init();
    void play(const std::string& filename);
    void update(); // Wird weiterhin in deiner Gameloop aufgerufen!

    // Der Oboe-Callback (wird von Androids Audio-Thread aufgerufen)
    oboe::DataCallbackResult onAudioReady(
        oboe::AudioStream *audioStream, 
        void *audioData, 
        int32_t numFrames) override;

private:
    std::shared_ptr<oboe::AudioStream> stream;
    AudioDecoder decoder;
    AudioRingBuffer ringBuffer;
    bool isPlaying = false;
};