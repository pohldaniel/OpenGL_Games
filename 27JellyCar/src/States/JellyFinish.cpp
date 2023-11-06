#include <engine/Fontrenderer.h>

#include "JellyFinish.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellyFinish::JellyFinish(StateMachine& machine, const Framebuffer& mainRT, bool newJumpRecord, bool newTimeRecord) : 
	State(machine, CurrentState::JELLYFINISH),
	mainRT(mainRT),
	_newJumpRecord(newJumpRecord),
	_newTimeRecord(newTimeRecord){

	controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());
}

JellyFinish::~JellyFinish() {

}

void JellyFinish::fixedUpdate() {}

void JellyFinish::update() {
	Keyboard &keyboard = Keyboard::instance();

	
	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_machine.clearAndPush(new JellyMenu(m_machine));
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		_newJumpRecord = !_newJumpRecord;
	}

	if (keyboard.keyPressed(Keyboard::KEY_V)) {
		_newTimeRecord = !_newTimeRecord;
	}
}

void JellyFinish::render() {

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

	int offsetY = _newJumpRecord && _newTimeRecord ? 120 : _newJumpRecord || _newTimeRecord ? 60 : 0;

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	Globals::textureManager.get("controls").bind(0);

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + 78 - offsetY), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (102.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);

	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Level finished") / 2), static_cast<float>(Application::Height / 2 + 118), "Level finished", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Level finished") / 2), static_cast<float>(Application::Height / 2 + 120), "Level finished", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	if (_newJumpRecord && _newTimeRecord){
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 58), "New Jump Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 60), "New Jump Record!", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));

		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2 -2), "New Time Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2), "New Time Record!", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));	
	}else if (_newJumpRecord) {
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 58), "New Jump Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 60), "New Jump Record!", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));
	}else if (_newTimeRecord){
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2 + 58), "New Time Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2 + 60), "New Time Record!", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));
	}
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 + 58 - offsetY), "Main menu", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyFinish::resize(int deltaW, int deltaH) {

}