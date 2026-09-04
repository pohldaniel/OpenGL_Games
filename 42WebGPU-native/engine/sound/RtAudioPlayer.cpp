#include <iostream>
#include "RtAudioPlayer.h"

int rtaudio_callback_wrapper(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    return static_cast<RtAudioPlayer*>(userData)->audioCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status);
}

RtAudioPlayer::RtAudioPlayer() {
    m_ringBuffer.init(65536);
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

    return true;
}

void RtAudioPlayer::enqueueData(const std::vector<uint8_t>& pcmData) {

    if (!pcmData.empty()) {
        m_audioAccumulator.insert(m_audioAccumulator.end(), pcmData.begin(), pcmData.end());
    }

    if (m_audioAccumulator.size() < 4096) {
        return;
    }

    size_t availableWrite = m_ringBuffer.getAvailableWrite();

    if (availableWrite > 0 && !m_audioAccumulator.empty()) {
        size_t toWrite = std::min(availableWrite, m_audioAccumulator.size());
        m_ringBuffer.write(m_audioAccumulator.data(), toWrite);
        m_audioAccumulator.erase(m_audioAccumulator.begin(), m_audioAccumulator.begin() + toWrite);
    }

    resume();
}


int RtAudioPlayer::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status) {

    size_t samplesNeeded = nBufferFrames * 2;
    size_t bytesNeeded = samplesNeeded * sizeof(int16_t);
    int16_t* out = static_cast<int16_t*>(outputBuffer);
    size_t bytesRead = m_ringBuffer.read(reinterpret_cast<uint8_t*>(out), bytesNeeded);

    if (bytesRead < bytesNeeded) {
        std::fill_n(reinterpret_cast<uint8_t*>(out) + bytesRead, bytesNeeded - bytesRead, 0);
    }

    m_softwareMixer.mixAudio(out, static_cast<int32_t>(samplesNeeded));

    return 0;
}

SoftwareMixer& RtAudioPlayer::getMixer() {
    return m_softwareMixer;
}

void RtAudioPlayer::pause() {
    if (dac.isStreamRunning())
        dac.stopStream();
}

void RtAudioPlayer::resume() {
    if (!dac.isStreamRunning())
        dac.startStream();
}

void RtAudioPlayer::setVolume(float volume) {
    getMixer().setVolume(volume);
}

float RtAudioPlayer::getVolume() {
    return getMixer().getVolume();
}

void RtAudioPlayer::flush() {
    try {
        if (dac.isStreamRunning()) {
            dac.abortStream();
        }
    }catch (const std::exception& e) {
        std::cerr << "Fehler beim Stoppen von RtAudio: " << e.what() << std::endl;
    }

    m_audioAccumulator.clear();
    m_ringBuffer.clear();
}