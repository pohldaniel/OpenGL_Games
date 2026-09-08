#include "RtAudioPlayer.h"

int RtAudioPlayer::RtAudioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void* userData) {
    return static_cast<RtAudioPlayer*>(userData)->audioCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status);
}

RtAudioPlayer::RtAudioPlayer() {
    m_ringBuffer.init(65536);
}

RtAudioPlayer::~RtAudioPlayer() {
    if (m_dac.isStreamOpen()) {
        m_dac.abortStream();
        m_dac.closeStream();
    }
}

bool RtAudioPlayer::init() {
    if (m_dac.getDeviceCount() < 1) {
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256;
    m_dac.openStream(&parameters, nullptr, RTAUDIO_SINT16, 44100, &bufferFrames, &RtAudioCallback, this);
    return true;
}

void RtAudioPlayer::enqueueData(const std::vector<uint8_t>& pcmData) {

    if (!pcmData.empty()) {
        m_accumulator.insert(m_accumulator.end(), pcmData.begin(), pcmData.end());
    }

    if (m_accumulator.size() < 4096) {
        return;
    }

    size_t availableWrite = m_ringBuffer.getAvailableWrite();

    if (availableWrite > 0 && !m_accumulator.empty()) {
        size_t toWrite = std::min(availableWrite, m_accumulator.size());
        m_ringBuffer.write(m_accumulator.data(), toWrite);
        m_accumulator.erase(m_accumulator.begin(), m_accumulator.begin() + toWrite);
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
    if (m_dac.isStreamRunning())
        m_dac.stopStream();
}

void RtAudioPlayer::resume() {
    if (!m_dac.isStreamRunning())
        m_dac.startStream();
}

void RtAudioPlayer::setVolume(float volume) {
    getMixer().setVolume(volume);
}

float RtAudioPlayer::getVolume() {
    return getMixer().getVolume();
}

void RtAudioPlayer::flush() {   
    if (m_dac.isStreamRunning()) 
        m_dac.abortStream();
       
    m_accumulator.clear();
    m_ringBuffer.clear();
}