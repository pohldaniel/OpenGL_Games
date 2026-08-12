#include <iostream>
#include <algorithm>

#include "MediaDecoder.h"
#include "../sound/AudioRingBuffer.h"

MediaDecoder::MediaDecoder() {
    m_packet = av_packet_alloc();
    m_videoFrame = av_frame_alloc();
    m_frameRgba = av_frame_alloc();
    m_audioFrame = av_frame_alloc();
}

MediaDecoder::~MediaDecoder() {
    close();
    av_packet_free(&m_packet);
    av_frame_free(&m_videoFrame);
    av_frame_free(&m_frameRgba);
    av_frame_free(&m_audioFrame);
}

bool MediaDecoder::open(const std::string& filename) {
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0) return false;
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) return false;

    // 1. STREAMS SUCKEN (Video und Audio parallel)
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && m_videoStreamIndex == -1) {
            m_videoStreamIndex = i;
        } else if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && m_audioStreamIndex == -1) {
            m_audioStreamIndex = i;
        }
    }

    // Mindestens eine Videospur ist Pflicht
    if (m_videoStreamIndex == -1) return false;

    // 2. VIDEO-CODEC CONFIG
    const AVCodec* videoCodec = avcodec_find_decoder(m_formatContext->streams[m_videoStreamIndex]->codecpar->codec_id);
    m_videoCodecContext = avcodec_alloc_context3(videoCodec);
    avcodec_parameters_to_context(m_videoCodecContext, m_formatContext->streams[m_videoStreamIndex]->codecpar);
    if (avcodec_open2(m_videoCodecContext, videoCodec, nullptr) < 0) return false;

    m_width = m_videoCodecContext->width;
    m_height = m_videoCodecContext->height;
    AVRational streamFps = m_formatContext->streams[m_videoStreamIndex]->r_frame_rate;
    m_fps = (streamFps.den > 0) ? av_q2d(streamFps) : 30.0;
    m_timePerFrame = 1.0 / m_fps;

    //int yuvSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_width, m_height, 1);
    //m_currentFramePixels.resize(yuvSize, 0);

    // Internen RGBA-Pixelbuffer allokieren
    int rgbaSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_width, m_height, 1);
    m_rgbaBufferInternal = (uint8_t*)av_malloc(rgbaSize * sizeof(uint8_t));
    m_currentFramePixels.resize(rgbaSize, 0);
    av_image_fill_arrays(m_frameRgba->data, m_frameRgba->linesize, m_rgbaBufferInternal, AV_PIX_FMT_RGBA, m_width, m_height, 1);

    m_swsContext = sws_getContext(m_width, m_height, m_videoCodecContext->pix_fmt, m_width, m_height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);

    // 3. AUDIO-CODEC CONFIG (Optional, falls Film Sound hat)
    if (m_audioStreamIndex != -1) {
        const AVCodec* audioCodec = avcodec_find_decoder(m_formatContext->streams[m_audioStreamIndex]->codecpar->codec_id);
        if (audioCodec) {
            m_audioCodecContext = avcodec_alloc_context3(audioCodec);
            avcodec_parameters_to_context(m_audioCodecContext, m_formatContext->streams[m_audioStreamIndex]->codecpar);
            if (avcodec_open2(m_audioCodecContext, audioCodec, nullptr) >= 0) {
                
                // Resampler auf Standard Stereo i16 44.1kHz fixieren
                m_swrContext = swr_alloc();
                av_opt_set_chlayout(m_swrContext, "in_chlayout", &m_audioCodecContext->ch_layout, 0);
                av_opt_set_int(m_swrContext, "in_sample_rate", m_audioCodecContext->sample_rate, 0);
                av_opt_set_sample_fmt(m_swrContext, "in_sample_fmt", m_audioCodecContext->sample_fmt, 0);

                AVChannelLayout outLayout;
                av_channel_layout_default(&outLayout, 2); // Stereo
                av_opt_set_chlayout(m_swrContext, "out_chlayout", &outLayout, 0);
                av_opt_set_int(m_swrContext, "out_sample_rate", 44100, 0);
                av_opt_set_sample_fmt(m_swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
                swr_init(m_swrContext);
            }
        }
    }

    // Erstes Bild laden, damit wir direkt Pixel haben
    decodeVideoFrame();

    // Gesamtdauer der Datei in Sekunden auslesen
    if (m_formatContext->duration != AV_NOPTS_VALUE) {
        m_duration = static_cast<double>(m_formatContext->duration) / AV_TIME_BASE;
    }

    // Zeitbasis des Videos sichern (wichtig für präzises Seeking)
    AVStream* videoStream = m_formatContext->streams[m_videoStreamIndex];
    m_videoTimebase = av_q2d(videoStream->time_base);

    // ... (restlicher bestehender open-Code) ...
    m_isPaused = false;
    m_currentTime = 0.0;
    return true;

    return true;
}

