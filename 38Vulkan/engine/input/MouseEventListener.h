#pragma once
#include "Event.h"

class EventDispatcher;

class MouseEventListener{
	friend class EventDispatcher;

public:
	virtual ~MouseEventListener();

private:
	virtual void OnMouseMotion(Event::MouseMoveEvent& event) {}
	virtual void OnMouseWheel(Event::MouseWheelEvent& event) {}
	virtual void OnMouseButtonDown(Event::MouseButtonEvent& event) {};
	virtual void OnMouseButtonUp(Event::MouseButtonEvent& event) {};

	EventDispatcher * mDispatcher = nullptr;	
};