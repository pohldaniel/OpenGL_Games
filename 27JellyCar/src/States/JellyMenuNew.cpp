#include <engine/Fontrenderer.h>

#include "JellyMenuNew.h"
#include "Application.h"
#include "Globals.h"

JellyMenuNew::JellyMenuNew(StateMachine& machine) : State(machine, CurrentState::JELLYMENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);


	backWidth = Globals::textureManager.get("paper").getWidth();
	backHeight = Globals::textureManager.get("paper").getHeight();

	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
}

JellyMenuNew::~JellyMenuNew() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void JellyMenuNew::fixedUpdate() {}

void JellyMenuNew::update() {}

void JellyMenuNew::render() {

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	auto shader = Globals::shaderManager.getAssetPointer("quad_back");

	shader->use();
	Globals::textureManager.get("paper").bind(0);

	for (int y = 0; y < rows; y++){
		for (int x = 0; x < columns; x++) {

			int posx = (backWidth * x);
			int posy = Application::Height - (backHeight * (1 + y));

			shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f) * Matrix4f::Scale(static_cast<float>(backWidth), static_cast<float>(backHeight), 1.0f));
			Globals::shapeManager.get("quad_aligned").drawRaw();
		}
	}
	Globals::textureManager.get("paper").unbind(0);

	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	//512 512
	//glm::vec2(239.0f, 194.0f), glm::vec2(74.0f, 38.0f) || start 
	//glm::vec2(159.0f, 193.0f), glm::vec2(74.0f, 38.0f) || select 

	//left = 239 / 512 || right = (239 + 74) / 512
	//bottom = (512 - (194 + 38))/ 512 || top = (512 - 194)/ 512
	
	glEnable(GL_BLEND);
	Globals::textureManager.get("controls").bind(0);

	int posx = Application::Width / 2 - 40 ;
	int posy = 29;
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f)* Matrix4f::Scale(static_cast<float>(78), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(239.0f / 512.0f, (512 - (194.0f + 38.0f)) / 512.0f, (239.0f + 74.0f) / 512.0f, (512 - 194.0f) / 512.0f));

	Globals::shapeManager.get("quad_half").drawRaw();

	posx = Application::Width / 2 + 40 ;
	posy = 29;
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f) * Matrix4f::Translate(static_cast<float>(posx), static_cast<float>(posy), 0.0f)* Matrix4f::Scale(static_cast<float>(78), static_cast<float>(38), 1.0f));
	shader->loadVector("u_texRect", Vector4f(159.0f/ 512.0f, (512 - (193.0f + 38.0f))/ 512.0f, (159.0f + 74.0f)/512.0f, (512 - 193.0f) / 512.0f));

	Globals::shapeManager.get("quad_half").drawRaw();

	Globals::textureManager.get("controls").unbind(0);
	shader->unuse();

	Globals::spritesheetManager.getAssetPointer("jelly_font")->bind(0);

	int levelTextPositionY = Application::Height - ( (Application::Height / 2) - 156 - 30);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Tutorial") / 2), levelTextPositionY, "Tutorial", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_64"), static_cast<float>(Application::Width / 2 - Globals::fontManager.get("jelly_64").getWidth("Tutorial") / 2), levelTextPositionY + 3, "Tutorial", Vector4f(1.0f,  0.65f, 0.0f, 1.0f));

	int timePositionX = (Application::Width / 2) - (272 / 2);
	int jumpPositionX = (Application::Width / 2) + (272 / 2);
	int textPosY = Application::Height - ((Application::Height / 2) + 30);

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), timePositionX , textPosY, "999.00s", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), timePositionX , textPosY + 2, "999.00s", Vector4f(0.0f, 0.84f, 0.0f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), jumpPositionX - Globals::fontManager.get("jelly_32").getWidth("0.00m"), textPosY, "0.00m", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), jumpPositionX - Globals::fontManager.get("jelly_32").getWidth("0.00m"), textPosY + 2, "0.00m", Vector4f(0.71f, 0.16f, 0.18f, 1.0f));

	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 - 90 - Globals::fontManager.get("jelly_32").getWidth("Options")), 10, "Options", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("jelly_32"), static_cast<float>(Application::Width / 2 + 90), 10, "Exit", Vector4f(0.19f, 0.14f, 0.17f, 1.0f));
	Fontrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("jelly_font")->unbind(0);
	glDisable(GL_BLEND);

}

void JellyMenuNew::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void JellyMenuNew::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void JellyMenuNew::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void JellyMenuNew::resize(int deltaW, int deltaH) {
	columns = ceil(static_cast<float>(Application::Width) / static_cast<float>(backWidth));
	rows = ceil(static_cast<float>(Application::Height) / static_cast<float>(backHeight));
}