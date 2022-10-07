#pragma once

#include <vector>
#include <string>
#include <cstdarg>

#include "engine/Fontrenderer.h"

struct Text {
	std::string string;
	int x_pos, y_pos;
	float red, green, blue, transparency;
	int decayrate;
	float lifetime;		
	int lastframe, thisframe;
	unsigned int startdecaylastframe, startdecaythisframe;
};

class Message{

public:
	Message();
	~Message();
	void addText(int x, int y, float red, float green, float blue, float transparency, int decayrate, float lifetime, const char *text, ...);
	void draw();
	void deleteDecayed();

	static Message& Get();

private:

	std::vector<Text> m_messages;
	static Message s_instance;
};