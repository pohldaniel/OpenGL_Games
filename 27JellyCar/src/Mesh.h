#pragma once

enum VertexPrimitive2 {
	_Triangle,
	_TriangleStrip,
	_TriangleFan,
	_Lines
};

enum VertexBufferDrawType2 {
	_StaticDraw,
	_DynamicDraw
};

enum VertexType2 {
	_Simple,
	_Color,
	_Textured,
	_TextureNormal,
	_TextureColor,
	_ColorNormal,
	_TextureColorNormal
};

class Mesh {

public:

	Mesh();
	~Mesh();

	void drawRaw();
	void createBuffer(bool removeData = false);

	void setVertexPrimitive(VertexPrimitive2 vertexPrimitive);
	void setVertexType(VertexType2 vertexType);
	void setVertexBufferDrawType(VertexBufferDrawType2 drawType);
	void createVertices(int count);
	void setVertices(void* data, int count);
	void updateVertices(void* data, int count, bool deleteData);
	void* getVertices();
	void createIndices(int count);
	void setIndicesNumber(int count);
	void setIndices(void* data, int count);
	void updateIndices(void* data, int count);
	void* getIndices();
	unsigned int getVertexSize(VertexType2 vertexType);

private:

	unsigned int m_vertexSize;

	void* m_vertices;
	int m_verticesCount;

	void* m_indices;
	int m_indicesCount;

	VertexType2 m_vertexType;
	VertexPrimitive2 m_vertexPrimitive;
	VertexBufferDrawType2 m_vertexBufferDrawType;

	void setVertexArtibutes();

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_drawMode;	
};