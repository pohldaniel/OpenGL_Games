#include "OboeAudioPlayer.h"
#include <android/log.h>
#include <algorithm>

#define LOG_TAG "OboeAudioPlayer"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

OboeAudioPlayer::OboeAudioPlayer() {
    ringBuffer.init(65536); // 64 KB bietet optimale Reserve auf Mobilgeräten
}

OboeAudioPlayer::~OboeAudioPlayer() {
    if (stream) {
        stream->stop();
        stream->close();
    }
}

bool OboeAudioPlayer::init() {
    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output)
           ->setPerformanceMode(oboe::PerformanceMode::LowLatency) // Erzwingt geringste Latenz für Spiele
           ->setSharingMode(oboe::SharingMode::Exclusive)         // Exklusiver Hardwarezugriff falls verfügbar
           ->setFormat(oboe::AudioFormat::I16)                     // Wir füttern 16-Bit Signed Integer (PCM S16)
           ->setChannelCount(2)                                    // Stereo-Ausgabe
           ->setSampleRate(44100)                                  // 44.1 kHz Standard
           ->setCallback(this);

    oboe::Result result = builder.openStream(stream);
    if (result != oboe::Result::OK) {
        LOGE("Fehler beim Öffnen des Oboe Streams: %s", oboe::convertToText(result));
        return false;
    }
    return true;
}

void OboeAudioPlayer::playMusic(const std::string& filename) {
    if (!decoder.open(filename)) {
        LOGE("FFmpeg konnte Musikdatei nicht öffnen: %s", filename.c_str());
        return;
    }

    update(); // Vorab-Befüllung des Ringbuffers (verhindert Start-Underrun)
    
    oboe::Result result = stream->start();
    if (result == oboe::Result::OK) {
        m_isMusicPlaying = true;
    } else {
        LOGE("Fehler beim Starten des Oboe Streams: %s", oboe::convertToText(result));
    }
}

// LÄUFT IN DEINER ANDROID-GAMELOOP (Single-Threaded)
void OboeAudioPlayer::update() {
    if (!m_isMusicPlaying) return;

    // FFmpeg decodiert blockweise PCM-Daten nach, solange Platz im Ringbuffer ist
    while (ringBuffer.getAvailableWrite() > 16384) {
        std::vector<uint8_t> frameData;
        if (!decoder.decodeFrame(frameData)) {
            m_isMusicPlaying = false; // EOF erreicht
            break;
        }

        if (!frameData.empty()) {
            ringBuffer.write(frameData.data(), frameData.size());
        }
    }
}

// LÄUFT IM NATIVEN OBOE AUDIO-THREAD (High Priority Callback)
oboe::DataCallbackResult OboeAudioPlayer::onAudioReady(
    oboe::AudioStream *audioStream, 
    void *audioData, 
    int32_t numFrames) {

    // numFrames * 2 Kanäle = Gesamte Einzelsamples, die Oboe abholen will
    int32_t samplesNeeded = numFrames * 2; 
    size_t bytesNeeded = samplesNeeded * sizeof(int16_t);
    
    int16_t* out = static_cast<int16_t*>(audioData);

    // 1. Hintergrundmusik aus dem lock-freien Ringbuffer lesen
    size_t bytesRead = ringBuffer.read(reinterpret_cast<uint8_t*>(out), bytesNeeded);

    // Schutz vor Rucklern (Underruns): Falls die Gameloop hing, mit Stille füllen
    if (bytesRead < bytesNeeded) {
        std::fill_n(reinterpret_cast<uint8_t*>(out) + bytesRead, bytesNeeded - bytesRead, 0);
    }

    // 2. Jetzt alle aktiven, lock-freien Waffensounds mit Filtern, Pitch und Gain direkt dazumischen!
    m_mixer.mixAudio(out, samplesNeeded);

    return oboe::DataCallbackResult::Continue;
}