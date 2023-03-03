#ifndef __frustumH__
#define __frustumH__

#include <memory>
#include <vector>
#include "Shader.h"
#include "Camera.h"


#define FRUSTUM_VERTEX	"#version 410 core									\n \
																			\n \
						layout(location = 0) in vec3 i_position;			\n \
						uniform vec4 u_color;								\n \
																			\n \
						out vec4 v_color;									\n \
						uniform mat4 u_transform = mat4(1.0);				\n \
																			\n \
						void main() {										\n \
						gl_Position = u_transform * vec4(i_position, 1.0);	\n \
						v_color = u_color;									\n \
						}"


#define FRUSTUM_FRGAMENT	"#version 410 core	\n \
												\n \
							in vec4 v_color;	\n \
							out vec4 color;		\n \
												\n \
												\n \
							void main() {		\n \
							color = v_color;	\n \
							}"    

class Frustum {

public:
	Frustum(const Matrix4f& perspective, float scale);
	~Frustum();

	void drawRaw();
	void draw(const Camera& camera, const Vector3f& position, const Vector3f& scale);
private:

	void createBuffer(const Matrix4f& perspective, float scale);

	unsigned int m_vao = 0;
	unsigned int m_vaoFlipped = 0;
	unsigned int m_vbo = 0;

	static std::unique_ptr<Shader> s_shaderFrustum;
	Vector3f m_center;
};
#endif
