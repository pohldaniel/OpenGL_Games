#include "Transition.h"

Transition Transition::instance;

Transition& Transition::get() {
	return instance;
}

void Transition::init() {
	m_shader = Globals::shaderManager.getAssetPointer("transition");
}

void Transition::setFunction(std::function<void()> fun) { 
	m_fun = fun; 
}

void Transition::update(const float& dt) {
	m_elapsedTime += dt;
	while (m_elapsedTime >= m_updateTime) {
		m_elapsedTime -= m_updateTime;
		updateTransition(((bool)m_mode ? 1 : -1));
	}
}

void Transition::start(Mode mode) {
	m_mode = mode;
}

Shader& Transition::getShader() const { 
	return *m_shader;
}

void Transition::updateTransition(int multi) {
	glUseProgram(m_shader->m_program);
	m_shader->loadFloat("u_progress", m_progress);
	glUseProgram(0);
	m_progress += 0.01f * multi;

	if (multi > 0) {
		if (m_progress > 1.0f) {
			m_progress = 1.0f;
			if (m_fun) {
				m_fun();
				m_fun = 0;
			}
		}
	}else {
		if (m_progress < 0.0f) {
			m_progress = 0.0f;
			if (m_fun) {
				m_fun();
				m_fun = 0;
			}
		}
	}
}