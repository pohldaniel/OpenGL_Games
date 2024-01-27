#include "DebugRenderer.h"
#include "Globals.h"

DebugRenderer::DebugRenderer(){
	//vertexElements.push_back(VertexElement(ELEM_VECTOR3, SEM_POSITION));
	//vertexElements.push_back(VertexElement(ELEM_UBYTE4, SEM_COLOR));

	//indexBuffer = new IndexBuffer();
	//vertexBuffer = new VertexBuffer();

	shader = std::make_shared<Shader>(Globals::shaderManager.getAssetPointer("debug_lines"));
}

void DebugRenderer::SetView(Camera* camera){
	
	if (!camera)
		return;

	view = camera->getViewMatrix();
	projection = camera->getPerspectiveMatrix();
}

void DebugRenderer::AddLine(const Vector3f& start, const Vector3f& end, const Vector4f& color){
	AddLine(start, end, color.toUInt());
}

void DebugRenderer::AddLine(const Vector3f& start, const Vector3f& end, unsigned color){
	unsigned startVertex = (unsigned)vertices.size();

	vertices.push_back(DebugVertex(start, color));
	vertices.push_back(DebugVertex(end, color));

	indices.push_back(startVertex);
	indices.push_back(startVertex + 1);
}

void DebugRenderer::AddBoundingBox(const BoundingBoxNew& box, const Vector4f& color){

	unsigned startVertex = (unsigned)vertices.size();
	unsigned uintColor = color.toUInt();

	const Vector3f& min = box.min;
	const Vector3f& max = box.max;

	vertices.push_back(DebugVertex(min, uintColor));
	vertices.push_back(DebugVertex(Vector3f(max[0], min[1], min[2]), uintColor));
	vertices.push_back(DebugVertex(Vector3f(max[0], max[1], min[2]), uintColor));
	vertices.push_back(DebugVertex(Vector3f(min[0], max[1], min[2]), uintColor));
	vertices.push_back(DebugVertex(Vector3f(min[0], min[1], max[2]), uintColor));
	vertices.push_back(DebugVertex(Vector3f(max[0], min[1], max[2]), uintColor));
	vertices.push_back(DebugVertex(Vector3f(min[0], max[1], max[2]), uintColor));
	vertices.push_back(DebugVertex(max, uintColor));

	indices.push_back(startVertex);
	indices.push_back(startVertex + 1);

	indices.push_back(startVertex + 1);
	indices.push_back(startVertex + 2);

	indices.push_back(startVertex + 2);
	indices.push_back(startVertex + 3);

	indices.push_back(startVertex + 3);
	indices.push_back(startVertex);

	indices.push_back(startVertex + 4);
	indices.push_back(startVertex + 5);

	indices.push_back(startVertex + 5);
	indices.push_back(startVertex + 7);

	indices.push_back(startVertex + 7);
	indices.push_back(startVertex + 6);

	indices.push_back(startVertex + 6);
	indices.push_back(startVertex + 4);

	indices.push_back(startVertex + 0);
	indices.push_back(startVertex + 4);

	indices.push_back(startVertex + 1);
	indices.push_back(startVertex + 5);

	indices.push_back(startVertex + 2);
	indices.push_back(startVertex + 7);

	indices.push_back(startVertex + 3);
	indices.push_back(startVertex + 6);
}

void DebugRenderer::AddBoundingBox(const BoundingBoxNew& box, const Matrix4f& transform, const Vector4f& color){

	unsigned startVertex = (unsigned)vertices.size();
	unsigned uintColor = color.toUInt();

	const Vector3f& min = box.min;
	const Vector3f& max = box.max;

	vertices.push_back(DebugVertex(Vector3f(transform * min), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * Vector3f(max[0], min[1], min[2])), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * Vector3f(max[0], max[1], min[2])), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * Vector3f(min[0], max[1], min[2])), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * Vector3f(min[0], min[1], max[2])), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * Vector3f(max[0], min[1], max[2])), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * Vector3f(min[0], max[1], max[2])), uintColor));
	vertices.push_back(DebugVertex(Vector3f(transform * max), uintColor));

	indices.push_back(startVertex);
	indices.push_back(startVertex + 1);

	indices.push_back(startVertex + 1);
	indices.push_back(startVertex + 2);

	indices.push_back(startVertex + 2);
	indices.push_back(startVertex + 3);

	indices.push_back(startVertex + 3);
	indices.push_back(startVertex);

	indices.push_back(startVertex + 4);
	indices.push_back(startVertex + 5);

	indices.push_back(startVertex + 5);
	indices.push_back(startVertex + 7);

	indices.push_back(startVertex + 7);
	indices.push_back(startVertex + 6);

	indices.push_back(startVertex + 6);
	indices.push_back(startVertex + 4);

	indices.push_back(startVertex + 0);
	indices.push_back(startVertex + 4);

	indices.push_back(startVertex + 1);
	indices.push_back(startVertex + 5);

	indices.push_back(startVertex + 2);
	indices.push_back(startVertex + 7);

	indices.push_back(startVertex + 3);
	indices.push_back(startVertex + 6);
}

void DebugRenderer::AddCylinder(const Vector3f& position, float radius, float height, const Vector4f& color){
	Vector3f heightVec(0, height, 0);
	Vector3f offsetXVec(radius, 0, 0);
	Vector3f offsetZVec(0, 0, radius);

	/*Sphere sphere(position, radius);
	for (float i = 0.0f; i < 360.0f; i += 45.0f){
		Vector3f p1 = sphere.Point(i, 90.0f);
		Vector3f p2 = sphere.Point(i + 45.0f, 90.0f);
		AddLine(p1, p2, color);
		AddLine(p1 + heightVec, p2 + heightVec, color);
	}*/

	AddLine(position + offsetXVec, position + heightVec + offsetXVec, color);
	AddLine(position - offsetXVec, position + heightVec - offsetXVec, color);
	AddLine(position + offsetZVec, position + heightVec + offsetZVec, color);
	AddLine(position - offsetZVec, position + heightVec - offsetZVec, color);
}

void DebugRenderer::Render(){

}