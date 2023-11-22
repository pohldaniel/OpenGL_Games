#include <iostream>

#include "EventDispatcher.h"
#include "ApplicationEventListener.h"
#include "MouseEventListener.h"
#include "KeyboardEventListener.h"

EventDispatcher EventDispatcher::s_instance;

EventDispatcher& EventDispatcher::Get() {
	return s_instance;
}

void EventDispatcher::AddApplicationListener(ApplicationEventListener * el) {
	// do not add NULL
	if (!el)
		return;

	auto it = std::find(s_instance.mApplicationListeners.begin(), s_instance.mApplicationListeners.end(), el);

	// listener not found -> add it
	if (s_instance.mApplicationListeners.end() == it) {
		el->mDispatcher = &s_instance;
		s_instance.mApplicationListeners.emplace_back(el);
	}
}

void EventDispatcher::AddMouseListener(MouseEventListener * el){
	if (!el)
		return;

	auto it = std::find(s_instance.mMouseListeners.begin(), s_instance.mMouseListeners.end(), el);

	if (s_instance.mMouseListeners.end() == it){
		el->mDispatcher = &s_instance;
		s_instance.mMouseListeners.emplace_back(el);
	}
}

void EventDispatcher::AddKeyboardListener(KeyboardEventListener * el) {
	if (!el)
		return;

	auto it = std::find(s_instance.mKeyboardListeners.begin(), s_instance.mKeyboardListeners.end(), el);

	if (s_instance.mKeyboardListeners.end() == it) {
		el->mDispatcher = &s_instance;
		s_instance.mKeyboardListeners.emplace_back(el);
	}
}

void EventDispatcher::RemoveApplicationListener(ApplicationEventListener * el) {
	auto it = std::find(s_instance.mApplicationListeners.begin(), s_instance.mApplicationListeners.end(), el);

	// listener found -> remove it
	if (it != s_instance.mApplicationListeners.end())
		s_instance.mApplicationListeners.erase(it);
}

void EventDispatcher::RemoveMouseListener(MouseEventListener * el){
	auto it = std::find(s_instance.mMouseListeners.begin(), s_instance.mMouseListeners.end(), el);

	if (it != s_instance.mMouseListeners.end())
		s_instance.mMouseListeners.erase(it);
}

void EventDispatcher::RemoveKeyboardListener(KeyboardEventListener * el) {
	auto it = std::find(s_instance.mKeyboardListeners.begin(), s_instance.mKeyboardListeners.end(), el);

	if (it != s_instance.mKeyboardListeners.end())
		s_instance.mKeyboardListeners.erase(it);
}

bool EventDispatcher::update() {

	while (pollEvent(m_event)) {
		switch (m_event.type) {
			case Event::CLOSED:
				return false;
			case Event::MOUSEMOTION: {
				for (MouseEventListener * el : mMouseListeners){
					el->OnMouseMotion(m_event.data.mouseMove);
				}
				return true;
			}case Event::KEYDOWN: {
				for (KeyboardEventListener * el : mKeyboardListeners) {
					el->OnKeyDown(m_event.data.keyboard);
				}
				return true;
			}case Event::KEYUP: {
				for (KeyboardEventListener * el : mKeyboardListeners) {
					el->OnKeyUp(m_event.data.keyboard);
				}
				return true;
			}case Event::MOUSEBUTTONDOWN: {
				for (MouseEventListener * el : mMouseListeners) {
					el->OnMouseButtonDown(m_event.data.mouseButton);
				}
				return true;
			}case Event::MOUSEBUTTONUP: {
				for (MouseEventListener * el : mMouseListeners) {
					el->OnMouseButtonUp(m_event.data.mouseButton);
				}
				return true;
			}case Event::RESIZE: {
				for (ApplicationEventListener * el : mApplicationListeners) {
					el->OnResize(m_event.data.application);
				}
				return true;
			}
		}
	}
	return m_isRunning;
}

bool EventDispatcher::popEvent(Event& event) {
	
	if (!m_events.empty()) {
		event = m_events.front();
		m_events.pop();
		return true;
	}
	
	m_isRunning = processOSEvents();
	return false;
}

bool EventDispatcher::pollEvent(Event& event) {	
	return popEvent(event);
}

void EventDispatcher::pushEvent(const Event& event) {
	m_events.push(event);
}

void EventDispatcher::setProcessOSEvents(std::function<bool()> fun) {
	processOSEvents = fun;
}