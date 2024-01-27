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