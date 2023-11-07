#include <engine/Fontrenderer.h>

#include "JellyPause.h"
#include "JellyMenu.h"
#include "JellyOptions.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellyPause::JellyPause(StateMachine& machine, const Framebuffer& mainRT) : State(machine, States::JELLYPAUSE) , mainRT(mainRT) {
	controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());
	_checkpoint = false;
}

JellyPause::~JellyPause() {
	
}

void JellyPause::fixedUpdate() {}

void JellyPause::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;
	}

	if (keyboard.keyPressed(Keyboard::KEY_ENTER) && !keyboard.keyDown(Keyboard::KEY_RALT)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_machine.addStateAtTop(new JellyOptions(m_machine));
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_machine.clearAndPush(new JellyMenu(m_machine));
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		_checkpoint = !_checkpoint;
	}
}

void JellyPause::render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	mainRT.bindColorTexture();
	Globals::shapeManager.get("quad").drawRaw();
	mainRT.unbindColorTexture();
	shader->unuse();

	
	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();

	Globals::textureManager.get("controls").bind(0);

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + 42), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (102.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();
	
	int offsetY = 40;
	if (_checkpoint) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + 2), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
		shader->loadVector("u_texRect", Vector4f(273.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (273.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));
		Globals::shapeManager.get("quad_half").drawRaw();
		offsetY = 0;
	}

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f)  * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 - 38 + offsetY), 0.0f) * Matrix4f::Scale(static_cast<float>(74), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(239.0f / controlsWidth, (controlsHeight - (194.0f + 38.0f)) / controlsHeight, (239.0f + 74.0f) / controlsWidth, (controlsHeight - 194.0f) / controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f)  * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 - 78 + offsetY), 0.0f) * Matrix4f::Scale(static_cast<float>(74), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(159.0f / controlsWidth, (512.0f - (193.0f + 38.0f)) / controlsHeight, (159.0f + 74.0f) / controlsWidth, (controlsHeight - 193.0f) / controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);



	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Pause") / 2), static_cast<float>(Application::Height / 2 + 72 - 2), "Pause", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Pause") / 2), static_cast<float>(Application::Height / 2 + 72), "Pause", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 + 22), "Resume", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));

	if (_checkpoint) {
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 18), "Go to checkpoint", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));	
	}
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 58 + offsetY), "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 98 + offsetY), "Main menu", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));

	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyPause::resize(int deltaW, int deltaH) {	
	m_machine.resizeState(deltaW, deltaH, States::JELLYGAME);
}