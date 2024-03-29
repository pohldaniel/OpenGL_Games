#include <iostream>
#include <GL/glew.h>
#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer(){
	
}

void ParticleRenderer::draw(const ParticleSystem& particleSystem) {

	NormalAttributes aux;
	normalVertex.clear();

	for (int i = 0; i < particleSystem.particles.size(); i++){
		const ParticleSystem::Particle& p = particleSystem.particles[i];

		aux.pos = p.pos;
		aux.color = p.color;
		aux.size = p.size;
		aux.alpha = p.alpha;

		aux.uv = Vector2f(0.0f, 0.0f);
		normalVertex.push_back(aux);	
		aux.uv = Vector2f(1.0f, 1.0f);
		normalVertex.push_back(aux);
		aux.uv = Vector2f(0.0f, 1.0f);
		normalVertex.push_back(aux);

		aux.uv = Vector2f(0.0f, 0.0f);
		normalVertex.push_back(aux);	
		aux.uv = Vector2f(1.0f, 0.0f);
		normalVertex.push_back(aux);
		aux.uv = Vector2f(1.0f, 1.0f);
		normalVertex.push_back(aux);
	}

	//glDepthMask(false);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].pos);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].uv);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].color);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].size);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, false, sizeof(NormalAttributes), &normalVertex[0].alpha);

	glDrawArrays(GL_TRIANGLES, 0, normalVertex.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);

	//glDepthMask(true);
}