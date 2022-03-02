#pragma once
#pragma once
#include <iostream>
#include <map>
#include "Constants.h"
#include "Shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
	unsigned int textureID; // ID handle of the glyph texture
	int size[2];
	int bearing[2];
	unsigned int advance;   // Horizontal offset to advance to next glyph
};

class Text {
public:
	Text();
	~Text();	
	void renderText(std::string text, float x, float y, float scale, Vector3f color);
private:

	Shader *m_shaderText;
	std::map<GLchar, Character> Characters;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f projection = Matrix4f::IDENTITY;
};