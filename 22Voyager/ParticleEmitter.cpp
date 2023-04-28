#include <GL/glew.h>
#include "ParticleEmitter.h"
#include "Constants.h"

ParticleEmitter::ParticleEmitter() : m_numberOfParticles(500){

}

ParticleEmitter::~ParticleEmitter(){

}

void ParticleEmitter::Init(int numOfParticles) {

	m_numberOfParticles = numOfParticles;

	for (unsigned int i = 0; i < m_numberOfParticles; ++i) {
		m_position.push_back(Vector3f(0.0f, 0.0f, 0.0f));
		Particle p;
		m_particles.push_back(p);
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(TOTAL_BUFFERS, m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[VERTEX_VB]);
	glBufferData(GL_ARRAY_BUFFER, m_position.size() * sizeof(Vector3f), &m_position[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleEmitter::Render(const Camera& cam, float dt, const Vector3f& origin) {
	auto shader = Globals::shaderManager.getAssetPointer("particle");
	shader->use();

	std::vector<Particle>::iterator iter = m_particles.begin();
	while (iter != m_particles.end()) {
		// Check if the particle's dead, if so delete it, otherwise update its position and send its updated state to the GPU
		if(!iter->Update(dt)){
			iter = m_particles.erase(iter);
		}else {

			iter->SetOrigin(origin);
			Vector3f pos = iter->GetPos();
			Matrix4f model = Matrix4f::Translate(pos[0] + 2.7f, pos[1] - 2.5f, pos[2]);
			shader->loadMatrix("model", model);
			shader->loadMatrix("view", cam.getViewMatrix());
			shader->loadMatrix("projection", cam.getPerspectiveMatrix());
			

			glBindVertexArray(m_vao);
			Globals::textureManager.get("redOrb").bind();
			glDrawArrays(GL_POINTS, 0, m_numberOfParticles);
			Globals::textureManager.get("redOrb").unbind();
			glBindVertexArray(0);

			++iter;
		}
	}
	shader->unuse();
}
