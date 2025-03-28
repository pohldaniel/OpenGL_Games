#include "Message.h"

Message Message::s_instance;

Message& Message::Get() {
	return s_instance;
}

void Message::draw() {

	for (unsigned int book = 0; book < m_messages.size(); book++) {
		// if we have a lifetime, we need to wait until we start decaying the message.
		if(m_messages[book].lifetime > 0.0){
			// currently waiting 1 second, until we decrease our lifetime with one second and then repeat.
			if (m_messages[book].timer.getElapsedTimeMilli() > 1000) {
				m_messages[book].timer.reset();
				m_messages[book].lifetime -= 1.0;				
			}
		}else {
			if (m_messages[book].timer.getElapsedTimeMilli() > m_messages[book].decayrate) {
				m_messages[book].timer.reset();
				m_messages[book].transparency -= 0.01f;
			}
		}

		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_12"), m_messages[book].x_pos, m_messages[book].y_pos, m_messages[book].string, Vector4f(m_messages[book].red, m_messages[book].green, m_messages[book].blue, m_messages[book].transparency));
	}
}

void Message::addText(int x, int y, float red, float green, float blue, float transparency, unsigned int decayrate, float lifetime, const char *text, ...){
	char buffer[1024];
	std::va_list args;

	// writing our text and arguments to the buffer
	va_start(args, text);
	vsnprintf(buffer, 1024, text, args);
	va_end(args);

	// push everything to our vector.
	m_messages.push_back({std::string(buffer), x, y, red, green, blue, transparency, decayrate, lifetime});
}

void Message::deleteDecayed() {
	for (unsigned int book = 0; book < m_messages.size(); book++) {
		if (m_messages[book].transparency < 0.0) {
			m_messages.erase(m_messages.begin() + book);
		}
	}
}