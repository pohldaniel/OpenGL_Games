#include <engine/Fontrenderer.h>

#include "JellySplash.h"
#include "JellyIntroNew.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellySplash::JellySplash(StateMachine& machine) : State(machine, CurrentState::JELLYSPLASH) {
	_dt = 0.0f;
	_splashTimer = 0.0f;
	_alpha = 0.0f;
	_end = false;
}

JellySplash::~JellySplash() {
}

void JellySplash::fixedUpdate() {}

void JellySplash::update() {

	if (_end){
		return;
	}

	_dt = m_dt;
	_splashTimer += _dt;

	if (_splashTimer < 2.0){
		_alpha += _dt;
		if (_alpha >= 1.0f){
			_alpha = 1.0f;
		}
	}

	if (_splashTimer >= 3.0 && _splashTimer <= 4.0){
		_alpha -= _dt;
		if (_alpha < 0.0f){
			_alpha = 0.0f;
		}
	}

	if (_splashTimer >= 4.0){
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;
		m_machine.addStateAtBottom(new JellyIntroNew(m_machine));
		//_audioHelper->StopEngineSound();
		_end = true;
	}
}

void JellySplash::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	Globals::textureManager.get("splash").bind();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, _alpha));
	Globals::shapeManager.get("quad").drawRaw();
	Globals::textureManager.get("splash").unbind();
	shader->unuse();
}