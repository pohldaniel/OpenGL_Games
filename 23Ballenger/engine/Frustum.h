#ifndef __frustumH__
#define __frustumH__

#include <memory>
#include <vector>
#include "Shader.h"
#include "Camera.h"

#define FRUSTUM_VERTEX	"#version 410 core											\n \
																					\n \
						layout(location = 0) in vec3 i_position;					\n \
						layout(location = 1) in uint i_count;						\n \
						uniform vec4 u_color;										\n \
																					\n \
						out vec4 v_color;											\n \
						uniform mat4 u_transform = mat4(1.0);						\n \
																					\n \
						void main() {												\n \
						gl_Position = u_transform * vec4(i_position, 1.0);			\n \
						v_color = i_count < 4 ? vec4(1.0, 1.0, 1.0, 1.0f) : u_color;\n \
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

	Frustum();
	~Frustum();

	void drawFrustm(const Matrix4f& projection, const Matrix4f& view, float distance);
	void updatePlane(const Matrix4f& perspective, const Matrix4f& view, const Matrix4f& model = Matrix4f::IDENTITY);
	void update(const Matrix4f& perspective, const Matrix4f& view);

	void init();
	bool intersectAABBFrustum(const Vector3f& position, const Vector3f& size);
	bool aabbVisible(const Vector3f* AABBVertices);
	float aabbDistance(const Vector3f* AABBVertices);
	bool& getDebug();

	static bool IntersectAABBPlane(const Vector3f& position, const Vector3f& size, const Vector4f& plane);
	
private:

	Vector4f m_planes[6];
	unsigned short m_origins[6];

	unsigned int m_vao = 0;
	unsigned int m_vboPos = 0;
	unsigned int m_vboCount = 0;
	bool m_debug = true;

	static std::unique_ptr<Shader> s_shaderFrustum;
};
#endif
