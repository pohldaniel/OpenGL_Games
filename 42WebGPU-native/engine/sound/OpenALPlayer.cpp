#include <iostream>
#include "OpenALPlayer.h"

OpenALPlayer::OpenALPlayer() {
    m_ringBuffer.init(65536);
   
}

OpenALPlayer::~OpenALPlayer() {
    if (m_source) {
        alSourceStop(m_source);
        alDeleteSources(1, &m_source);
    }
    if (m_buffers[0]) 
        alDeleteBuffers(2, m_buffers);
    
}

bool OpenALPlayer::init() {
    alGenSources(1, &m_source);
    alGenBuffers(2, m_buffers);
    alSourcef(m_source, AL_GAIN, 0.5f);
    return true;
}

void OpenALPlayer::play(const std::string& filename) {
    if (!m_decoder.open(filename)) {
        std::cerr << "Fehler beim Öffnen der MP3-Datei!" << std::endl;
        return;
    }

    // Beide Start-Buffer initial befüllen
    streamBuffer(m_buffers[0]);
    streamBuffer(m_buffers[1]);

    alSourceQueueBuffers(m_source, 2, m_buffers);
    alSourcePlay(m_source);
    m_isPlaying = true;
}

void OpenALPlayer::streamBuffer(ALuint bufferId) {
    std::vector<uint8_t> pcmData;
    // Liest so lange Frames, bis wir genug Daten zum Befüllen des OpenAL-Buffers haben
    while (pcmData.size() < 16384) { // ca. 4KB bis 16KB pro Buffer ist optimal
        std::vector<uint8_t> frameData;
        if (!m_decoder.decodeFrame(frameData)) {
            break; // Datei-Ende erreicht
        }
        pcmData.insert(pcmData.end(), frameData.begin(), frameData.end());
    }

    if (!pcmData.empty()) {
        alBufferData(bufferId, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), m_decoder.getSampleRate());
    }
}

void OpenALPlayer::update() {
    if (!m_isPlaying) return;

    ALint processedBuffers = 0;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);
   
    // Gameloop-Sicherheit: Solange fertige Buffer da sind, füllen wir sie neu
    while (processedBuffers--) {
        ALuint unqueuedBuffer;
        alSourceUnqueueBuffers(m_source, 1, &unqueuedBuffer);

        std::vector<uint8_t> pcmData;
        // Erneut decodieren
        while (pcmData.size() < 16384) {
            std::vector<uint8_t> frameData;
            if (!m_decoder.decodeFrame(frameData)) {
                break;
            }
            pcmData.insert(pcmData.end(), frameData.begin(), frameData.end());
        }

        if (!pcmData.empty()) {
            alBufferData(unqueuedBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), m_decoder.getSampleRate());
            alSourceQueueBuffers(m_source, 1, &unqueuedBuffer);

            // Verhindert Underruns, falls OpenAL durch Stottern komplett gestoppt ist
            ALint state;
            alGetSourcei(m_source, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                alSourcePlay(m_source);
            }
        }
        else {
            // Keine Daten mehr -> Song fertig
            ALint queuedBuffers = 0;
            alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBuffers);
            if (queuedBuffers == 0) {
                m_isPlaying = false;
                m_decoder.close();
                std::cout << "Wiedergabe beendet." << std::endl;
            }
        }
    }
}

void OpenALPlayer::prepareForMovie() {
    // Falls noch Musik läuft, stoppen und Source säubern
    alSourceStop(m_source);
    alSourcei(m_source, AL_BUFFER, 0);

    // Beide Buffer leeren, falls noch Reste drin sind
    ALint processed;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
    while (processed--) {
        ALuint unqueued;
        alSourceUnqueueBuffers(m_source, 1, &unqueued);
    }

    m_isPlaying = true;
}

void OpenALPlayer::streamMovieAudio(const std::vector<uint8_t>& pcmData) {
    if (pcmData.empty()) return;

    ALint processedBuffers = 0;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processedBuffers);

    // 1. Fall: Es sind freie/abgespielte Buffer in der Warteschlange vorhanden
    if (processedBuffers > 0) {
        ALuint unqueuedBuffer;
        alSourceUnqueueBuffers(m_source, 1, &unqueuedBuffer);

        // Befüllen mit den neuen Film-Audiodaten
        alBufferData(unqueuedBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
        alSourceQueueBuffers(m_source, 1, &unqueuedBuffer);
    }
    // 2. Fall: Initiales Befüllen (beim Start des Videos sind noch keine Buffer eingereiht)
    else {
        ALint queuedBuffers = 0;
        alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queuedBuffers);

        if (queuedBuffers < 2) {
            // Wir schnappen uns einen der beiden globalen Buffer (buffers[0] oder buffers[1])
            ALuint targetBuffer = m_buffers[queuedBuffers];
            alBufferData(targetBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
            alSourceQueueBuffers(m_source, 1, &targetBuffer);
        }
    }

    // Verhindert Underruns (Stottern), falls OpenAL kurz stehen geblieben ist
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(m_source);
    }
}

