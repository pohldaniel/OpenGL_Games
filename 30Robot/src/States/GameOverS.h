#pragma once
#include <glm/gtx/transform.hpp>

#include <NsGui/IntegrationAPI.h>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>

#include <States/StateMachine.h>

#include <Event/EventEmitter.h>
#include <Event/EventListener.h>

#include <Level/Level.h>

class GameOverS : public State, public MouseEventListener, public KeyboardEventListener, public EventListener {

public:

	GameOverS(StateMachine& machine);
	~GameOverS();

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

	void OnStateChange(States states) override;

private:

	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;
};

