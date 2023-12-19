#pragma once

#include <NsGui/IView.h>
#include <NsGui/FrameworkElement.h>
#include <NsGui/Grid.h>

#include <glm/gtx/transform.hpp>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>

#include <States/StateMachine.h>

#include <Event/EventEmitter.h>
#include <Event/EventListener.h>
#include <Level/Level.h>


struct TitleScreenConnections {
	TitleScreenConnections();	
};

class TitleScreenS : public State, public MouseEventListener, public KeyboardEventListener, public EventListener {

public:

	TitleScreenS(StateMachine& machine);
	~TitleScreenS();

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
	void OnApplicationQuit() override;

private:

	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;

	Noesis::Ptr<Noesis::Grid> xaml;

	static TitleScreenConnections TitleScreenSConnections;
};

