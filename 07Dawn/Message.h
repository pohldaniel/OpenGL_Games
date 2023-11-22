#pragma once

#include <vector>
#include <string>
#include <cstdarg>

#include "engine/Fontrenderer.h"
#include "engine/Clock.h"

#include "Constants.h"

struct Text {
	std::string string;
	int x_pos, y_pos;
	float red, green, blue, transparency;
	unsigned int decayrate;
	float lifetime;	
	Clock timer;
};

class Message{

public:
	void addText(int x, int y, float red, float green, float blue, float transparency, unsigned int decayrate, float lifetime, const char *text, ...);
	void draw();
	void deleteDecayed();

	static Message& Get();

private:
	Message() = default;
	std::vector<Text> m_messages;
	static Message s_instance;
};