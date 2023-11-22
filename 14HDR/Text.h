#pragma once

#include <GL/glew.h>
#include <vector>

#include "engine/Vector.h"
#include "engine/Shader.h"
#include "CharacterSetBmp.h"
#include "Constants.h"

class Text {
public:

	Text(CharacterSetBmp &charset);
	Text(std::string label, CharacterSetBmp &charset);

	void setLabel(std::string label, float scale = 1.0f);
	void addChar(const Vector2f& pos, unsigned int _c, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	void render();
	void calcSize(std::string label);
	void setColor(const Vector4f &color);
	void setPosition(const float x, const float y);
	void setPosition(const Vector2f &position);

	CharacterSetBmp &m_charset;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f m_projection = Matrix4f::IDENTITY;
	std::string m_label;
	unsigned int m_indexCount = 0;

	Shader *m_fontShader;
	
	float m_scale = 0.5f;

	Vector2f m_size = Vector2f(0.0f, 0.0f);
	Vector4f m_blendColor = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
};