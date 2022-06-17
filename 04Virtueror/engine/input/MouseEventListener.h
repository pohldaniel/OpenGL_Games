#pragma once
#include "Event.h"

class EventDispatcher;

class MouseEventListener{
public:
	virtual ~MouseEventListener();

private:
	virtual void OnMouseMotion(Event::MouseMoveEvent& event) {}
	virtual void OnMouseButtonDown(Event::MouseButtonEvent& event) {};
	virtual void OnMouseButtonUp(Event::MouseButtonEvent& event) {};
private:
	EventDispatcher * mDispatcher = nullptr;

	// only EventDispatcher can generate events
	friend class EventDispatcher;
};