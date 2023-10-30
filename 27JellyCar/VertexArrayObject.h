#pragma once

#include "GraphicsEnums.h"

class RenderManager;

class VertexArrayObject
{
protected:

	static int _objectCount;

	unsigned int _vertexSize;

	bool _removeData;

	void* _vertices;
	int _verticesCount;

	void* _indices;
	int _indicesCount;

	VertexType _vertexType;
	VertexPrimitive _vertexPrimitive;
	VertexBufferDrawType _vertexBufferDrawType;

	VertexArrayObject();

public:

	static int GetObjectCount() { return _objectCount; }

	bool IsDataDeleted() { return _removeData; }

public:

	virtual ~VertexArrayObject();

	virtual void SetVertexPrimitive(VertexPrimitive vertexPrimitive) = 0;
	virtual void SetVertexType(VertexType vertexType) = 0;
	virtual void SetVertexBufferDrawType(VertexBufferDrawType drawType) = 0;

	virtual void CreateVertices(int count) = 0;
	virtual void SetVertices(void* data, int count) = 0;
	virtual void UpdateVertices(void* data, int count, bool deleteData = true) = 0;
	virtual void* GetVertices() = 0;

	virtual void CreateIndices(int count) = 0;
	virtual void SetIndicesNumber(int count) = 0;
	virtual void SetIndices(void* data, int count) = 0;
	virtual void UpdateIndices(void* data, int count) = 0;
	virtual void* GetIndices() = 0;

	virtual void Generate(bool deleteData = true) = 0;

	virtual void Draw() = 0;

	friend class RenderManager;
};