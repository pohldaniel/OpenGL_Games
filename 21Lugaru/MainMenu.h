#pragma once
#include "engine/input/EventDispatcher.h"
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "StateMachine.h"

#include "Graphic/TextureLu.hpp"

struct MenuItem {

	enum MenuItemType {
		NONE,
		LABEL,
		BUTTON,
		IMAGE,
		IMAGEBUTTON,
		MAPMARKER,
		MAPLINE,
		MAPLABEL
	} type;

	int id;
	string text;
	TextureLu texture;
	int x, y, w, h;
	float r, g, b;
	float effectfade;

	float linestartsize;
	float lineendsize;

	MenuItem(MenuItemType _type, int _id, const string& _text, TextureLu _texture,
		int _x, int _y, int _w, int _h, float _r, float _g, float _b,
		float _linestartsize = 1, float _lineendsize = 1);
};

class MainMenu : public State, public MouseEventListener {

public:
	MainMenu(StateMachine& machine);
	~MainMenu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void processInput();

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void drawMenu();
	void drawItems();
	void handleFadeEffect(float delta);
	void Load();

	void clearMenu();
	void addImage(int id, TextureLu texture, int x, int y, int w, int h, float r = 1, float g = 1, float b = 1);
	void addButtonImage(int id, TextureLu texture, int x, int y, int w, int h, float r = 1, float g = 1, float b = 1);
	void addLabel(int id, const string& text, int x, int y, float r = 1, float g = 0, float b = 0);
	void addButton(int id, const string& text, int x, int y, float r = 1, float g = 0, float b = 0);	
	void setText(int id, const string& text);
	void setText(int id, const string& text, int x, int y, int w, int h);
	void updateSettingsMenu();
	void updateStereoConfigMenu();

	static std::vector<MenuItem> items;

	const std::string VERSION_NUMBER = "1.3";
	const std::string VERSION_SUFFIX = "-dev";

};