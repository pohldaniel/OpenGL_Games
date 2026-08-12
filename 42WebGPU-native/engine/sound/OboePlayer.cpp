#include "OboePlayer.h"
#include <android/log.h>

#define LOG_TAG "OboePlayer"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

OboePlayer::OboePlayer() {
    // 64 KB Buffer bietet genug Reserve für Gameloop-Schwankungen auf Android
    ringBuffer.init(65536); 
}

OboePlayer::~OboePlayer() {
    if (stream) {
        stream->stop();
        stream->close();
    }
}

bool OboePlayer::init() {
    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output)
           ->setPerformanceMode(oboe::PerformanceMode::LowLatency) // Wichtig für Spiele
           ->setSharingMode(oboe::SharingMode::Exclusive)
           ->setFormat(oboe::AudioFormat::I16) // Unser FFmpeg liefert Int16 (S16)
           ->setChannelCount(2)                // Stereo
           ->setSampleRate(44100)              // 44.1 kHz
           ->setCallback(this);                // Reicht den Callback an diese Klasse weiter

    oboe::Result result = builder.openStream(stream);
    if (result != oboe::Result::OK) {
        LOGE("Fehler beim Öffnen des Oboe Streams: %s", oboe::convertToText(result));
        return false;
    }
    return true;
}

void OboePlayer::play(const std::string& filename) {
    if (!decoder.open(filename)) {
        LOGE("FFmpeg konnte Datei nicht öffnen: %s", filename.c_str());
        return;
    }

    // Buffer initial befüllen, bevor der Ton startet
    update();

    stream->start();
    isPlaying = true;
}

// LÄUFT IN DEINER GAMELOOP (Single-Threaded)
void OboePlayer::update() {
    if (!isPlaying) return;

    // Schiebe so lange decodierte Frames in den Ringbuffer, wie Platz ist
    // Wir lassen immer etwas Platz frei, um die Gameloop nicht zu blockieren
    while (ringBuffer.getAvailableWrite() > 16384) {
        std::vector<uint8_t> frameData;
        if (!decoder.decodeFrame(frameData)) {
            // EOF erreicht
            isPlaying = false;
            break;
        }

        if (!frameData.empty()) {
            ringBuffer.write(frameData.data(), frameData.size());
        }
    }
}

// LÄUFT IM OBOE AUDIO-THREAD (High Priority Callback)
oboe::DataCallbackResult OboePlayer::onAudioReady(
    oboe::AudioStream *audioStream, 
    void *audioData, 
    int32_t numFrames) {

    // Ein Frame besteht bei Stereo-I16 aus 2 Kanälen * 2 Bytes = 4 Bytes
    size_t bytesNeeded = numFrames * 4; 
    uint8_t* outputBuffer = static_cast<uint8_t*>(audioData);

    // Daten aus dem Ringbuffer auslesen
    size_t bytesRead = ringBuffer.read(outputBuffer, bytesNeeded);

    // Falls die Gameloop zu langsam war und der Ringbuffer leer ist (Underrun):
    // Den Rest des Buffers mit Stille (Nullen) füllen, sonst knackt es furchtbar
    if (bytesRead < bytesNeeded) {
        std::fill_n(outputBuffer + bytesRead, bytesNeeded - bytesRead, 0);
    }

    return oboe::DataCallbackResult::Continue;
}