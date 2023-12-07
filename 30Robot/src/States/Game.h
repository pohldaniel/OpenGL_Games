#pragma once

#include <NsGui/IView.h>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>
#include <States/i-game-state.hpp>
#include "Background.h"

#include <Event/EventEmitter.h>

#include <System/AttackSystem.h>
#include <System/render-system.hpp>
#include <System/movement-system.hpp>
#include <System/animation-system.hpp>
#include <System/wave-system.hpp>
#include <System/life-and-death-system.hpp>

#include <Level/level.hpp>

#include "GUI/title-screen.hpp"

class Game : public State, public IGameState,  public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void enter() override;
	void exit() override;

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

	static EventEmitter Emitter;

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Background m_background;

	entt::DefaultRegistry registry;

	RenderSystem* renderSystem;
	AnimationSystem* animationSystem;
	MovementSystem* movementSystem;
	WaveSystem* waveSystem;
	AttackSystem* attackSystem;
	LifeAndDeathSystem* lifeAndDeathSystem;
	
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	TitleScreen m_titleScreen;
	Progression progression;

	Level* level;
	glm::mat4 m_projMat;
	glm::mat4 m_viewMat;
	glm::vec2 m_viewTranslation;
	float m_viewScale;
};

