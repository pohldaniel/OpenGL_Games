#ifndef __frustumH__
#define __frustumH__

#include <memory>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "BoundingBox.h"

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

static const size_t NUM_FRUSTUM_PLANES = 6;
static const size_t NUM_FRUSTUM_VERTICES = 8;
static const size_t NUM_SAT_AXES = 3 + 5 + 3 * 6;

class Frustum;
struct SATData{
	void calculate(const Frustum& frustum);
	Vector3f axes[NUM_SAT_AXES];
	std::pair<float, float> frustumProj[NUM_SAT_AXES];
};

class Frustum {

	friend struct SATData;

public:

	Frustum();
	~Frustum();

	void drawFrustum(const Matrix4f& projection, const Matrix4f& view, float distance);
	void updatePlane(const Matrix4f& perspective, const Matrix4f& view, const Matrix4f& model);
	void updatePlane(const Matrix4f& perspective, const Matrix4f& view);
	void updateVertices(const Matrix4f& perspective, const Matrix4f& view);
	void updateVbo(const Matrix4f& perspective, const Matrix4f& view);

	void init();
	bool intersectAABBFrustum(const Vector3f& position, const Vector3f& size);
	bool aabbVisible(const Vector3f* AABBVertices);
	float aabbDistance(const Vector3f* AABBVertices);
	bool& getDebug();
	unsigned char isInsideMasked(const BoundingBox& box, unsigned char planeMask = 0x3f) const;
	BoundingBox::Intersection isInsideSAT(const BoundingBox& box, const SATData& data) const;

	static bool IntersectAABBPlane(const Vector3f& position, const Vector3f& size, const Vector4f& plane);
	
	SATData m_frustumSATData;

private:

	std::pair<float, float> projected(const Vector3f& axis) const;

	Vector4f m_planes[NUM_FRUSTUM_PLANES];
	unsigned short m_origins[NUM_FRUSTUM_PLANES];
	Vector3f m_vertices[NUM_FRUSTUM_VERTICES];

	unsigned int m_vao = 0;
	unsigned int m_vboPos = 0;
	unsigned int m_vboCount = 0;
	bool m_debug = false;

	static std::unique_ptr<Shader> s_shaderFrustum;
};
#endif
