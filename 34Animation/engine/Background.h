#pragma once

#include <memory>
#include "Texture.h"
#include "Vector.h"
#include "Shader.h"

#define BACKGROUND_VERTEX	"#version 410 core										\n \
																					\n \
							layout(location = 0) in vec3 i_position;				\n \
							layout(location = 1) in vec2 i_texCoord;				\n \
																					\n \
							uniform mat4 u_transform = mat4(1.0);					\n \
							uniform vec4 u_color = vec4(1.0);						\n \
							uniform vec4 u_texRect = vec4(0.0, 0.0, 1.0, 1.0);		\n \
																					\n \
							out vec2 texCoord;										\n \
							out vec4 vertColor;										\n \
																					\n \
							void main() {											\n \
								gl_Position = u_transform * vec4(i_position, 1.0);	\n \
								gl_Position.z = gl_Position.w;						\n \
																					\n \
								texCoord.x = i_texCoord.x * (u_texRect.z - u_texRect.x) + u_texRect.x; 	\n \
								texCoord.y = i_texCoord.y * (u_texRect.w - u_texRect.y) + u_texRect.y;	\n \
								vertColor  = u_color;									\n \
							}"


#define BACKGROUND_FRGAMENT	"#version 410 core												\n \
							 uniform sampler2D u_texture;									\n \
																							\n \
							 in vec2 texCoord;												\n \
							 in vec4 vertColor;												\n \
																							\n \
							 out vec4 outColor;												\n \
																							\n \
							 void main() {													\n \
								outColor = texture2D( u_texture, texCoord ) * vertColor;	\n \
							 }"


struct BackgroundLayer {

	
	BackgroundLayer(Texture* texture, unsigned int zoomX, float speed = 1.0f, float offset = 0.0f, Vector2f textureOffsetY = Vector2f(0.0f, 1.0f));
	BackgroundLayer(BackgroundLayer const& rhs);
	BackgroundLayer& operator=(const BackgroundLayer& rhs);

	void init(Texture* texture, unsigned int zoomX, float speed = 1.0f, float offset = 0.0f, Vector2f textureOffsetY = Vector2f(0.0f, 1.0f));
	void addOffset(float offset);
	void draw(Shader* shader, unsigned int width);

	Texture* m_texture;
	unsigned int m_zoomX;
	float m_speed;
	float m_offset;
	
	static void Init();

private:

	float clampOffset(float input);
	std::vector<Vector4f> m_textureRects;
	static unsigned int Vao;
	static unsigned int Vbo;
	
};

class Background {

public:
	Background();
	Background(Background const& rhs);
	Background& operator=(const Background& rhs);

	void draw();
	void update(float dt);
	void addOffset(float offset);
	void addLayer(BackgroundLayer layer);
	void setLayer(std::vector<BackgroundLayer>& layer);
	void setSpeed(float speed);
	void resize(unsigned int width, unsigned int height);

private:

	std::vector<BackgroundLayer> m_layer;
	static std::unique_ptr<Shader> s_shader;
	float m_speed;
	unsigned int m_width;
	unsigned int m_height;
};