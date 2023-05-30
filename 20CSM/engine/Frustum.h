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

struct Plane {
	Vector3f normal; 
	float d;

	void normalize(){
		Plane Result;
		float distance = sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		normal[0] = normal[0] * (1.0f / distance);
		normal[1] = normal[1] * (1.0f / distance);
		normal[2] = normal[2] * (1.0f / distance);
		d = d * (1.0f / distance);
	}
};


class Frustum {

public:
	Frustum(const Matrix4f& perspective, float scale);
	~Frustum();

	void drawRaw();
	void draw(const Camera& camera, const Vector3f& position, const Vector3f& scale);
	void drawFrustm(const Camera& camera, const Vector3f& debugShift = Vector3f(0.0f, 0.0f, 0.0f));

	Plane m_planes[6];

	void updatePlane(const Camera& camera, const Matrix4f& perspective, const Matrix4f& model = Matrix4f::IDENTITY);
	bool intersectAABBFrustum(const Vector3f& position, const Vector3f& size);

	static bool IntersectAABBPlane(const Vector3f& position, const Vector3f& size, const Plane& plane);

private:

	void createBuffer(const Matrix4f& perspective, float scale);

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	unsigned int m_vaoFrustum = 0;
	unsigned int m_vboFrustum = 0;
	bool m_debug = true;

	static std::unique_ptr<Shader> s_shaderFrustum;
	Vector3f m_center;	
};
#endif
