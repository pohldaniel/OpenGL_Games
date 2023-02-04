#include "FluidSystem.h"

FluidSystem::FluidSystem(int width, int height, int depth) : m_width(width), m_height(height), m_depth(depth), m_current(0) {

	m_state[0] = new VolumeBuffer(GL_RGBA16F, width, height, depth);
	m_state[1] = new VolumeBuffer(GL_RGBA16F, width, height, depth);
	m_prevState = new VolumeBuffer(GL_RGBA16F, width, height, depth);

	m_splat = new Shader("res/shader/compute/splat.vert", "res/shader/compute/splat.frag");
	m_copy = new Shader("res/shader/compute/copy.cs");
	m_waves = new Shader("res/shader/compute/waves.cs");
	m_clear = new Shader("res/shader/compute/clear.cs");
	m_step = new Shader("res/shader/compute/step.vert", "res/shader/compute/step.frag");

	m_state[0]->setShader(m_splat);
	m_state[1]->setShader(m_splat);

	reset();
}

FluidSystem::~FluidSystem() {
	delete m_state[0];
	delete m_state[1];
	delete m_prevState;

	delete m_splat;
	delete m_copy;
	delete m_waves;
	delete m_step;
}

float FluidSystem::frand() {
	return rand() / (float)RAND_MAX;
}

// add a random splat to the simulation
void FluidSystem::splat() {

	glUseProgram(m_splat->m_program);
	m_splat->loadVector("centre", Vector3f(frand(), frand(), frand()));
	m_splat->loadFloat("radius", 0.1f);
	m_splat->loadVector("color", Vector4f(frand(), frand(), frand(), 1.0f));
	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	m_state[m_current]->draw();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// reset the simulation by clearing buffers
void FluidSystem::reset() { 
	glUseProgram(m_clear->m_program);
	m_clear->loadVector("color", Vector4f(0.0f, 0.0f, 0.0, 0.0f));
	glBindImageTexture(0, m_state[m_current]->getTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(m_width / 8, m_height / 8, m_depth / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, m_state[1 - m_current]->getTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(m_width / 8, m_height / 8, m_depth / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, m_prevState->getTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(m_width / 8, m_height / 8, m_depth / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(0);
}

// step the simulation
void FluidSystem::step(float timestep) {
	glUseProgram(m_copy->m_program);
	glBindImageTexture(0, m_state[1 - m_current]->getTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(1, m_prevState->getTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(m_width / 8, m_height / 8, m_depth / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);

	glUseProgram(m_waves->m_program);
	m_waves->loadFloat("damping", 0.99f);
	glBindImageTexture(0, m_state[m_current]->getTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(1, m_prevState->getTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(2, m_state[1 - m_current]->getTexture(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(m_width / 8, m_height / 8, m_depth / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);

	m_current = 1 - m_current;
}