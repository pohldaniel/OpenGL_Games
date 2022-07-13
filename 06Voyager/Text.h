#pragma once
#include <vector>

#include "engine/Extension.h"
#include "engine/Vector.h"
#include "engine/Shader.h"

#include "Constants.h"

#include "CharacterSetBmp.h"

class Text {
public:

	Text(CharacterSetBmp &charset);
	Text(std::string label, CharacterSetBmp &charset);

	void setLabel(std::string label);
	void addChar(const Vector2f& pos, unsigned int _c, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	void render();


	CharacterSetBmp &m_charset;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;
	std::string m_label;
	unsigned int m_indexCount = 0;

	Shader *m_fontShader;
	Texture* m_texture;

	float m_scale = 1.0f;
};