#ifndef _BATCHRENDERER_H
#define _BATCHRENDERER_H

#include "../Constants.h"
#include "Extension.h"
#include "Vector.h"
#include "Shader.h"

struct Vertex{
	Vector4f posTex;
	Vector4f color;
};

static const size_t max_quad = 1000;
static const size_t max_quad_vert_count = max_quad * 4;
static const size_t max_quad_index_count = max_quad * 6;

class Batchrenderer {
public:

	Batchrenderer() = default;
	~Batchrenderer();

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_indexbuffer = 0;

	uint32_t index_count = 0;

	Vertex* buffer;
	Vertex* buffer_ptr;
	std::vector<Vertex> _buffer;
	// stats
	int draw_calls = 0;
	int quad_vertex = 0;

	void init();
	void resetStats();
	void shutdown();
	void endBatch();
	void beginBatch();
	void flush();
	void addQuad(Vector2f position, Vector2f size, Vector4f color);
	void addVertex(Vector2f position, Vector4f color);
	void addQuad(std::vector<Vertex> particles);


	void endBatch2();
	void beginBatch2();
	void flush2();
	void resetStats2();

	Shader *m_shader;

	unsigned int batchSize = 0;
};

#endif
