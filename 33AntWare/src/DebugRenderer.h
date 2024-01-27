#pragma once

#include <memory>
#include <engine/Camera.h>
#include <engine/Shader.h>

#include <Scene/BoundingBoxNew.h>


struct DebugVertex {
	DebugVertex(const Vector3f& position, unsigned int color) : position(position), color(color){

	}

	Vector3f position;
	unsigned int color;
};

class DebugRenderer{

public:
	/// Construct. Register subsystem. Graphics subsystem must have been initialized.
	DebugRenderer();
	/// Destruct.
	~DebugRenderer();

	/// Set the camera viewpoint. Call before rendering, or before adding geometry if you want to use culling.
	void SetView(Camera* camera);
	void SetViewProjection(const Matrix4f& projection, const Matrix4f& view);
	/// Add a line.
	void AddLine(const Vector3f& start, const Vector3f& end, const Vector4f& color);
	/// Add a line with color already converted to unsigned.
	void AddLine(const Vector3f& start, const Vector3f& end, unsigned color);
	/// Add a bounding box.
	void AddBoundingBox(const BoundingBoxNew& box, const Vector4f& color);
	/// Add a bounding box with transform.
	void AddBoundingBox(const BoundingBoxNew& box, const Matrix4f& transform, const Vector4f& color);
	/// Add a cylinder.
	void AddCylinder(const Vector3f& position, float radius, float height, const Vector4f& color);
	/// Update vertex buffer and render all debug lines to the currently set framebuffer and viewport. Then clear the lines for the next frame.
	void Render();

	

private:

	/// Debug geometry vertices.
	/// View transform.
	Matrix4f view;
	/// Projection transform.
	Matrix4f projection;
	/// Vertex buffer for the debug geometry.
	std::vector<DebugVertex> vertices;
	/// Index buffer for the debug geometry.
	std::vector<unsigned> indices;
	/// Cached shader program.
	std::shared_ptr<Shader> shader;
};