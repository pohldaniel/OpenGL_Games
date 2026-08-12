#include "VideoDecoder.h"

VideoDecoder::VideoDecoder() {
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_frameRgba = av_frame_alloc();
}

VideoDecoder::~VideoDecoder() {
    close();
    av_packet_free(&m_packet);
    av_frame_free(&m_frame);
    av_frame_free(&m_frameRgba);
}

bool VideoDecoder::open(const std::string& filename) {
    if (avformat_open_input(&m_formatContext, filename.c_str(), nullptr, nullptr) < 0) return false;
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) return false;

    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }
    if (m_videoStreamIndex == -1) return false;

    const AVCodec* codec = avcodec_find_decoder(m_formatContext->streams[m_videoStreamIndex]->codecpar->codec_id);
    m_codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecContext, m_formatContext->streams[m_videoStreamIndex]->codecpar);
    if (avcodec_open2(m_codecContext, codec, nullptr) < 0) return false;

    m_width = m_codecContext->width;
    m_height = m_codecContext->height;

    AVRational streamFps = m_formatContext->streams[m_videoStreamIndex]->r_frame_rate;
    double fps = (streamFps.den > 0) ? av_q2d(streamFps) : 30.0;
    m_timePerFrame = 1.0 / fps; // Dauer eines einzelnen Video-Frames

    int size = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_width, m_height, 1);
    m_rgbaBufferInternal = (uint8_t*)av_malloc(size * sizeof(uint8_t));
    m_currentFramePixels.resize(size, 0); // Vektor-Größe initialisieren

    av_image_fill_arrays(m_frameRgba->data, m_frameRgba->linesize, m_rgbaBufferInternal,
        AV_PIX_FMT_RGBA, m_width, m_height, 1);

    m_swsContext = sws_getContext(m_width, m_height, m_codecContext->pix_fmt,
        m_width, m_height, AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr, nullptr);

    // Erstes Frame direkt decodieren, damit wir nicht mit einem schwarzen Bild starten
    decodeNextFrame();
    return true;
}

bool VideoDecoder::update(double deltaTime, std::vector<uint8_t>& outRgbaBuffer) {
    m_accumulator += deltaTime;

    bool newFrameDecoded = false;

    // Falls die Gameloop mal laggt (z.B. großer Frame-Drop), 
    // überspringen wir Video-Frames, um synchron zur Echtzeit zu bleiben
    while (m_accumulator >= m_timePerFrame) {
        if (decodeNextFrame()) {
            newFrameDecoded = true;
        }
        m_accumulator -= m_timePerFrame;
    }

    // Unabhängig davon, ob neu decodiert wurde oder nicht:
    // Wir spucken immer den aktuellen Pixelbuffer aus
    outRgbaBuffer = m_currentFramePixels;

    return newFrameDecoded; // Meldet der Gameloop, ob die GPU-Textur geupdated werden muss
}

bool VideoDecoder::decodeNextFrame() {
    while (av_read_frame(m_formatContext, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            if (avcodec_send_packet(m_codecContext, m_packet) >= 0) {
                if (avcodec_receive_frame(m_codecContext, m_frame) >= 0) {

                    // Konvertierung nach RGBA
                    sws_scale(m_swsContext, (uint8_t const* const*)m_frame->data, m_frame->linesize,
                        0, m_height, m_frameRgba->data, m_frameRgba->linesize);

                    // In unseren internen Pixel-Cache kopieren
                    std::copy(m_rgbaBufferInternal, m_rgbaBufferInternal + m_currentFramePixels.size(),
                        m_currentFramePixels.begin());

                    av_packet_unref(m_packet);
                    return true;
                }
            }
        }
        av_packet_unref(m_packet);
    }
    return false; // EOF
}

void VideoDecoder::close() {
    if (m_swsContext) { sws_freeContext(m_swsContext); m_swsContext = nullptr; }
    if (m_rgbaBufferInternal) { av_free(m_rgbaBufferInternal); m_rgbaBufferInternal = nullptr; }
    if (m_codecContext) { avcodec_free_context(&m_codecContext); m_codecContext = nullptr; }
    if (m_formatContext) { avformat_close_input(&m_formatContext); m_formatContext = nullptr; }
}