#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Mouse.h>
#include <engine/ui/Button.h>
#include <engine/ui/TextField.h>
#include <engine/Texture.h>
#include <engine/Sprite.h>

#include <States/StateMachine.h>
#include "FreetypeGL.h"

#define			DEFAULT_FONT_WIDTH				12.5f
#define			DEFAULT_FONT_HEIGHT				25.0f

enum AnimationType2 {
	ANIMATION_STRAIGHT,
	ANIMATION_SPIRAL
};

struct MenuItem {
	float width;
	float height;	
	float startx;
	float starty;
	float x;
	float y;

	AnimationType2 animationType;
	Texture texture;
	float xincrement;
	float yincrement;
	float tmpx;
	float tmpy;

	void show();
	void animate(float animatePercent);
};

class AdrianMenu : public State, public MouseEventListener, public KeyboardEventListener {

public:
	AdrianMenu(StateMachine& machine);
	~AdrianMenu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnReEnter(unsigned int prevState) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	TextField m_headline;
	std::unordered_map<std::string, Button> m_buttons;
	FontRenderer m_fontRenderer;

	Texture m_texture1, m_texture2, m_texture3, m_texture4, m_texture5;
	Sprite m_sprite;

	void animate();
	float animatePercent = 0.0f;
	int noOfMenuItems = 5;
	AnimationType2 animationType = ANIMATION_SPIRAL;

	MenuItem item1, item2, item3, item4, item5;
};