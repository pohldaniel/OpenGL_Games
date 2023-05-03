#pragma once
#include "Event.h"

class EventDispatcher;

class KeyboardEventListener {
public:
	virtual ~KeyboardEventListener();

private:
	virtual void OnKeyDown(Event::KeyboardEvent& event) {}
	virtual void OnKeyUp(Event::KeyboardEvent& event) {}

private:
	EventDispatcher * mDispatcher = nullptr;

	// only EventDispatcher can generate events
	friend class EventDispatcher;
};