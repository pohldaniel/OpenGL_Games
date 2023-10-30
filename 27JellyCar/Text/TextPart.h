#ifndef TextPart_H
#define TextPart_H

#include <string>
#include <glm/glm.hpp>
#include "../Texture2.h"

class TextPart{
public:

	std::string Text;
	glm::vec2 Position;
	glm::vec3 Color;
	TextureFontAlign FontAlign;
};

#endif
