#pragma once

#include <NsGui/IntegrationAPI.h>
#include <NsGui/IView.h>

#include <glm/gtx/transform.hpp>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>

#include <States/StateMachine.h>

#include "EventListener.h"
#include "GUI/level-hud.hpp"
#include "Entity/tower-factory.hpp"
#include "Entity/mirror-factory.hpp"
#include "Event/Interactions/construct-selection.hpp"

enum class LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};

class LevelS : public State, public MouseEventListener, public KeyboardEventListener, public EventListener {
public:
	LevelS(StateMachine& machine);
	virtual ~LevelS();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;

	// Events
	//void onLeftClickUp(const evnt::LeftClickUp& event) override;
	//void onLeftClickDown(const evnt::LeftClickDown& event) override;
	//void onRightClickDown(const evnt::RightClickDown& event) override;
	//void onMouseMove(const evnt::MouseMove& event) override;
	//void onMouseScrolled(const evnt::MouseScrolled& event) override;

	// Getters
	LevelInteractionState getInteractionState() const;

	// Setters
	void changeState(LevelInteractionState state);

	void OnStateChange(States states) override;

private:
	void handleVictoryConditions();
	void handleConstructions();

private:
	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
	LevelHud m_levelHud;
	LevelInteractionState m_state;

	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;

	unsigned int m_invalidTimeCounter;
	unsigned int m_invalidTimeMax;
	ConstructibleType m_constructType;
	std::uint32_t m_lastSelectedEntity;
	bool m_bWaveDone;
};