bool MediaDecoder::update(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, AudioRingBuffer& targetBuffer) {
    m_accumulator += deltaTime;
    bool newFrameUploaded = false;
    if (m_isPaused) {
        outRgbaBuffer = m_currentFramePixels;
        return false;
    }
    // --- DIE KORREKTE WEICHE ---
    // Wir lesen so lange ununterbrochen Pakete aus der Datei, 
    // bis wir das für diesen Frame fällige Video-Bild ERFOLGREICH aus dem Decoder extrahiert haben.
    // Das bricht den Teufelskreis aus EAGAIN und vollem Audiobuffer!
    while (m_accumulator >= m_timePerFrame && !newFrameUploaded) {

        // Versuche, das nächste Paket aus der Datei zu lesen
        if (av_read_frame(m_formatContext, m_packet) < 0) {
            av_seek_frame(m_formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
            // Den internen Codec-Speicher leeren, um Bildartefakte beim Sprung zu verhindern
            if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
            if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
            continue; // Schleife direkt mit dem ersten Paket von vorne fortsetzen
        }

        // FALL A: Es ist ein VIDEO-Paket
        if (m_packet->stream_index == m_videoStreamIndex) {
            // JEDES Paket wandert sofort in den Codec (kein POC Abreißen!)
            avcodec_send_packet(m_videoCodecContext, m_packet);

            // Direkt versuchen, das Bild abzurufen
            if (decodeVideoFrame()) {
                newFrameUploaded = true;
                m_accumulator -= m_timePerFrame; // Ein Frame-Zeitfenster abziehen
            }
        }
        // FALL B: Es ist ein AUDIO-Paket
        else if (m_packet->stream_index == m_audioStreamIndex && m_swrContext) {
            std::vector<uint8_t> pcmData;
            if (decodeAudioFrame(pcmData)) {
                // Nur schreiben, wenn Platz ist, um Überläufe im RAM zu verhindern
                if (targetBuffer.getAvailableWrite() >= pcmData.size()) {
                    targetBuffer.write(pcmData.data(), pcmData.size());
                }
            }
        }

        av_packet_unref(m_packet);
    }

    // Extrem wichtiger Schutz vor unendlichem Frame-Drop (z.B. nach harten Rucklern/Ladebildschirmen):
    // Wenn der Akkumulator weiter als 2 Frames in der Zukunft liegt, bremsen wir ihn ein,
    // sonst würde die Schleife versuchen, 100 Frames im selben Durchlauf zu decodieren.
    if (m_accumulator > m_timePerFrame * 2.0) {
        m_accumulator = m_timePerFrame;
    }

    outRgbaBuffer = m_currentFramePixels;
    return newFrameUploaded;
}

bool MediaDecoder::decodeVideoFrame() {
    int response = avcodec_receive_frame(m_videoCodecContext, m_videoFrame);

    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
        return false;
    }
    else if (response < 0) {
        return false; // Echter Fehler
    }

    if (m_videoFrame->pts != AV_NOPTS_VALUE) {
        m_currentTime = m_videoFrame->pts * m_videoTimebase;
    }
    else if (m_videoFrame->pkt_dts != AV_NOPTS_VALUE) {
        // Fallback, falls PTS fehlt, nehmen wir den Decoding-Zeitstempel
        m_currentTime = m_videoFrame->pkt_dts * m_videoTimebase;
    }

    // --- ERFOLG: Frame erhalten! ---
    sws_scale(m_swsContext, (uint8_t const* const*)m_videoFrame->data, m_videoFrame->linesize,
        0, m_height, m_frameRgba->data, m_frameRgba->linesize);

    std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_currentFramePixels.size(),
        m_currentFramePixels.begin());

    return true;
}

bool MediaDecoder::decodeAudioFrame(std::vector<uint8_t>& outPcmData) {
    outPcmData.clear();
    if (avcodec_send_packet(m_audioCodecContext, m_packet) >= 0) {
        if (avcodec_receive_frame(m_audioCodecContext, m_audioFrame) >= 0) {
            
            int maxOutSamples = swr_get_out_samples(m_swrContext, m_audioFrame->nb_samples);
            std::vector<uint8_t> tempBuf(maxOutSamples * 2 * sizeof(int16_t)); // 2 Kanäle, 16-Bit

            uint8_t* outData[1] = { tempBuf.data() };
            int converted = swr_convert(m_swrContext, outData, maxOutSamples, (const uint8_t**)m_audioFrame->data, m_audioFrame->nb_samples);
            
            if (converted > 0) {
                size_t actualSize = converted * 2 * sizeof(int16_t);
                outPcmData.insert(outPcmData.end(), tempBuf.begin(), tempBuf.begin() + actualSize);
                return true;
            }
        }
    }
    return false;
}

