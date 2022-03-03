#pragma once
#pragma once
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Constants.h"
#include "Shader.h"



struct Character {
	unsigned int textureID; // ID handle of the glyph texture
	int size[2];
	int bearing[2];
	unsigned int advance;   // Horizontal offset to advance to next glyph
};

struct CharacterSet {	
	std::map<GLchar, Character> characters;
	float m_characterSize = 90.0f;
	bool init = false;	
	void load(std::string path);
};

class Text {
public:
	Text(std::string label, float scale = 1.0f);
	~Text();	
	void render(Vector4f color);
	void render(std::string text, Vector4f color);
	void setPosition(const Vector2f &position);
	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;
private:
	
	static CharacterSet characterSet;

	Shader *m_shaderText;
	std::map<GLchar, Character> m_characters;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;

	Vector2f m_position;
	Vector2f m_size;

	std::string m_label;
	float m_scale;
	void calcSize();
};