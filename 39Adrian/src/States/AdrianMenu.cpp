#include <engine/Fontrenderer.h>

#include <States/Menu.h>
#include <States/Adrian.h>

#include "AdrianMenu.h"
#include "Application.h"
#include "Globals.h"



AdrianMenu::AdrianMenu(StateMachine& machine) : State(machine, States::ADRIAN_MENU) {

	EventDispatcher::AddMouseListener(this);
	EventDispatcher::AddKeyboardListener(this);

	m_headline.setCharset(Globals::fontManager.get("upheaval_100"));
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 320), static_cast<float>(Application::Height - 200)));
	m_headline.setOutlineThickness(5.0f);
	m_headline.setText("Adrian Menu");
	m_headline.setOffset(5.0f, -7.0f);

	m_buttons = std::initializer_list<std::pair<const std::string, Button>>({
		{ "adrian",            Button() }
	});

	m_buttons.at("adrian").setCharset(Globals::fontManager.get("upheaval_30"));
	m_buttons.at("adrian").setPosition(50.0f, 100.0f);
	m_buttons.at("adrian").setOutlineThickness(5.0f);
	m_buttons.at("adrian").setText("Adrian");
	m_buttons.at("adrian").setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine));
	});

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	m_texture1.createEmptyTexture((strlen("PLAY GAME")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture2.createEmptyTexture((strlen("SETTINGS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture3.createEmptyTexture((strlen("OPTIONS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture4.createEmptyTexture((strlen("CREDITS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture5.createEmptyTexture((strlen("QUIT")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);

	m_texture6.createEmptyTexture((strlen("VIDEO")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture7.createEmptyTexture((strlen("SOUND")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture8.createEmptyTexture((strlen("KEYBOARD")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture9.createEmptyTexture((strlen("MOUSE")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture10.createEmptyTexture((strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);

	m_texture11.createEmptyTexture((strlen("BHANU KALYAN")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture12.createEmptyTexture((strlen("VAMSI KRISHNA")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture13.createEmptyTexture((strlen("SWAMY SUMAN")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture14.createEmptyTexture((strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);

	m_texture15.createEmptyTexture((strlen("640 X 480")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture16.createEmptyTexture((strlen("800 X 600")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture17.createEmptyTexture((strlen("1024 X 768")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture17.createEmptyTexture((strlen("1600 X 1200")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture19.createEmptyTexture((strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);

	Globals::fontManager.get("tahomab_64").bind();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font_ttf"));
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "PLAY GAME", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("PLAY GAME")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture1);
	item1 = { (strlen("PLAY GAME")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 600.0f, 400.0f, 600.0f, ANIMATION_SPIRAL, m_texture1 , 0.0f, 0.0f, 0.0f, 0.0f, 20.0f, nullptr, true };
	item1.OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Adrian(m_machine));
	};

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SETTINGS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("SETTINGS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture2);
	item2 = { (strlen("SETTINGS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 900.0f, 500.0f, 400.0f, 500.0f, ANIMATION_SPIRAL, m_texture2 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, true };
	item2.OnClick = [&]() {
		item1.enable = false;
		item2.enable = false;
		item3.enable = false;
		item4.enable = false;
		item5.enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		item6.show();
		item6.animate(animatePercent);
		item7.show();
		item7.animate(animatePercent);	
		item8.show();
		item8.animate(animatePercent);
		item9.show();
		item9.animate(animatePercent);	
		item10.show();
		item10.animate(animatePercent);
	};

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "OPTIONS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("OPTIONS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture3);
	item3 = { (strlen("OPTIONS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 400.0f, ANIMATION_SPIRAL, m_texture3 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, true };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "CREDITS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("CREDITS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture4);
	item4 = { (strlen("CREDITS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 0.0f, 400.0f, 300.0f, ANIMATION_SPIRAL, m_texture4 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, true };
	item4.OnClick = [&]() {
		item1.enable = false;
		item2.enable = false;
		item3.enable = false;
		item4.enable = false;
		item5.enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		item11.show();
		item11.animate(animatePercent);
		item12.show();
		item12.animate(animatePercent);		
		item13.show();
		item13.animate(animatePercent);
		item14.show();
		item14.animate(animatePercent);
		
	};

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "QUIT", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("QUIT")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture5);
	item5 = { (strlen("QUIT")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 700.0f, 400.0f, 200.0f, ANIMATION_SPIRAL, m_texture5 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, true };
	item5.OnClick = [&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	};
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "VIDEO", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("VIDEO")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture6);
	item6 = { (strlen("VIDEO")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 600.0f, ANIMATION_STRAIGHT, m_texture6 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };
	item6.OnClick = [&]() {
		item6.enable = false;
		item7.enable = false;
		item8.enable = false;
		item9.enable = false;
		item10.enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		item15.show();
		item15.animate(animatePercent);
		item16.show();
		item16.animate(animatePercent);
		item17.show();
		item17.animate(animatePercent);
		item18.show();
		item18.animate(animatePercent);
		item19.show();
		item19.animate(animatePercent);
	};

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SOUND", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("SOUND")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture7);
	item7 = { (strlen("SOUND")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 1000.0f, 500.0f, 400.0f, 500.0f, ANIMATION_STRAIGHT, m_texture7 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "KEYBOARD", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("KEYBOARD")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture8);
	item8 = { (strlen("KEYBOARD")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 400.0f, ANIMATION_STRAIGHT, m_texture8 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "MOUSE", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("MOUSE")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture9);
	item9 = { (strlen("MOUSE")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 300.0f, ANIMATION_STRAIGHT, m_texture9 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture10);
	item10 = { (strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 0.0f, 400.0f, 200.0f, ANIMATION_STRAIGHT, m_texture10 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };
	item10.OnClick = [&]() {
		item6.enable = false;
		item7.enable = false;
		item8.enable = false;
		item9.enable = false;
		item10.enable = false;
		
		animatePercent = 0.0f;
		animatePercent += 1;
		item1.show();
		item1.animate(animatePercent);
		item2.show();
		item2.animate(animatePercent);
		item3.show();
		item3.animate(animatePercent);
		item4.show();
		item4.animate(animatePercent);
		item5.show();
		item5.animate(animatePercent);
	};

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BHANU KALYAN", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("BHANU KALYAN")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture11);
	item11 = { (strlen("BHANU KALYAN")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 600.0f, ANIMATION_SPIRAL, m_texture11 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "VAMSI KRISHNA", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("VAMSI KRISHNA")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture12);
	item12 = { (strlen("VAMSI KRISHNA")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 500.0f, ANIMATION_SPIRAL, m_texture12 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SWAMY SUMAN", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("SWAMY SUMAN")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture13);
	item13 = { (strlen("SWAMY SUMAN")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 400.0f, ANIMATION_SPIRAL, m_texture13 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture14);
	item14 = { (strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 300.0f, ANIMATION_SPIRAL, m_texture14 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };
	item14.OnClick = [&]() {
		item11.enable = false;
		item12.enable = false;
		item13.enable = false;
		item14.enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		item1.show();
		item1.animate(animatePercent);
		item2.show();
		item2.animate(animatePercent);
		item3.show();
		item3.animate(animatePercent);
		item4.show();
		item4.animate(animatePercent);
		item5.show();
		item5.animate(animatePercent);
	};

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "640 X 480", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("640 X 480")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture15);
	item15 = { (strlen("640 X 480")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 600.0f, ANIMATION_STRAIGHT, m_texture15 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "800 X 600", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("800 X 600")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture16);
	item16 = { (strlen("800 X 600")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 500.0f, ANIMATION_STRAIGHT, m_texture16 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "1024 X 768", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("1024 X 768")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture17);
	item17 = { (strlen("1024 X 768")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 400.0f, ANIMATION_STRAIGHT, m_texture17 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "1600 X 1200", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("1600 X 1200")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture18);
	item18 = { (strlen("1600 X 1200")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 300.0f, ANIMATION_STRAIGHT, m_texture18 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "BACK", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture19);
	item19 = { (strlen("BACK")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 500.0f, 400.0f, 200.0f, ANIMATION_STRAIGHT, m_texture19 , 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, nullptr, false };
	item19.OnClick = [&]() {
		item15.enable = false;
		item16.enable = false;
		item17.enable = false;
		item18.enable = false;
		item19.enable = false;

		animatePercent = 0.0f;
		animatePercent += 1;
		item6.show();
		item6.animate(animatePercent);
		item7.show();
		item7.animate(animatePercent);
		item8.show();
		item8.animate(animatePercent);
		item9.show();
		item9.animate(animatePercent);
		item10.show();
		item10.animate(animatePercent);
	};

	animatePercent += 1;
	item1.show();
	item1.animate(animatePercent);

	item2.show();
	item2.animate(animatePercent);

	item3.show();
	item3.animate(animatePercent);

	item4.show();
	item4.animate(animatePercent);

	item5.show();
	item5.animate(animatePercent);

	linex = 0;
	increment = 1;
}

AdrianMenu::~AdrianMenu() {
	EventDispatcher::RemoveMouseListener(this);
	EventDispatcher::RemoveKeyboardListener(this);
}

void AdrianMenu::fixedUpdate() {}

void AdrianMenu::update() {
	m_headline.processInput(0, 0);
	animate();
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

	if (item1.enable) {
		item1.texture.bind();
		m_sprite.setPosition(item1.x + item1.offset, item1.y);
		m_sprite.setScale(item1.width, item1.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item2.enable) {
		item2.texture.bind();
		m_sprite.setPosition(item2.x + item2.offset, item2.y);
		m_sprite.setScale(item2.width, item2.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item3.enable) {
		item3.texture.bind();
		m_sprite.setPosition(item3.x + item3.offset, item3.y);
		m_sprite.setScale(item3.width, item3.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item4.enable) {
		item4.texture.bind();
		m_sprite.setPosition(item4.x + item4.offset, item4.y);
		m_sprite.setScale(item4.width, item4.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item5.enable) {
		item5.texture.bind();
		m_sprite.setPosition(item5.x + item5.offset, item5.y);
		m_sprite.setScale(item5.width, item5.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item6.enable) {
		item6.texture.bind();
		m_sprite.setPosition(item6.x + item6.offset, item6.y);
		m_sprite.setScale(item6.width, item6.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item7.enable) {
		item7.texture.bind();
		m_sprite.setPosition(item7.x + item7.offset, item7.y);
		m_sprite.setScale(item7.width, item7.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item8.enable) {
		item8.texture.bind();
		m_sprite.setPosition(item8.x + item8.offset, item8.y);
		m_sprite.setScale(item8.width, item8.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item9.enable) {
		item9.texture.bind();
		m_sprite.setPosition(item9.x + item9.offset, item9.y);
		m_sprite.setScale(item9.width, item9.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item10.enable) {
		item10.texture.bind();
		m_sprite.setPosition(item10.x + item10.offset, item10.y);
		m_sprite.setScale(item10.width, item10.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item11.enable) {
		item11.texture.bind();
		m_sprite.setPosition(item11.x + item11.offset, item11.y);
		m_sprite.setScale(item11.width, item11.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item12.enable) {
		item12.texture.bind();
		m_sprite.setPosition(item12.x + item12.offset, item12.y);
		m_sprite.setScale(item12.width, item12.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item13.enable) {
		item13.texture.bind();
		m_sprite.setPosition(item13.x + item13.offset, item13.y);
		m_sprite.setScale(item13.width, item13.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item14.enable) {
		item14.texture.bind();
		m_sprite.setPosition(item14.x + item14.offset, item14.y);
		m_sprite.setScale(item14.width, item14.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item15.enable) {
		item15.texture.bind();
		m_sprite.setPosition(item15.x + item15.offset, item15.y);
		m_sprite.setScale(item15.width, item15.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item16.enable) {
		item16.texture.bind();
		m_sprite.setPosition(item16.x + item16.offset, item16.y);
		m_sprite.setScale(item16.width, item16.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item17.enable) {
		item17.texture.bind();
		m_sprite.setPosition(item17.x + item17.offset, item17.y);
		m_sprite.setScale(item17.width, item17.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item18.enable) {
		item18.texture.bind();
		m_sprite.setPosition(item18.x + item18.offset, item18.y);
		m_sprite.setScale(item18.width, item18.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	if (item19.enable) {
		item19.texture.bind();
		m_sprite.setPosition(item19.x + item19.offset, item19.y);
		m_sprite.setScale(item19.width, item19.height);
		m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);
	}

	Sprite::SwitchShader();

}

void AdrianMenu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y);

	item1.processInput(event.x, event.y);
	item2.processInput(event.x, event.y);
	item3.processInput(event.x, event.y);
	item4.processInput(event.x, event.y);
	item5.processInput(event.x, event.y);

	item6.processInput(event.x, event.y);
	item7.processInput(event.x, event.y);
	item8.processInput(event.x, event.y);
	item9.processInput(event.x, event.y);
	item10.processInput(event.x, event.y);

	item11.processInput(event.x, event.y);
	item12.processInput(event.x, event.y);
	item13.processInput(event.x, event.y);
	item14.processInput(event.x, event.y);

	item15.processInput(event.x, event.y);
	item16.processInput(event.x, event.y);
	item17.processInput(event.x, event.y);
	item18.processInput(event.x, event.y);
	item19.processInput(event.x, event.y);
}

void AdrianMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y, event.button);

	item1.processInput(event.x, event.y, event.button);
	item2.processInput(event.x, event.y, event.button);
	item3.processInput(event.x, event.y, event.button);
	item4.processInput(event.x, event.y, event.button);
	item14.processInput(event.x, event.y, event.button);
	item5.processInput(event.x, event.y, event.button);

	item6.processInput(event.x, event.y, event.button);
	item7.processInput(event.x, event.y, event.button);
	item8.processInput(event.x, event.y, event.button);
	item9.processInput(event.x, event.y, event.button);
	item10.processInput(event.x, event.y, event.button);

	item11.processInput(event.x, event.y, event.button);
	item12.processInput(event.x, event.y, event.button);
	item13.processInput(event.x, event.y, event.button);
	

	item15.processInput(event.x, event.y, event.button);
	item16.processInput(event.x, event.y, event.button);
	item17.processInput(event.x, event.y, event.button);
	item18.processInput(event.x, event.y, event.button);
	item19.processInput(event.x, event.y, event.button);
}

void AdrianMenu::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void AdrianMenu::resize(int deltaW, int deltaH) {
	m_headline.setPosition(Vector2f(static_cast<float>(Application::Width / 2 - 220), static_cast<float>(Application::Height - 200)));
}

void AdrianMenu::OnReEnter(unsigned int prevState) {
	auto shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f));
	shader->unuse();
}

void AdrianMenu::animate() {
	if (linex >= 400.0f)
		increment = -0.5f;
	if (linex <= 0.0f)
		increment = 0.5f;
	linex += increment;

	if (animatePercent >= 100.0f) {
		animatePercent = 100.0f;
		return;
	}

	animatePercent += 0.25f;
	item1.animate(animatePercent);
	item2.animate(animatePercent);
	item3.animate(animatePercent);
	item4.animate(animatePercent);
	item5.animate(animatePercent);

	item6.animate(animatePercent);
	item7.animate(animatePercent);
	item8.animate(animatePercent);
	item9.animate(animatePercent);
	item10.animate(animatePercent);

	item11.animate(animatePercent);
	item12.animate(animatePercent);
	item13.animate(animatePercent);
	item14.animate(animatePercent);

	item15.animate(animatePercent);
	item16.animate(animatePercent);
	item17.animate(animatePercent);
	item18.animate(animatePercent);
	item19.animate(animatePercent);
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
			sinf(animatePercent * 3.6f * 3.142f / 180.0f);
		y = tmpy -
			((yincrement * (100.0 - animatePercent))) *
			cosf(animatePercent * 3.6f * 3.142f / 180.0f);
	}
}

void MenuItem::processInput(const int mousex, const int mousey, const Event::MouseButtonEvent::MouseButton button) {
	if (!enable)
		return;

	bool pressed = false;
	bool hover = false;
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
}