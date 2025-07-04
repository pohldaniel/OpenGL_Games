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

	//m_fontRenderer.InitFontRenderer();

	m_texture1.createEmptyTexture((strlen("PLAY GAME")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture2.createEmptyTexture((strlen("SETTINGS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture3.createEmptyTexture((strlen("OPTIONS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture4.createEmptyTexture((strlen("CREDITS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);
	m_texture5.createEmptyTexture((strlen("QUIT")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT);

	Globals::fontManager.get("tahomab_64").bind();
	Fontrenderer::Get().setShader(Globals::shaderManager.getAssetPointer("font_ttf"));
	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "PLAY GAME", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("PLAY GAME")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture1);

	item1 = { (strlen("PLAY GAME")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 600.0f, 400.0f, 600.0f, ANIMATION_SPIRAL, m_texture1 , 0.0f, 0.0f, 0.0f, 0.0f };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "SETTINGS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("SETTINGS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture2);

	item2 = { (strlen("SETTINGS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 900.0f, 500.0f, 400.0f, 500.0f, ANIMATION_SPIRAL, m_texture2 , 0.0f, 0.0f, 0.0f, 0.0f };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "OPTIONS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("OPTIONS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture3);

	item3 = { (strlen("OPTIONS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 0.0f, 0.0f, 400.0f, 400.0f, ANIMATION_SPIRAL, m_texture3 , 0.0f, 0.0f, 0.0f, 0.0f };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "CREDITS", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("CREDITS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture4);

	item4 = { (strlen("CREDITS")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 0.0f, 400.0f, 300.0f, ANIMATION_SPIRAL, m_texture4 , 0.0f, 0.0f, 0.0f, 0.0f };

	Fontrenderer::Get().addText(Globals::fontManager.get("tahomab_64"), 0.0f, 0.0f, "QUIT", Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, false);
	Fontrenderer::Get().blitTextToTexture((strlen("QUIT")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 20, 20, m_texture5);

	item5 = { (strlen("QUIT")) * DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, 400.0f, 700.0f, 400.0f, 200.0f, ANIMATION_SPIRAL, m_texture5 , 0.0f, 0.0f, 0.0f, 0.0f };

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

	item1.texture.bind();
	m_sprite.setPosition(item1.x + 20.0f, item1.y);
	m_sprite.setScale(item1.width, item1.height);
	m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);

	item2.texture.bind();
	m_sprite.setPosition(item2.x, item2.y);
	m_sprite.setScale(item2.width, item2.height);
	m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);

	item3.texture.bind();
	m_sprite.setPosition(item3.x, item3.y);
	m_sprite.setScale(item3.width, item3.height);
	m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);

	item4.texture.bind();
	m_sprite.setPosition(item4.x, item4.y);
	m_sprite.setScale(item4.width, item4.height);
	m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);

	item5.texture.bind();
	m_sprite.setPosition(item5.x, item5.y);
	m_sprite.setScale(item5.width, item5.height);
	m_sprite.draw(Vector4f::ONE, false, 1.0f, 1.0f);

	Sprite::SwitchShader();

}

void AdrianMenu::OnMouseMotion(Event::MouseMoveEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y);
}

void AdrianMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	for (auto&& b : m_buttons)
		b.second.processInput(event.x, Application::Height - event.y, event.button);
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
	if (animatePercent >= 100) {
		animatePercent = 100;
		return;
	}

	animatePercent += 0.25f;
	item1.animate(animatePercent);
	item2.animate(animatePercent);
	item3.animate(animatePercent);
	item4.animate(animatePercent);
	item5.animate(animatePercent);
}

void MenuItem::show(){
	if (animationType == ANIMATION_STRAIGHT) {
		xincrement = (x - startx) / 100.0;
		yincrement = (y - starty) / 100.0;
	}
	else if (animationType == ANIMATION_SPIRAL) {
		xincrement = (x - startx) / 100.0;
		yincrement = (y - starty) / 100.0;
	}

	tmpx = x;
	tmpy = y;
}


void MenuItem::animate(float animatePercent) {

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