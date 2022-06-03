#pragma once
#include "Event.h"

class EventDispatcher;

class MouseEventListener{
public:
	virtual ~MouseEventListener();

private:
	virtual void OnMouseMotion(Event::MouseMoveEvent& event) {}

private:
	EventDispatcher * mDispatcher = nullptr;

	// only EventDispatcher can generate events
	friend class EventDispatcher;
};