#pragma once
#include <vector>
#include <string>
#include <iostream>

#include <AL/al.h>
#include <AL/alc.h>





class AudioStreamer {
private:
   
    int audioStreamIndex = -1;

    // OpenAL Member
    ALCdevice* alDevice = nullptr;
    ALCcontext* alContext = nullptr;
    static const int NUM_BUFFERS = 4;
    ALuint buffers[NUM_BUFFERS];
    ALuint source = 0;

    // Streaming Zustand
    std::vector<uint8_t> pcmTempBuffer;
    const size_t BUFFER_SIZE = 40960; // ~40KB pro Häppchen
    int initialBuffersQueued = 0;
    bool isFinished = false;
    int outSampleRate = 44100;

    void queueBuffer(ALuint bufferId, std::vector<uint8_t>& data) {
        alBufferData(bufferId, AL_FORMAT_STEREO16, data.data(), static_cast<ALsizei>(data.size()), outSampleRate);
        alSourceQueueBuffers(source, 1, &bufferId);
        data.clear();
    }

public:
    AudioStreamer() = default;

    ~AudioStreamer() {
        //cleanup();
    }

    bool init(const std::string& filename) {
       

        // 3. OpenAL initialisieren
        alDevice = alcOpenDevice(nullptr);
        if (!alDevice) return false;

        std::cout << alDevice << "  " << alContext << std::endl;
        ALCint attributes[] = {
        ALC_FREQUENCY, 44100,
        ALC_MONO_SOURCES, 4,
        ALC_STEREO_SOURCES, 4,
        0 // Array-Abschluss
        };
        alContext = alcCreateContext(alDevice, nullptr);

        std::cout << alDevice << "  " << alContext << std::endl;

        if (!alContext || !alcMakeContextCurrent(alContext)) return false;

        /*alGenBuffers(NUM_BUFFERS, buffers);
        alGenSources(1, &source);*/

        return true;
    }

    void update() {
        /*if (isFinished) return;

        // 1. Prüfen, ob OpenAL Buffer fertig abgespielt hat
        ALint processed = 0;
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while (processed > 0 && initialBuffersQueued >= NUM_BUFFERS) {
            ALuint unqueuedBuffer;
            alSourceUnqueueBuffers(source, 1, &unqueuedBuffer);
            processed--;

            // Wenn wir genug Daten im Temp-Buffer haben, direkt wieder füllen
            if (pcmTempBuffer.size() >= BUFFER_SIZE) {
                std::vector<uint8_t> chunk(pcmTempBuffer.begin(), pcmTempBuffer.begin() + BUFFER_SIZE);
                pcmTempBuffer.erase(pcmTempBuffer.begin(), pcmTempBuffer.begin() + BUFFER_SIZE);
                queueBuffer(unqueuedBuffer, chunk);
            }
            else {
                // Nicht genug Daten? Wir merken uns den freien Buffer, indem wir die Queue-Zahl senken
                initialBuffersQueued--;
                // Wir schieben den Buffer temporär ans Ende, um ihn später zu befüllen
                buffers[initialBuffersQueued] = unqueuedBuffer;
            }
        }

        // 2. FFmpeg füttern, falls unser Temp-Buffer Nachschub braucht
        if (pcmTempBuffer.size() < BUFFER_SIZE * 2) {
            if (av_read_frame(formatContext, packet) >= 0) {
                if (packet->stream_index == audioStreamIndex) {
                    if (avcodec_send_packet(codecContext, packet) >= 0) {
                        while (avcodec_receive_frame(codecContext, frame) >= 0) {
                            int maxOutSamples = av_rescale_rnd(
                                swr_get_delay(swrContext, codecContext->sample_rate) + frame->nb_samples,
                                outSampleRate, codecContext->sample_rate, AV_ROUND_UP
                            );

                            uint8_t* outputBuffer = nullptr;
                            av_samples_alloc(&outputBuffer, nullptr, 2, maxOutSamples, AV_SAMPLE_FMT_S16, 0);

                            int convertedSamples = swr_convert(
                                swrContext, &outputBuffer, maxOutSamples,
                                (const uint8_t**)frame->data, frame->nb_samples
                            );

                            if (convertedSamples > 0) {
                                int dataSize = convertedSamples * 2 * 2; // 2 Kanäle * 2 Bytes
                                pcmTempBuffer.insert(pcmTempBuffer.end(), outputBuffer, outputBuffer + dataSize);
                            }
                            av_freep(&outputBuffer);
                        }
                    }
                }
                av_packet_unref(packet);
            }
            else {
                // Dateiende erreicht
                if (pcmTempBuffer.empty() && initialBuffersQueued == 0) {
                    isFinished = true;
                }
            }
        }

        // 3. Buffer abschicken, falls genug Daten da sind und Plätze in OpenAL frei sind
        while (pcmTempBuffer.size() >= BUFFER_SIZE && initialBuffersQueued < NUM_BUFFERS) {
            std::vector<uint8_t> chunk(pcmTempBuffer.begin(), pcmTempBuffer.begin() + BUFFER_SIZE);
            pcmTempBuffer.erase(pcmTempBuffer.begin(), pcmTempBuffer.begin() + BUFFER_SIZE);

            queueBuffer(buffers[initialBuffersQueued], chunk);
            initialBuffersQueued++;
        }

        // 4. Wiedergabe am Laufen halten
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING && initialBuffersQueued > 0) {
            alSourcePlay(source);
        }*/
    }

    void cleanup() {
        if (source) {
            alSourceStop(source);
            alDeleteSources(1, &source);
            source = 0;
        }
        alDeleteBuffers(NUM_BUFFERS, buffers);
        if (alContext) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(alContext);
            alContext = nullptr;
        }
        if (alDevice) {
            alcCloseDevice(alDevice);
            alDevice = nullptr;
        }
        
    }
};