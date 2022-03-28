#pragma once
#include "Constants.h"
#include "Shader.h"
#include "CharacterSet.h"

class Text {
public:

	Text() = default;
	Text(std::string label, float scale = 1.0f);
	Text(size_t maxChar, float scale = 1.0f);

	~Text();
	void render();
	void render(Vector4f color);
	void render(std::string label, float x, float y, float sx, float sy);
	
	void render(std::string label);
	void render(std::string label, Vector4f color);

	void setPosition(const float x, const float y);
	void setPosition(const Vector2f &position);
	
	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);

	void setColor(const Vector4f &color);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;

	void setMaxChar(const size_t maxChar);
	void calcSize(std::string label);

	static std::string floatToString(float val, int precision);
private:

	Shader *m_shaderText;
	std::map<GLchar, Character> m_characters;

	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;

	Vector2f m_position = Vector2f(0.0f, 0.0f);
	Vector2f m_size = Vector2f(0.0f, 0.0f);
	Vector2f m_origin = Vector2f(0.0f, 0.0f);
	Vector4f m_color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	std::string m_label;
	float m_scale = 1.0f;
	unsigned int m_indexCount = 0;
	void calcSize();
	void addChar(const Vector2f& pos, unsigned int _c, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	
	
	size_t m_maxChar = 5;
};