#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/Texture.h>
#include <States/StateMachine.h>

#include <Entities/SpriteEntity.h>
#include <Entities/Player.h>
#include <Entities/Character.h>
#include "Zone.h"
#include "Dialog.h"


struct DialogNew {
	std::vector<std::string> undefeated;
	std::vector<std::string> defeated;
};

struct Monster {
	std::string name;
	int level;
};

struct Trainer {
	std::vector<Monster> monsters;
	DialogNew dialog;
	std::vector<ViewDirection> viewDirections;
	bool lookAround;
	bool defeated;
	std::string binom;
};

class MonsterHunter : public State, public MouseEventListener, public KeyboardEventListener {

public:

	MonsterHunter(StateMachine& machine);
	~MonsterHunter();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();
	
	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawCenter = false;
	bool m_useCulling = true;
	bool m_drawScreenBorder = false;
	bool m_debugCollision = false;
	Camera m_camera;

	unsigned int m_atlasWorld;
	float m_movingSpeed;
	float m_viewWidth;
	float m_viewHeight;
	float m_mapHeight;
	float m_screeBorder;

	Zone m_zone;
	std::unordered_map<std::string, Trainer> m_trainers;
};