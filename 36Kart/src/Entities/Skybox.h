#pragma once

#include <engine/Object.h>
#include <engine/Camera.h>
#include <Entities/Entity.h>

#define SKYBOX_VERTEX      "#version 410 core										           \n \
																					           \n \
							layout(location = 0) in vec3 i_position;				           \n \
																					           \n \
							uniform mat4 u_view;									           \n \
							uniform mat4 u_projection;                                         \n \
																					           \n \
							out vec3 texCoord;                                                 \n \
																					           \n \
							void main() {											           \n \
								gl_Position = u_projection * u_view * vec4(i_position, 1.0f);  \n \
								gl_Position.z = gl_Position.w;								   \n \
                                texCoord = i_position;                                         \n \
							}"


#define SKYBOX_FRGAMENT    "#version 410 core                             \n \
														                  \n \
                             in vec3 texCoord;			                  \n \
                             out vec4 outColor;                           \n \
                             uniform samplerCube u_texture;               \n \
                                                                          \n \
                             void main() {                                \n \
                                 outColor = texture(u_texture, texCoord); \n \
                             }"


class Skybox : public Object, public Entity {

public:

	Skybox();
	~Skybox();
	void draw() override;
	void setCamera(const Camera& camera);

private:

	void drawCube();
	void createCube();
	const Camera* m_camera;
	unsigned int m_vao;
	unsigned int m_vbo;

	static std::unique_ptr<Shader> SkyboxShader;
};