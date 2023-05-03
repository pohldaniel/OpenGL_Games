#pragma once

#include <queue>
#include <functional>
#include "Event.h"
#include "Mouse.h"

class MouseEventListener;
class KeyboardEventListener;

class EventDispatcher{

public:
	//void AddApplicationListener(ApplicationEventListener * el);
	void AddKeyboardListener(KeyboardEventListener * el);
	void AddMouseListener(MouseEventListener * el);

	//void RemoveApplicationListener(ApplicationEventListener * el);
	void RemoveKeyboardListener(KeyboardEventListener * el);
	void RemoveMouseListener(MouseEventListener * el);

	bool update();
	void pushEvent(const Event& event);
	void setProcessOSEvents(std::function<bool()> fun);

private:
	//std::vector<ApplicationEventListener *> mApplicationListeners;
	//std::vector<KeyboardEventListener *> mKeyboardListeners;

	std::queue<Event> m_events;
	Event m_event;

	bool popEvent(Event& event);
	bool pollEvent(Event& event);
	std::function<bool()> processOSEvents = 0;

	std::vector<MouseEventListener *> mMouseListeners;
	std::vector<KeyboardEventListener *> mKeyboardListeners;
	bool m_isRunning = true;
};