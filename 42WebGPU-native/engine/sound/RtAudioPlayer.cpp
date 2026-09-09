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
    m_dac.openStream(&parameters, nullptr, RTAUDIO_FLOAT32, 44100, &bufferFrames, &RtAudioCallback, this);
    return true;
}

void RtAudioPlayer::enqueueData(const std::vector<float>& pcmData) {

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

int RtAudioPlayer::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status) {

    size_t samplesNeeded = nBufferFrames * 2;
    float* out = static_cast<float*>(outputBuffer);
    size_t samplesRead = m_ringBuffer.read(out, samplesNeeded);
    if (samplesRead < samplesNeeded) {
        std::fill_n(out + samplesRead, samplesNeeded - samplesRead, 0.0f);
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
    m_softwareMixer.setVolume(volume);
}

float RtAudioPlayer::getVolume() {
    return m_softwareMixer.getVolume();
}

void RtAudioPlayer::flush() {   
    if (m_dac.isStreamRunning()) 
        m_dac.abortStream();
       
    m_accumulator.clear();
    m_ringBuffer.clear();
}