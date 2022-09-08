#ifndef _BATCHRENDERER_H
#define _BATCHRENDERER_H

#include "Extension.h"
#include "Vector.h"
#include "Shader.h"
#include "Camera.h"


struct Vertex{
	Vector3f position;
	Vector2f texCoord;
	unsigned int frame;
};

static const size_t max_quad = 200;
static const size_t max_quad_vert_count = max_quad * 4;
static const size_t max_quad_index_count = max_quad * 6;

class Batchrenderer {

public:

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_indexbuffer = 0;

	uint32_t index_count = 0;

	Vertex* buffer;
	Vertex* buffer_ptr;
	std::vector<Vertex> _buffer;

	void init();
	void shutdown();
	void endBatch();
	void beginBatch();
	void flush();
	void addQuad(Vector4f position, Vector4f texCoord, unsigned int frame);

	void addQuad(std::vector<Vertex> particles);
	void setCamera(const Camera& camera);

	const Camera* m_camera;
	//const Camera& m_cameraRef;

	Shader *m_shader;

	unsigned int batchSize = 0;

	static Batchrenderer& get();

private:
	//Batchrenderer(const Camera& camera);
	Batchrenderer() = default;
	~Batchrenderer();
	static Batchrenderer s_instance;
};

#endif
