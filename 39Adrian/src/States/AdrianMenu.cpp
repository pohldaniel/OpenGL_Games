#include <engine/Fontrenderer.h>

#include <States/Menu.h>
#include <States/Adrian.h>

#include "AdrianMenu.h"
#include "Application.h"
#include "Globals.h"



AdrianMenu::AdrianMenu(StateMachine& machine) : State(machine, States::ADRIAN_MENU) {
	Globals::musicManager.get("background").play("data/wavs/menu.wav");
	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	items.resize(24);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Globals::fontManager.get("tahomab_64").bind();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font_ttf"));
	items[0] = MenuItem(strlen("PLAY GAME") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 600.0f, 400.0f, 600.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true);
	items[0].OnClick = [&]() {
		items[0].enable = false;
		items[1].enable = false;
		items[2].enable = false;
		items[3].enable = false;
		items[4].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[19].show();
		items[19].animate(animatePercent);
		items[20].show();
		items[20].animate(animatePercent);
		items[21].show();
		items[21].animate(animatePercent);
		items[22].show();
		items[22].animate(animatePercent);
		items[23].show();
		items[23].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "PLAY GAME", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("PLAY GAME") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[0].texture);

	items[1] = MenuItem(strlen("SETTINGS") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 900.0f, 500.0f, 400.0f, 500.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true );
	items[1].OnClick = [&]() {
		items[0].enable = false;
		items[1].enable = false;
		items[2].enable = false;
		items[3].enable = false;
		items[4].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[5].show();
		items[5].animate(animatePercent);
		items[6].show();
		items[6].animate(animatePercent);
		items[7].show();
		items[7].animate(animatePercent);
		items[8].show();
		items[8].animate(animatePercent);
		items[9].show();
		items[9].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SETTINGS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("SETTINGS") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[1].texture);

	
	items[2] = MenuItem(strlen("OPTIONS") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 400.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "OPTIONS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("OPTIONS") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[2].texture);

	items[3] = MenuItem(strlen("CREDITS") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 0.0f, 400.0f, 300.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true);
	items[3].OnClick = [&]() {
		items[0].enable = false;
		items[1].enable = false;
		items[2].enable = false;
		items[3].enable = false;
		items[4].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[10].show();
		items[10].animate(animatePercent);
		items[11].show();
		items[11].animate(animatePercent);
		items[12].show();
		items[12].animate(animatePercent);
		items[13].show();
		items[13].animate(animatePercent);
		
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "CREDITS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("CREDITS") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[3].texture);
	
	items[4] = MenuItem(strlen("QUIT") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 700.0f, 400.0f, 200.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true);
	items[4].OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "QUIT", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("QUIT") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[4].texture);

	items[5] = MenuItem(strlen("VIDEO") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 401.0f, 600.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[5].OnClick = [&]() {
		items[5].enable = false;
		items[6].enable = false;
		items[7].enable = false;
		items[8].enable = false;
		items[9].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[14].show();
		items[14].animate(animatePercent);
		items[15].show();
		items[15].animate(animatePercent);
		items[16].show();
		items[16].animate(animatePercent);
		items[17].show();
		items[17].animate(animatePercent);
		items[18].show();
		items[18].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "VIDEO", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("VIDEO") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[5].texture);

	items[6] = MenuItem(strlen("SOUND") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 1000.0f, 500.0f, 407.0f, 500.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SOUND", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("SOUND") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[6].texture);
	
	items[7] = MenuItem(strlen("KEYBOARD") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 400.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "KEYBOARD", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("KEYBOARD") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[7].texture);
	
	items[8] = MenuItem(strlen("MOUSE") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 300.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "MOUSE", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("MOUSE") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[8].texture);

	items[9] = MenuItem(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 0.0f, 402.0f, 200.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[9].OnClick = [&]() {
		items[5].enable = false;
		items[6].enable = false;
		items[7].enable = false;
		items[8].enable = false;
		items[9].enable = false;
		
		animatePercent = 0.0f;
		animatePercent += 1;
		items[0].show();
		items[0].animate(animatePercent);
		items[1].show();
		items[1].animate(animatePercent);
		items[2].show();
		items[2].animate(animatePercent);
		items[3].show();
		items[3].animate(animatePercent);
		items[4].show();
		items[4].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[9].texture);
	
	items[10] = MenuItem(strlen("BHANU KALYAN") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 600.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BHANU KALYAN", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("BHANU KALYAN") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[10].texture);
	
	items[11] = MenuItem(strlen("VAMSI KRISHNA") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 500.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "VAMSI KRISHNA", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("VAMSI KRISHNA") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[11].texture);

	items[12] = MenuItem(strlen("SWAMY SUMAN") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 400.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SWAMY SUMAN", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("SWAMY SUMAN") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[12].texture);

	items[13] = MenuItem(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 300.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[13].OnClick = [&]() {
		items[10].enable = false;
		items[11].enable = false;
		items[12].enable = false;
		items[13].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[0].show();
		items[0].animate(animatePercent);
		items[1].show();
		items[1].animate(animatePercent);
		items[2].show();
		items[2].animate(animatePercent);
		items[3].show();
		items[3].animate(animatePercent);
		items[4].show();
		items[4].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[13].texture);
	
	items[14] = MenuItem(strlen("640 X 480") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 600.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "640 X 480", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("640 X 480") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[14].texture);
	
	items[15] = MenuItem(strlen("800 X 600") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 500.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "800 X 600", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("800 X 600") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[15].texture);

	items[16] = MenuItem(strlen("1024 X 768") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 400.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "1024 X 768", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("1024 X 768") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[16].texture);

	items[17] = MenuItem(strlen("1600 X 1200") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 300.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "1600 X 1200", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("1600 X 1200") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[17].texture);
	
	items[18] = MenuItem(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 200.0f, ANIMATION_STRAIGHT, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[18].OnClick = [&]() {
		items[14].enable = false;
		items[15].enable = false;
		items[16].enable = false;
		items[17].enable = false;
		items[18].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[5].show();
		items[5].animate(animatePercent);
		items[6].show();
		items[6].animate(animatePercent);
		items[7].show();
		items[7].animate(animatePercent);
		items[8].show();
		items[8].animate(animatePercent);
		items[9].show();
		items[9].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[18].texture);
	
	items[19] = MenuItem(strlen("Area 51, Nevada") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 600.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[19].OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine, "data/textures/buildings/ground.tga"));
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "Area 51, Nevada", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("Area 51, Nevada") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[19].texture);

	items[20] = MenuItem(strlen("USS Nimitz, Pacific") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 500.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[20].OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine, "data/textures/buildings/concrete.tga"));
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "USS Nimitz, Pacific", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("USS Nimitz, Pacific") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[20].texture);
	
	items[21] = MenuItem(strlen("Ross Island, Antarctica") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 400.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[21].OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine, "data/textures/buildings/snow.jpg"));
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "Ross Island, Antarctica", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("Ross Island, Antarctica") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[21].texture);
	
	items[22] = MenuItem(strlen("Edwards Air Force Base") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 300.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[22].OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine, "data/textures/buildings/concrete.tga"));
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "Edwards Air Force Base", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("Edwards Air Force Base") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[22].texture);

	items[23] = MenuItem(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 200.0f, ANIMATION_SPIRAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false);
	items[23].OnClick = [&]() {
		items[19].enable = false;
		items[20].enable = false;
		items[21].enable = false;
		items[22].enable = false;
		items[23].enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		items[0].show();
		items[0].animate(animatePercent);
		items[1].show();
		items[1].animate(animatePercent);
		items[2].show();
		items[2].animate(animatePercent);
		items[3].show();
		items[3].animate(animatePercent);
		items[4].show();
		items[4].animate(animatePercent);
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture(strlen("BACK") * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, items[23].texture);

	animatePercent += 1;
	items[0].show();
	items[0].animate(animatePercent);

	items[1].show();
	items[1].animate(animatePercent);

	items[2].show();
	items[2].animate(animatePercent);

	items[3].show();
	items[3].animate(animatePercent);

	items[4].show();
	items[4].animate(animatePercent);

	linex = 0;
	increment = 1;

	for (MenuItem& item : items) {
		item.texture.markForDelete();
	}
}

AdrianMenu::~AdrianMenu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void AdrianMenu::fixedUpdate() {}

void AdrianMenu::update() {
	animate(m_dt);
}

void AdrianMenu::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Sprite::SwitchShader();
	Globals::textureManager.get("background").bind(0);
	m_sprite.setPosition(0.0f, 0.0f);
	m_sprite.setScale(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);

	Globals::textureManager.get("null").bind(0);
	m_sprite.setPosition(0.0f, linex * 2.0f + 200.0f - 2.5f);
	m_sprite.setScale(static_cast<float>(Application::Width), 5.0f);
	m_sprite.draw(Vector4f(10.0f, 0.0f, 0.0f, 1.0f), false, 1.0f, 1.0f);

	m_sprite.setPosition(0.0f, linex * 3.0f -2.5f);
	m_sprite.setScale(static_cast<float>(Application::Width), 5.0f);
	m_sprite.draw(Vector4f(10.0f, 0.0f, 0.0f, 1.0f), false, 1.0f, 1.0f);

	m_sprite.setPosition(linex * 3.0f -2.5f, 0.0f);
	m_sprite.setScale(5.0f, static_cast<float>(Application::Height));
	m_sprite.draw(Vector4f(10.0f, 0.0f, 0.0f, 1.0f), false, 1.0f, 1.0f);

	for (const MenuItem& item : items) {
		if (item.enable) {
			item.texture.bind();
			m_sprite.setPosition(item.x + item.offset, item.y);
			m_sprite.setScale(item.width, item.height);
			m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
		}
	}
	Sprite::SwitchShader();

}

