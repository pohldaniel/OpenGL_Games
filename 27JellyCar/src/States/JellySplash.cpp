#include <engine/Fontrenderer.h>

#include "JellySplash.h"
#include "JellyIntro.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellySplash::JellySplash(StateMachine& machine) : State(machine, States::JELLYSPLASH) {
	m_splashTimer = 0.0f;
	m_alpha = 0.0f;
	Globals::textureManager.get("splash").bind();
}

JellySplash::~JellySplash() {
}

void JellySplash::fixedUpdate() {}

void JellySplash::update() {

	m_splashTimer += m_dt;

	if (m_splashTimer < 2.0){
		m_alpha += m_dt;
		if (m_alpha >= 1.0f){
			m_alpha = 1.0f;
		}
	}

	if (m_splashTimer >= 3.0 && m_splashTimer <= 4.0){
		m_alpha -= m_dt;
		if (m_alpha < 0.0f){
			m_alpha = 0.0f;
		}
	}

	if (m_splashTimer >= 4.0){
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		shader->unuse();

		Globals::textureManager.get("splash").unbind();

		m_isRunning = false;
		//_audioHelper->StopEngineSound();
		m_machine.addStateAtBottom(new JellyIntro(m_machine));

	}
}

void JellySplash::render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();	
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, m_alpha));
	Globals::shapeManager.get("quad").drawRaw();	
	shader->unuse();
}