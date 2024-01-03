#pragma once

#include <engine/input/Mouse.h>
#include <ECS/ECS.h>
#include <EventBus/Event.h>

class MouseClickedEvent : public EventNew {

public:
	Mouse::MouseButton symbol;
	int x;
	int y;
	MouseClickedEvent(Mouse::MouseButton symbol, int x, int y) : symbol(symbol), x(x), y(y) {}
};
