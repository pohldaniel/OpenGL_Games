#pragma once
#include "Event.h"

class EventDispatcher;

class KeyboardEventListener {
	friend class EventDispatcher;

public:
	virtual ~KeyboardEventListener();

private:
	virtual void OnKeyDown(Event::KeyboardEvent& event) {}
	virtual void OnKeyUp(Event::KeyboardEvent& event) {}

	EventDispatcher * mDispatcher = nullptr;
};