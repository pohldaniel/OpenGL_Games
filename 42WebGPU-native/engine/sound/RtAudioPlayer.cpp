#include "RtAudioPlayer.h"
#include <iostream>

// Globaler oder statischer Trampolin-Callback, da RtAudio einen C-Funktionszeiger braucht
int rtaudio_callback_wrapper(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    return static_cast<RtAudioPlayer*>(userData)->audioCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status);
}

RtAudioPlayer::RtAudioPlayer() {
    m_ringBuffer.init(65536); // Gleiche Buffer-Größe wie bei Android
}

RtAudioPlayer::~RtAudioPlayer() {
    if (dac.isStreamOpen()) dac.closeStream();
}

bool RtAudioPlayer::init() {
    if (dac.getDeviceCount() < 1) {
        std::cerr << "Keine Soundkarte gefunden!" << std::endl;
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2; // Stereo
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256; // Entspricht numFrames bei Oboe (Low Latency)

    try {
        dac.openStream(&parameters, nullptr, RTAUDIO_SINT16, 44100, &bufferFrames, &rtaudio_callback_wrapper, this);
    }
    catch (const std::exception& e) {
        std::cerr << "RtAudio Fehler: " << e.what() << std::endl;
        return false;
    }

    startHardwareStream();
    return true;
}

void RtAudioPlayer::play(const std::string& filename) {
    if (!decoder.open(filename)) return;

    update(); // Vorab-Befüllung des Ringbuffers (wichtig!)

    try {
        dac.startStream();
        isPlaying = true;
    }
    catch (const std::exception& e) {
        std::cerr << "Fehler beim Starten des Audio-Streams: " << e.what() << std::endl;
    }
}

void RtAudioPlayer::startHardwareStream() {
    // Falls der Stream bereits geöffnet, aber noch gestoppt ist, schalten wir ihn ein
    if (dac.isStreamOpen() && !dac.isStreamRunning()) {
        try {
            dac.startStream();
            std::cout << "RtAudio Hardware-Stream erfolgreich gestartet!" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Fehler beim Starten des RtAudio Streams: " << e.what() << std::endl;
        }
    }
}

// 1:1 identisch zur Oboe-Logik für deine Windows-Gameloop
void RtAudioPlayer::update() {
    if (!isPlaying) return;

    while (m_ringBuffer.getAvailableWrite() > 16384) {
        std::vector<uint8_t> frameData;
        if (!decoder.decodeFrame(frameData)) {
            isPlaying = false;
            break;
        }
        if (!frameData.empty()) {
            m_ringBuffer.write(frameData.data(), frameData.size());
        }
    }
}

void RtAudioPlayer::enqueueData(const std::vector<uint8_t>& pcmData) {
    if (m_ringBuffer.getAvailableWrite() >= pcmData.size()) {
        m_ringBuffer.write(pcmData.data(), pcmData.size());
    }
}

// Verhält sich EXAKT wie Oboes onAudioReady
int RtAudioPlayer::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status) {

    size_t samplesNeeded = nBufferFrames * 2;
    size_t bytesNeeded = samplesNeeded * sizeof(int16_t);

    int16_t* out = static_cast<int16_t*>(outputBuffer);

    // 1. Musik aus dem Ringbuffer holen
    size_t bytesRead = m_ringBuffer.read(reinterpret_cast<uint8_t*>(out), bytesNeeded);

    // Bei Lag oder Ende mit Stille füllen
    if (bytesRead < bytesNeeded) {
        std::fill_n(reinterpret_cast<uint8_t*>(out) + bytesRead, bytesNeeded - bytesRead, 0);
    }

    // 2. Jetzt die MG-Salven lock-frei dazumischen
    // Hier übergeben wir die exakte Anzahl an SAMPLES (nicht Frames!)
    m_softwareMixer.mixAudio(out, static_cast<int32_t>(samplesNeeded));

    return 0;
}

SoftwareMixer& RtAudioPlayer::getMixer() {
    return m_softwareMixer;
}

AudioRingBuffer& RtAudioPlayer::getRingBuffer() {
    return m_ringBuffer;
}