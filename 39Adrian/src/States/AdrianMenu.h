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

enum AnimationTypeMenu {
	ANIMATION_STRAIGHT,
	ANIMATION_SPIRAL
};

struct MenuItem {

	MenuItem() = default;
	MenuItem(float width, float height, float startx, float starty, float x, float y, AnimationTypeMenu animationType, float xincrement, float yincrement, float tmpx, float tmpy, float offset, bool enable);

	float width;
	float height;	
	float startx;
	float starty;
	float x;
	float y;

	AnimationTypeMenu animationType;
	
	float xincrement;
	float yincrement;
	float tmpx;
	float tmpy;
	float offset;

	
	bool enable;
	std::function<void()> OnClick;
	Texture texture;

	void show();
	void animate(float animatePercent);
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE);
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

	FontRenderer m_fontRenderer;

	Sprite m_sprite;

	void animate(float dt);
	float animatePercent = 0.0f;
	int noOfMenuItems = 5;
	AnimationTypeMenu animationType = ANIMATION_SPIRAL;
	std::vector<MenuItem> items;

	float linex;
	float increment;
};