#include <engine/Fontrenderer.h>

#include "JellySplash.h"
#include "JellyIntro.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellySplash::JellySplash(StateMachine& machine) : State(machine, States::JELLYSPLASH) {
	_dt = 0.0f;
	_splashTimer = 0.0f;
	_alpha = 0.0f;
	_end = false;

	Globals::textureManager.get("splash").bind();
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
		_end = true;

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
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, _alpha));
	Globals::shapeManager.get("quad").drawRaw();	
	shader->unuse();
}