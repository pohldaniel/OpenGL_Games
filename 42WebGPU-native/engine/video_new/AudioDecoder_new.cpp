#include "AudioDecoder_new.h"

AudioDecoderNew::AudioDecoderNew() : m_audioFrame(nullptr){
    m_audioFrame = av_frame_alloc();
}

AudioDecoderNew::~AudioDecoderNew() {
    if (m_audioFrame) 
        av_frame_free(&m_audioFrame);
}

bool AudioDecoderNew::decodeAudioFrame(AVCodecContext* audioCtx, SwrContext* swrCtx, AVPacket* packet, std::vector<uint8_t>& outPcmData) {
    
    if(avcodec_send_packet(audioCtx, packet) < 0) 
        return false;

    
    if(avcodec_receive_frame(audioCtx, m_audioFrame) < 0)    
        return false;
  
    int outSamples = swr_get_out_samples(swrCtx, m_audioFrame->nb_samples);

    outPcmData.resize(outSamples * 2 * sizeof(int16_t));
    uint8_t* outputBuffer = outPcmData.data();

    int translated = swr_convert(swrCtx, &outputBuffer, outSamples,
        (const uint8_t**)m_audioFrame->data, m_audioFrame->nb_samples);

    if (translated < 0) 
        return false;

    outPcmData.resize(translated * 2 * sizeof(int16_t));

    av_frame_unref(m_audioFrame);
    return true;
}