void MediaDecoder::close() {
    if (m_swrContext) { swr_free(&m_swrContext); m_swrContext = nullptr; }
    if (m_swsContext) { sws_freeContext(m_swsContext); m_swsContext = nullptr; }
    if (m_rgbaBufferInternal) { av_free(m_rgbaBufferInternal); m_rgbaBufferInternal = nullptr; }
    if (m_videoCodecContext) { avcodec_free_context(&m_videoCodecContext); m_videoCodecContext = nullptr; }
    if (m_audioCodecContext) { avcodec_free_context(&m_audioCodecContext); m_audioCodecContext = nullptr; }
    if (m_formatContext) { avformat_close_input(&m_formatContext); m_formatContext = nullptr; }
}

bool MediaDecoder::updateOpenAL(double deltaTime, std::vector<uint8_t>& outRgbaBuffer, std::vector<uint8_t>& outPcmAudio) {
    m_accumulator += deltaTime;
    bool newFrameUploaded = false;
    outPcmAudio.clear(); // Alten Audio-Frame-Rest leeren

    if (m_isPaused) {
        outRgbaBuffer = m_currentFramePixels;
        return false;
    }

    // Wir lesen Pakete, solange die Zeit für das Video es verlangt
    while (m_accumulator >= m_timePerFrame && !newFrameUploaded) {

        if (av_read_frame(m_formatContext, m_packet) < 0) {
            av_seek_frame(m_formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
            // Den internen Codec-Speicher leeren, um Bildartefakte beim Sprung zu verhindern
            if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
            if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);
            continue; // Schleife direkt mit dem ersten Paket von vorne fortsetzen
        }

        // VIDEO-PAKET
        if (m_packet->stream_index == m_videoStreamIndex) {
            avcodec_send_packet(m_videoCodecContext, m_packet);

            if (decodeVideoFrame()) {
                newFrameUploaded = true;
                m_accumulator -= m_timePerFrame;
            }
        }
        // AUDIO-PAKET: Wir sammeln den Ton dieses Frames direkt im Ausgabe-Vektor
        else if (m_packet->stream_index == m_audioStreamIndex && m_swrContext) {
            std::vector<uint8_t> pcmData;
            if (decodeAudioFrame(pcmData)) {
                // Audio-Daten für diesen Gameloop-Frame anhäufen
                outPcmAudio.insert(outPcmAudio.end(), pcmData.begin(), pcmData.end());
            }
        }

        av_packet_unref(m_packet);
    }

    if (m_accumulator > m_timePerFrame * 2.0) {
        m_accumulator = m_timePerFrame;
    }

    outRgbaBuffer = m_currentFramePixels;
    return newFrameUploaded;
}

void MediaDecoder::seekTo(double seconds) {
    if (!m_formatContext) return;

    m_currentTime = std::clamp(seconds, 0.0, m_duration);

    // 1. Berechne die Ziel-PTS für das Video
    int64_t targetVideoPts = static_cast<int64_t>(m_currentTime / m_videoTimebase);

    // 2. Springe in der Datei zum Keyframe davor
    av_seek_frame(m_formatContext, m_videoStreamIndex, targetVideoPts, AVSEEK_FLAG_BACKWARD);

    // Puffer leeren, damit keine alten Reste decodiert werden
    if (m_videoCodecContext) avcodec_flush_buffers(m_videoCodecContext);
    if (m_audioCodecContext) avcodec_flush_buffers(m_audioCodecContext);

    // 3. --- SMART SEEKING LOOP ---
    // Wir lesen die Datei im "Zeitraffer" intern so lange voraus,
    // bis wir exakt an der vom User gewünschten Sekunde angekommen sind.
    // Das eliminiert das Zurückspringen und Ruckeln komplett!
    while (av_read_frame(m_formatContext, m_packet) >= 0) {

        if (m_packet->stream_index == m_videoStreamIndex) {
            // Schicke das Paket in den Decoder
            avcodec_send_packet(m_videoCodecContext, m_packet);

            // Frame aus dem Decoder holen
            if (avcodec_receive_frame(m_videoCodecContext, m_videoFrame) >= 0) {
                int64_t currentPts = m_videoFrame->pts;

                // Haben wir die gewünschte Zeit erreicht oder überschritten?
                if (currentPts >= targetVideoPts) {
                    // Das ist unser neues, perfektes Startbild! Konvertieren und sichern
                    sws_scale(m_swsContext, (uint8_t const* const*)m_videoFrame->data, m_videoFrame->linesize,
                        0, m_height, m_frameRgba->data, m_frameRgba->linesize);

                    std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_currentFramePixels.size(),
                        m_currentFramePixels.begin());

                    m_currentTime = currentPts * m_videoTimebase;
                    av_packet_unref(m_packet);
                    break; // Ziel erreicht, wir beenden das Vorauslesen!
                }
            }
        }
        av_packet_unref(m_packet);
    }
    m_accumulator = 0.0;
}