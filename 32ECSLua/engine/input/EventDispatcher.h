#pragma once

#include <queue>
#include <functional>
#include "Event.h"
#include "Mouse.h"

class ApplicationEventListener;
class MouseEventListener;
class KeyboardEventListener;

class EventDispatcher {

public:

	static void AddApplicationListener(ApplicationEventListener * el);
	static void AddKeyboardListener(KeyboardEventListener * el);
	static void AddMouseListener(MouseEventListener * el);

	static void RemoveApplicationListener(ApplicationEventListener * el);
	static void RemoveKeyboardListener(KeyboardEventListener * el);
	static void RemoveMouseListener(MouseEventListener * el);

	bool update();
	void pushEvent(const Event& event);
	void setProcessOSEvents(std::function<bool()> fun);

	static EventDispatcher& Get();

private:
	EventDispatcher() = default;

	std::queue<Event> m_events;
	Event m_event;

	bool popEvent(Event& event);
	bool pollEvent(Event& event);
	std::function<bool()> processOSEvents = 0;

	std::vector<ApplicationEventListener *> mApplicationListeners;
	std::vector<MouseEventListener *> mMouseListeners;
	std::vector<KeyboardEventListener *> mKeyboardListeners;
	
	bool m_isRunning = true;

	static EventDispatcher s_instance;
};