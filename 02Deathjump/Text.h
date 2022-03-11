#pragma once
#include "Constants.h"
#include "Shader.h"
#include "CharacterSet.h"

class Text {
public:
	Text() = default;
	Text(std::string label, float scale = 1.0f);
	~Text();	
	void render(Vector4f color);
	void render(std::string text, Vector4f color);
	void setPosition(const Vector2f &position);
	void setOrigin(const Vector2f &origin);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;

private:
	
	Shader *m_shaderText;
	std::map<GLchar, Character> m_characters;
	float m_characterSize;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;

	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;

	std::string m_label;
	float m_scale;
	void calcSize();
};