void AdrianMenu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (MenuItem& item : items) {
		item.processInput(event.x, event.y);
	}
}

void AdrianMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (MenuItem& item : items) {
		item.processInput(event.x, event.y, event.button);
	}
}

void AdrianMenu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
#if DEVBUILD
		m_machine.addStateAtBottom(new Menu(m_machine));
#endif
	}
}

void AdrianMenu::resize(int deltaW, int deltaH) {
}

void AdrianMenu::OnReEnter(unsigned int prevState) {
	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();
}

void AdrianMenu::animate(float dt) {
	if (linex >= 400.0f)
		increment = -0.5f;
	if (linex <= 0.0f)
		increment = 0.5f;
	linex += increment * dt * 170.0f;

	if (animatePercent >= 100.0f) {
		animatePercent = 100.0f;
		return;
	}

	animatePercent += 0.5f * dt * 170.0f;
	for (MenuItem& item : items) {
		item.animate(animatePercent);
	}

}

MenuItem::MenuItem(float width, float height, float startx, float starty, float x, float y, AnimationTypeMenu animationType, float xincrement, float yincrement, float tmpx, float tmpy, float offset, bool enable) :
	width(width),
	height(height),
	startx(startx),
	starty(starty),
	x(x),
	y(y),
	animationType(animationType),
	xincrement(xincrement),
	yincrement(yincrement),
	tmpx(tmpx),
	tmpy(tmpy),
	offset(offset),
	enable(enable),
	OnClick(nullptr){
	texture.createEmptyTexture(width, height);
}

