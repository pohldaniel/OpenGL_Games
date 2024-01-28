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

	/// Set the camera viewpoint. Call before rendering, or before adding geometry if you want to use culling.
	void SetView(Camera* camera);
	void SetViewProjection(const Matrix4f& projection, const Matrix4f& view);
	/// Add a line.
	void AddLine(const Vector3f& start, const Vector3f& end, const Vector4f& color);
	/// Add a line with color already converted to unsigned.
	void AddLine(const Vector3f& start, const Vector3f& end, unsigned int color);
	/// Add a bounding box.
	void AddBoundingBox(const BoundingBox& box, const Vector4f& color);
	/// Add a bounding box with transform.
	void AddBoundingBox(const BoundingBox& box, const Matrix4f& transform, const Vector4f& color);
	/// Add a cylinder.
	void AddCylinder(const Vector3f& position, float radius, float height, const Vector4f& color);
	/// Update vertex buffer and render all debug lines to the currently set framebuffer and viewport. Then clear the lines for the next frame.


	void init(size_t size = 400);
	void shutdown();
	void drawBuffer();

	static DebugRenderer& Get();

private:

	/// Debug geometry vertices.
	/// View transform.
	Matrix4f view;
	/// Projection transform.
	Matrix4f projection;
	/// Vertex buffer for the debug geometry.
	//std::vector<DebugVertex> vertices;
	/// Index buffer for the debug geometry.
	//std::vector<unsigned int> indices;
	/// Cached shader program.
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
};