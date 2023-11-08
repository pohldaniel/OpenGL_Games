#include <engine/Fontrenderer.h>

#include "JellyDialog.h"
#include "JellyMenu.h"
#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellyDialog::JellyDialog(StateMachine& machine, Framebuffer& mainRT, std::string text) :
	State(machine, States::JELLYDIALOG),
	mainRT(mainRT),
	m_text(text){

	controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());
}

JellyDialog::~JellyDialog() {

}

void JellyDialog::fixedUpdate() {}

void JellyDialog::update() {
	Keyboard &keyboard = Keyboard::instance();


	if (keyboard.keyPressed(Keyboard::KEY_D)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_machine.clearAndPush(new JellyMenu(m_machine));
	}
}

void JellyDialog::render() {

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

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + 18), 0.0f) * Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / controlsWidth, (controlsHeight - (5.0f + 78.0f)) / controlsHeight, (102.0f + 78.0f) / controlsWidth, (controlsHeight - 5.0f) / controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);

	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Level finished") / 2), static_cast<float>(Application::Height / 2 + 58), m_text, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Level finished") / 2), static_cast<float>(Application::Height / 2 + 60), m_text, Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 2), "Main menu", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyDialog::resize(int deltaW, int deltaH) {
	m_machine.resizeState(deltaW, deltaH, States::JELLYGAME);
}