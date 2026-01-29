#pragma once
#include "Event.h"

class EventDispatcher;

class MouseEventListener{
	friend class EventDispatcher;

public:
	virtual ~MouseEventListener();

private:
	virtual void OnMouseMotion(const Event::MouseMoveEvent& event) {}
	virtual void OnMouseWheel(const Event::MouseWheelEvent& event) {}
	virtual void OnMouseButtonDown(const Event::MouseButtonEvent& event) {};
	virtual void OnMouseButtonUp(const Event::MouseButtonEvent& event) {};

	EventDispatcher * mDispatcher = nullptr;	
};