#include "VertexArrayObject.h"

int VertexArrayObject::_objectCount = 0;

VertexArrayObject::VertexArrayObject()
{
	_objectCount++;

	_vertexSize = 0;
	_verticesCount = 0;
	_indicesCount = 0;

	_vertices = 0;
	_indices = 0;

	_removeData = true;

	_vertexType = VertexType::Simple;
	_vertexPrimitive = VertexPrimitive::Triangle;
	_vertexBufferDrawType = VertexBufferDrawType::StaticDraw;
}

VertexArrayObject::~VertexArrayObject()
{
	_objectCount--;
}