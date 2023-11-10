#include <engine/Fontrenderer.h>

#include "JellyDialog.h"
#include "JellyMenu.h"
#include "JellyOptions.h"
#include "JellyGame.h"

#include "Application.h"
#include "Globals.h"
#include "JellyHelper.h"

JellyDialog::JellyDialog(StateMachine& machine, Framebuffer& mainRT) : State(machine, States::JELLYDIALOG),mainRT(mainRT){
	m_controlsWidth = static_cast<float>(Globals::textureManager.get("controls").getWidth());
	m_controlsHeight = static_cast<float>(Globals::textureManager.get("controls").getHeight());	
}

JellyDialog::JellyDialog(StateMachine& machine, Framebuffer& mainRT, std::string text) : JellyDialog(machine, mainRT) {
	m_text = text;
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
	shader->loadVector("u_texRect", Vector4f(102.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (102.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);

	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth(m_text) / 2), static_cast<float>(Application::Height / 2 + 58), m_text, Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth(m_text) / 2), static_cast<float>(Application::Height / 2 + 60), m_text, Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 2), "Main menu", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

void JellyDialog::resize(int deltaW, int deltaH) {
	m_machine.resizeState(deltaW, deltaH, States::JELLYGAME);
}

JellyDialogPause::JellyDialogPause(StateMachine& machine, Framebuffer& mainRT, JellyGame* game) : JellyDialog(machine, mainRT){
	m_game = game;
}

void JellyDialogPause::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_D) || (keyboard.keyPressed(Keyboard::KEY_A) && m_game->m_checkpoint)) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		shader->use();
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
		shader->unuse();

		m_isRunning = false;

		if (m_game->m_checkpoint)
			m_game->spawnAtCheckpoint();
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
}

void JellyDialogPause::render() {
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
	shader->loadVector("u_texRect", Vector4f(102.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (102.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	int offsetY = 40;
	if (m_game->m_checkpoint) {
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + 2), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
		shader->loadVector("u_texRect", Vector4f(273.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (273.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));
		Globals::shapeManager.get("quad_half").drawRaw();
		offsetY = 0;
	}

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f)  * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 - 38 + offsetY), 0.0f) * Matrix4f::Scale(static_cast<float>(74), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(239.0f / m_controlsWidth, (m_controlsHeight - (194.0f + 38.0f)) / m_controlsHeight, (239.0f + 74.0f) / m_controlsWidth, (m_controlsHeight - 194.0f) / m_controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f)  * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 - 78 + offsetY), 0.0f) * Matrix4f::Scale(static_cast<float>(74), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(159.0f / m_controlsWidth, (512.0f - (193.0f + 38.0f)) / m_controlsHeight, (159.0f + 74.0f) / m_controlsWidth, (m_controlsHeight - 193.0f) / m_controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Pause") / 2), static_cast<float>(Application::Height / 2 + 72 - 2), "Pause", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Pause") / 2), static_cast<float>(Application::Height / 2 + 72), "Pause", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 + 22), "Resume", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));

	if (m_game->m_checkpoint) {
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 18), "Go to checkpoint", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	}
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 58 + offsetY), "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 - 98 + offsetY), "Main menu", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));

	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}

JellyDialogFinish::JellyDialogFinish(StateMachine& machine, Framebuffer& mainRT, bool newJumpRecord, bool newTimeRecord) : JellyDialog(machine, mainRT) {
	m_newJumpRecord = newJumpRecord;
	m_newTimeRecord = newTimeRecord;
}

void JellyDialogFinish::update() {
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
		m_newJumpRecord = !m_newJumpRecord;
	}

	if (keyboard.keyPressed(Keyboard::KEY_V)) {
		m_newTimeRecord = !m_newTimeRecord;
	}
}

void JellyDialogFinish::render() {
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

	int offsetY = m_newJumpRecord && m_newTimeRecord ? 120 : m_newJumpRecord || m_newTimeRecord ? 60 : 0;

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	Globals::textureManager.get("controls").bind(0);

	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(Application::Width / 2), static_cast<float>(Application::Height / 2 + 78 - offsetY), 0.0f)* Matrix4f::Scale(static_cast<float>(78 * 0.4f), static_cast<float>(78 * 0.4f), 1.0f));
	shader->loadVector("u_texRect", Vector4f(102.0f / m_controlsWidth, (m_controlsHeight - (5.0f + 78.0f)) / m_controlsHeight, (102.0f + 78.0f) / m_controlsWidth, (m_controlsHeight - 5.0f) / m_controlsHeight));
	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);

	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Level finished") / 2), static_cast<float>(Application::Height / 2 + 118), "Level finished", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("Level finished") / 2), static_cast<float>(Application::Height / 2 + 120), "Level finished", Vector4f(1.0f, 0.65f, 0.0f, 1.0f));

	if (m_newJumpRecord && m_newTimeRecord) {
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 58), "New Jump Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 60), "New Jump Record!", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));

		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2 - 2), "New Time Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2), "New Time Record!", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));
	}
	else if (m_newJumpRecord) {
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 58), "New Jump Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Jump Record!") / 2), static_cast<float>(Application::Height / 2 + 60), "New Jump Record!", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));
	}
	else if (m_newTimeRecord) {
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2 + 58), "New Time Record!", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_32").getWidth("New Time Record!") / 2), static_cast<float>(Application::Height / 2 + 60), "New Time Record!", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));
	}
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 50), static_cast<float>(Application::Height / 2 + 58 - offsetY), "Main menu", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
}
