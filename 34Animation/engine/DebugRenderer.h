#pragma once

#include <memory>
#include "Camera.h"
#include "Shader.h"
#include "BoundingBox.h"

#include "scene/BoneNode.h"


#define DEBUG_VERTEX       "#version 410 core                                 \n \
                                                                              \n \
							layout(location = 0) in vec3 i_position;          \n \
							layout(location = 1) in vec4 i_color;             \n \
                                                                              \n \
							uniform mat4 u_vp;                                \n \
							out vec4 v_color;                                 \n \
                                                                              \n \
                                                                              \n \
							void main() {                                     \n \
								gl_Position = u_vp * vec4(i_position, 1.0);   \n \
								v_color = i_color;                            \n \
							}"


#define DEBUG_FRGAMENT     "#version 410 core           \n \
														\n \
                             in vec4 v_color;           \n \
                             out vec4 outColor;         \n \
														\n \
                             void main() {              \n \
                                 outColor = v_color;    \n \
                             }"

struct DebugVertex {
	Vector3f position;
	unsigned int color;
};

class DebugRenderer{

public:

	DebugRenderer() = default;
	~DebugRenderer();


	void SetView(Camera* camera);
	void SetViewProjection(const Matrix4f& projection, const Matrix4f& view);
	void AddLine(const Vector3f& start, const Vector3f& end, const Vector4f& color);
	void AddLine(const Vector3f& start, const Vector3f& end, unsigned int color);
	void AddBoundingBox(const BoundingBox& box, const Vector4f& color);
	void AddBoundingBox(const BoundingBox& box, const Matrix4f& transform, const Vector4f& color);
	void AddCylinder(const Vector3f& position, float radius, float height, const Vector4f& color);
	void AddSkeleton(BoneNode**& bones, unsigned short numBones, const Vector4f& color);

	void init(size_t size = 400);
	void shutdown();
	void drawBuffer();
	void disable();
	void enable();
	void setEnable(bool enable);

	static DebugRenderer& Get();
	static bool& Enabled();

private:

	Matrix4f view;
	Matrix4f projection;

	DebugVertex* vertices;
	DebugVertex* verticesPtr;

	unsigned int* indices;
	unsigned int* indicesPtr;

	unsigned int m_vao = 0u;
	unsigned int m_vbo = 0u;
	unsigned int m_ibo = 0u;

	size_t m_maxBoxes = 0;
	size_t m_maxVert = 0;
	size_t m_maxIndex = 0;

	uint32_t indexCount = 0;
	uint32_t vertexCount = 0;

	static DebugRenderer s_instance;
	static bool s_enabled;
	static std::unique_ptr<Shader> DebugShader;
};