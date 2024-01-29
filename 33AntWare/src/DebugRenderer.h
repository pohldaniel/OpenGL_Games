#pragma once

#include <memory>
#include <engine/Camera.h>
#include <engine/Shader.h>
#include <engine/BoundingBox.h>

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

	void init(size_t size = 400);
	void shutdown();
	void drawBuffer();
	void disable();
	void enable();

	static DebugRenderer& Get();
	static bool& Enabled();

private:

	Matrix4f view;
	Matrix4f projection;
	std::shared_ptr<Shader> shader;

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
};