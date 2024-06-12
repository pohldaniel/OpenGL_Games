#pragma once
#include "Event.h"

class EventDispatcher;

class ApplicationEventListener {
	friend class EventDispatcher;

public:
	virtual ~ApplicationEventListener();

private:
	virtual void OnResize(Event::ApplicationEvent& event) {}

	EventDispatcher * mDispatcher = nullptr;
};