#pragma once

#include <functional>
#include <memory>

#include "engine/input/Event.h"
#include "engine/CharacterSet.h"
#include "engine/Shader.h"
#include "Widget.h"

#define TEXTFIELD_VERTEX	"#version 410 core										\n \
																					\n \
							layout(location = 0) in vec3 i_position;				\n \
																					\n \
							uniform vec4 u_color;									\n \
							uniform mat4 u_transform = mat4(1.0);					\n \
																					\n \
							out vec4 color;											\n \
																					\n \
							void main() {											\n \
								gl_Position = u_transform * vec4(i_position, 1.0);	\n \
								color  = u_color;									\n \
							}"


#define TEXTFIELD_FRGAMENT	"#version 410 core		\n \
														\n \
								in vec4 color;			\n \
								out vec4 outColor;		\n \
														\n \
								void main() {			\n \
									outColor = color;	\n \
								}"

class TextField : public Widget {

public:

	TextField();
	TextField(TextField const& rhs);
	TextField& operator=(const TextField& rhs);
	~TextField();

	void draw() override;
	virtual void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f& position) override;
	void setSize(const float sx, const float sy) override;
	void setSize(const Vector2f& size) override;
	void setOffset(const float ox, const float oy);
	void setOffset(const Vector2f& offset);
	void setPadding(const float px, const float py);
	void setPadding(const Vector2f& padding);

	void setOutlineThickness(float thickness);
	void setOutlineColor(const Vector4f& color);
	void setText(std::string text);
	void setShader(const Shader* shader);
	void setCharset(const CharacterSet& charset);
	float getTickness();

	void static SetShader(const Shader* shader);

protected:

	const Shader *m_shader;
	const CharacterSet* m_charset;

	Matrix4f m_transformOutline;
	Vector4f m_fillColor;
	Vector4f m_outlineColor;

	Vector4f m_outlineColorDefault;
	Vector4f m_outlineColorHover;	
	Vector2f m_offset;
	Vector2f m_padding;
	std::string m_text;

	float m_thickness = 0.0f;

	static std::unique_ptr<Shader> s_shader;
};