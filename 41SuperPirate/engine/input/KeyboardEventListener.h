#pragma once
#include "Event.h"

class EventDispatcher;

class KeyboardEventListener {
	friend class EventDispatcher;

public:
	virtual ~KeyboardEventListener();

private:
	virtual void OnKeyDown(const Event::KeyboardEvent& event) {}
	virtual void OnKeyUp(const Event::KeyboardEvent& event) {}

	EventDispatcher * mDispatcher = nullptr;
};