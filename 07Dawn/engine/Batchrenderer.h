#ifndef _BATCHRENDERER_H
#define _BATCHRENDERER_H

#include "Extension.h"
#include "Vector.h"
#include "Shader.h"
#include "Camera.h"



class Batchrenderer {

public:
	struct Vertex {
		Vector3f position;
		Vector2f texCoord;
		unsigned int frame;
	};


	void init(size_t size = 400);
	void shutdown();
	void endBatch();
	void beginBatch();
	void flush();
	void addQuad(Vector4f position, Vector4f texCoord, unsigned int frame);

	void addQuad(std::vector<Vertex> particles);
	void setCamera(const Camera& camera);
	void setShader(Shader* shader);
	static Batchrenderer& get();

private:
	//Batchrenderer(const Camera& camera);
	Batchrenderer() = default;
	~Batchrenderer();

	size_t m_maxQuad = 0;
	size_t m_maxVert = 0;
	size_t m_maxIndex = 0;

	
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo= 0;

	uint32_t indexCount = 0;

	Vertex* buffer;
	Vertex* bufferPtr;

	//const Camera& m_cameraRef;
	const Camera* m_camera;
	
	Shader *m_shader;
	


	static Batchrenderer s_instance;
};

#endif