void MenuItem::show(){
	if (animationType == ANIMATION_STRAIGHT) {
		xincrement = (x - startx) / 100.0f;
		yincrement = (y - starty) / 100.0f;
	}else if (animationType == ANIMATION_SPIRAL) {
		xincrement = (x - startx) / 100.0f;
		yincrement = (y - starty) / 100.0f;
	}

	tmpx = x;
	tmpy = y;
	enable = true;
}

void MenuItem::animate(float animatePercent) {
	if (!enable)
		return;

	if (animationType == ANIMATION_STRAIGHT) {
		x = tmpx - (xincrement * (100.0f - animatePercent));
		y = tmpy - (yincrement * (100.0f - animatePercent));
	}else if (animationType == ANIMATION_SPIRAL) {
		x = tmpx -
			((xincrement * (100.0f - animatePercent))) *
			sinf(animatePercent * 3.6f * 3.142f / 180.0f) ;
		y = tmpy -
			((yincrement * (100.0 - animatePercent))) *
			cosf(animatePercent * 3.6f * 3.142f / 180.0f) ;
	}
}

void MenuItem::processInput(const int mousex, const int mousey, const Event::MouseButtonEvent::MouseButton button) {
	if (!enable)
		return;

	bool pressed = false;
	bool hover = false;
	bool wasHover = offset != 0.0f;
	if (mousex >= x && mousex <= x + width + 2.0f + offset) {
		if (Application::Height - mousey >= y && Application::Height - mousey <= y + DEFAULT_FONT_HEIGHT) {			
			hover = true;
			pressed = button == Event::MouseButtonEvent::MouseButton::BUTTON_LEFT;
		}
	}

	if (pressed && OnClick) {
		OnClick();
	}

	offset = OnClick && hover ? 20.0f : 0.0f;

	if(hover && !wasHover && OnClick)
		Globals::soundManager.get("game").play("data/wavs/ting.wav");
}