/*void OpenALPlayer::tick() {
    // OpenAL braucht hier kein aktives Füttern mehr, da enqueueData() das übernimmt!
    // Wir prüfen nur zur Sicherheit, ob der Stream nach einem Ruckler gestoppt ist
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    ALint queued = 0;
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    if (state != AL_PLAYING && queued >= 2) {
        alSourcePlay(source);
    }
}*/

void OpenALPlayer::enqueueData(const std::vector<uint8_t>& pcmData) {
    if (m_source == 0) return;

    // New frisch eingetroffene Daten am Ende unseres Sammlers anhängen
    if (!pcmData.empty()) {
        m_audioAccumulator.insert(m_audioAccumulator.end(), pcmData.begin(), pcmData.end());
    }

    // BEHINDERT MICRO-RUCKELN: Erst abschicken, wenn genug "Fleisch" da ist!
    // 4096 Bytes bei 44.1kHz Stereo entspricht einem lückenlosen Polster von ~23ms.
    // Bei 240 FPS sammelt er ca. 5-6 Frames lang, bevor er OpenAL füttert.
    if (m_audioAccumulator.size() < 4096) {
        return;
    }

    ALint processed = 0;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);

    if (processed > 0) {
        ALuint unqueued;
        alSourceUnqueueBuffers(m_source, 1, &unqueued);

        // Wir übergeben die gesammelten Daten aus dem Akkumulator
        alBufferData(unqueued, AL_FORMAT_STEREO16, m_audioAccumulator.data(), m_audioAccumulator.size(), 44100);
        alSourceQueueBuffers(m_source, 1, &unqueued);

        // Den Sammler leeren, da die Daten erfolgreich bei der Soundkarte liegen
        m_audioAccumulator.clear();
    }
    else {
        ALint queued = 0;
        alGetSourcei(m_source, AL_BUFFERS_QUEUED, &queued);

        if (queued < 2) {
            ALuint targetBuffer = m_buffers[queued];
            alBufferData(targetBuffer, AL_FORMAT_STEREO16, m_audioAccumulator.data(), m_audioAccumulator.size(), 44100);
            alSourceQueueBuffers(m_source, 1, &targetBuffer);

            m_audioAccumulator.clear();
        }
        // Falls beide Buffer voll sind und noch kein Buffer "processed" ist,
        // behalten wir die Daten im m_audioAccumulator und warten auf den nächsten Frame!
    }

    // Stream-Sicherheitsschaltung gegen Stillstand
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(m_source);
    }

    /*if (pcmData.empty()) return;

    ALint processedBuffers = 0;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processedBuffers);

    // 1. Fall: Ein Buffer ist fertig -> Unqueue, neu befüllen, Re-queue
    if (processedBuffers > 0) {
        ALuint unqueuedBuffer;
        alSourceUnqueueBuffers(source, 1, &unqueuedBuffer);

        alBufferData(unqueuedBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
        alSourceQueueBuffers(source, 1, &unqueuedBuffer);
    }
    // 2. Fall: Initiales Befüllen der ersten beiden Buffer
    else {
        ALint queuedBuffers = 0;
        alGetSourcei(source, AL_BUFFERS_QUEUED, &queuedBuffers);

        if (queuedBuffers < 2) {
            ALuint targetBuffer = buffers[queuedBuffers];
            alBufferData(targetBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
            alSourceQueueBuffers(source, 1, &targetBuffer);
        }
        else {
            // DAS WAR DER FEHLER IM ALTEN CODE: Wenn die Pipeline voll ist, 
            // dürfen wir die Daten nicht verwerfen. Wir erzwingen das Freimachen eines Buffers!
            // (Oder blockieren ganz kurz, bis die Soundkarte Platz hat)
            ALuint forcedBuffer;

            // Warte aktiv, bis OpenAL einen Buffer freigibt (passiert in Bruchteilen von Millisekunden)
            while (processedBuffers == 0) {
                alGetSourcei(source, AL_BUFFERS_PROCESSED, &processedBuffers);
            }

            alSourceUnqueueBuffers(source, 1, &forcedBuffer);
            alBufferData(forcedBuffer, AL_FORMAT_STEREO16, pcmData.data(), pcmData.size(), 44100);
            alSourceQueueBuffers(source, 1, &forcedBuffer);
        }
    }

    // Sicherstellen, dass es läuft
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
        alSourcePlay(source);
    }*/
}