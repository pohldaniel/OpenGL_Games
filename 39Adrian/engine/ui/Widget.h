#pragma once

#include <memory>
#include "../input/Event.h"
#include "../Vector.h"
#include "../Shader.h"

#define WIDGET_VERTEX      "#version 410 core										\n \
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


#define WIDGET_FRGAMENT    "#version 410 core           \n \
														\n \
                             in vec4 color;			    \n \
                             out vec4 outColor;         \n \
														\n \
                             void main() {              \n \
                                 outColor = color;      \n \
                             }"

#define BATCH_VERTEX       "#version 410 core										   \n \
																					   \n \
							layout(location = 0) in vec2 i_position;                   \n \
                            layout(location = 1) in vec2 i_texCoord;                   \n \
                            layout(location = 2) in vec4 i_color;                      \n \
                            layout(location = 3) in uint i_drawState;                  \n \
																					   \n \
							flat out uint v_layer;                                     \n \
                            out vec2 v_texCoord;                                       \n \
                            out vec4 v_color;                                          \n \
                                                                                       \n \
                            uniform mat4 u_transform = mat4(1.0);                      \n \
																					   \n \
							void main(void) {                                          \n \
                                gl_Position = u_transform * vec4(i_position, 0.0, 1.0);\n \
                                                                                       \n \
                                v_texCoord = i_texCoord;                               \n \
                                v_color = i_color;                                     \n \
                                v_layer = i_drawState;                                 \n \
                            }"


#define BATCH_FRGAMENT	    "#version 410 core       \n \
                                                     \n \
							 flat in uint v_layer;   \n \
                             in vec2 v_texCoord;     \n \
                             in vec4 v_color;        \n \
                                                     \n \
                             out vec4 outColor;      \n \
                                                     \n \
                             void main() {           \n \
	                             outColor = v_color; \n \
                             }"

class Widget {

public:

	Widget();
	Widget(Widget const& rhs);
	Widget& operator=(const Widget& rhs);
	~Widget();

	virtual void draw() = 0;
	virtual void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) = 0;

	virtual void setPosition(const float x, const float y);
	virtual void setPosition(const Vector2f& position);
	virtual void setSize(const float sx, const float sy);
	virtual void setSize(const Vector2f& size);

	const Vector2f& getPosition() const;
	const Vector2f& getSize() const;

	static void Init(unsigned int width, unsigned int height);
	static void CleanUp();
	static void Resize(unsigned int width, unsigned int height);

	void static SetWidgetShader(const Shader* shader);
	void static SetBatchShader(const Shader* shader);
	void static DrawQuad();

protected:

	Matrix4f m_transform;
	Vector2f m_position;
	Vector2f m_size;

	static Matrix4f Orthographic;
	static std::unique_ptr<Shader> WidgetShader;
	static std::unique_ptr<Shader> BatchShader;
	static unsigned int Vao;
	static unsigned int Vbo;
};