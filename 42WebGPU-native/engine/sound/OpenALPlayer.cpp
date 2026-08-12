#include <iostream>
#include "OpenALPlayer.h"

OpenALPlayer::OpenALPlayer() {}

OpenALPlayer::~OpenALPlayer() {
    if (source) {
        alSourceStop(source);
        alDeleteSources(1, &source);
    }
    if (buffers[0]) alDeleteBuffers(2, buffers);
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }
    if (device) alcCloseDevice(device);
}

bool OpenALPlayer::init() {
    device = alcOpenDevice(nullptr);
    if (!device) return false;

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) return false;

    alGenSources(1, &source);
    alGenBuffers(2, buffers);
    return true;
}

void OpenALPlayer::play(const std::string& filename) {
    if (!decoder.open(filename)) {
        std::cerr << "Fehler beim Öffnen der MP3-Datei!" << std::endl;
        return;
    }

    // Beide Start-Buffer initial befüllen
    streamBuffer(buffers[0]);
    streamBuffer(buffers[1]);

    alSourceQueueBuffers(source, 2, buffers);
    alSourcePlay(source);
    isPlaying = true;
}

void OpenALPlayer::streamBuffer(ALuint bufferId) {
    std::vector<uint8_t> pcmData;
    // Liest so lange Frames, bis wir genug Daten zum Befüllen des OpenAL-Buffers haben
    while (pcmData.size() < 16384) { // ca. 4KB bis 16KB pro Buffer ist optimal
        std::vector<uint8_t> frameData;
        if (!decoder.decodeFrame(frameData)) {
            break; // Datei-Ende erreicht
        }
        pcmData.insert(pcmData.end(), frameData.begin(), frameData.end());
    }

    if (!pcmData.empty()) {
        alBufferData(bufferId, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), decoder.getSampleRate());
    }
}

void OpenALPlayer::update() {
    if (!isPlaying) return;

    ALint processedBuffers = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processedBuffers);
   
    // Gameloop-Sicherheit: Solange fertige Buffer da sind, füllen wir sie neu
    while (processedBuffers--) {
        ALuint unqueuedBuffer;
        alSourceUnqueueBuffers(source, 1, &unqueuedBuffer);

        std::vector<uint8_t> pcmData;
        // Erneut decodieren
        while (pcmData.size() < 16384) {
            std::vector<uint8_t> frameData;
            if (!decoder.decodeFrame(frameData)) {
                break;
            }
            pcmData.insert(pcmData.end(), frameData.begin(), frameData.end());
        }

        if (!pcmData.empty()) {
            alBufferData(unqueuedBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), decoder.getSampleRate());
            alSourceQueueBuffers(source, 1, &unqueuedBuffer);

            // Verhindert Underruns, falls OpenAL durch Stottern komplett gestoppt ist
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                alSourcePlay(source);
            }
        }
        else {
            // Keine Daten mehr -> Song fertig
            ALint queuedBuffers = 0;
            alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedBuffers);
            if (queuedBuffers == 0) {
                isPlaying = false;
                decoder.close();
                std::cout << "Wiedergabe beendet." << std::endl;
            }
        }
    }
}

void OpenALPlayer::prepareForMovie() {
    // Falls noch Musik läuft, stoppen und Source säubern
    alSourceStop(source);
    alSourcei(source, AL_BUFFER, 0);

    // Beide Buffer leeren, falls noch Reste drin sind
    ALint processed;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
    while (processed--) {
        ALuint unqueued;
        alSourceUnqueueBuffers(source, 1, &unqueued);
    }

    isPlaying = true;
}

void OpenALPlayer::streamMovieAudio(const std::vector<uint8_t>& pcmData) {
    if (pcmData.empty()) return;

    ALint processedBuffers = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processedBuffers);

    // 1. Fall: Es sind freie/abgespielte Buffer in der Warteschlange vorhanden
    if (processedBuffers > 0) {
        ALuint unqueuedBuffer;
        alSourceUnqueueBuffers(source, 1, &unqueuedBuffer);

        // Befüllen mit den neuen Film-Audiodaten
        alBufferData(unqueuedBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
        alSourceQueueBuffers(source, 1, &unqueuedBuffer);
    }
    // 2. Fall: Initiales Befüllen (beim Start des Videos sind noch keine Buffer eingereiht)
    else {
        ALint queuedBuffers = 0;
        alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedBuffers);

        if (queuedBuffers < 2) {
            // Wir schnappen uns einen der beiden globalen Buffer (buffers[0] oder buffers[1])
            ALuint targetBuffer = buffers[queuedBuffers];
            alBufferData(targetBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
            alSourceQueueBuffers(source, 1, &targetBuffer);
        }
    }

    // Verhindert Underruns (Stottern), falls OpenAL kurz stehen geblieben ist
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(source);
    }
}