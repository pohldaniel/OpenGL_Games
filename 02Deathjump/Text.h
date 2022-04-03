#pragma once
#include <memory>
#include "Constants.h"
#include "Shader.h"
#include "CharacterSet.h"

#define TEXT_VERTEX		"#version 330 core									\n \
																			\n \
						layout(location = 0) in vec3 i_position;			\n \
						layout(location = 1) in vec2 i_texCoord;			\n \
																			\n \
						out vec2 texCoord;									\n \
						uniform mat4 u_transform = mat4(1.0);				\n \
																			\n \
						void main() {										\n \
						gl_Position = u_transform * vec4(i_position, 1.0);	\n \
						texCoord = i_texCoord;								\n \
						}"


#define TEXT_FRGAMENT	"#version 330 core													\n \
																							\n \
						in vec2 texCoord;													\n \
						out vec4 color;														\n \
																							\n \
						uniform sampler2D tex;												\n \
						uniform vec4 textColor;												\n \
																							\n \
						void main() {														\n \
						color = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r) * textColor;	\n \
						}"    

class Text {
public:

	Text(CharacterSet &charset);
	Text(std::string label, CharacterSet &charset, float scale = 1.0f);
	Text(size_t maxChar, CharacterSet &charset, float scale = 1.0f);
	Text(Text const& rhs);
	Text& operator=(const Text& rhs);

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

	void setLabel(std::string label, float scale = 1.0f);
	void setMaxChar(const size_t maxChar, float scale = 1.0f);
	void calcSize(std::string label);

	static std::string floatToString(float val, int precision);
	CharacterSet &m_charset;
private:
	
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

	static unsigned short s_total;
	static std::unique_ptr<Shader> s_shaderText;
};