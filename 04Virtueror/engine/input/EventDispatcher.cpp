#include <iostream>

#include "EventDispatcher.h"
#include "MouseEventListener.h"
#include "KeyboardEventListener.h"

void EventDispatcher::AddMouseListener(MouseEventListener * el){
	// do not add NULL
	if (!el)
		return;

	auto it = std::find(mMouseListeners.begin(), mMouseListeners.end(), el);

	// listener not found -> add it
	if (mMouseListeners.end() == it){
		el->mDispatcher = this;
		mMouseListeners.emplace_back(el);
	}
}

void EventDispatcher::AddKeyboardListener(KeyboardEventListener * el) {
	// do not add NULL
	if (!el)
		return;

	auto it = std::find(mKeyboardListeners.begin(), mKeyboardListeners.end(), el);

	// listener not found -> add it
	if (mKeyboardListeners.end() == it) {
		el->mDispatcher = this;
		mKeyboardListeners.emplace_back(el);
	}
}

void EventDispatcher::RemoveMouseListener(MouseEventListener * el){
	auto it = std::find(mMouseListeners.begin(), mMouseListeners.end(), el);

	// listener found -> remove it
	if (it != mMouseListeners.end())
		mMouseListeners.erase(it);
}

void EventDispatcher::RemoveKeyboardListener(KeyboardEventListener * el) {
	auto it = std::find(mKeyboardListeners.begin(), mKeyboardListeners.end(), el);

	// listener found -> remove it
	if (it != mKeyboardListeners.end())
		mKeyboardListeners.erase(it);
}

bool EventDispatcher::update() {

	while (pollEvent(m_event)) {
		switch (m_event.type) {
			case Event::CLOSED:
				return false;
			case Event::MOUSEMOTION: {
				Mouse::instance().handleEvent(m_event);

				for (MouseEventListener * el : mMouseListeners){
					el->OnMouseMotion(m_event.mouseMove);
				}
				return true;
			}case Event::KEYDOWN: {
				for (KeyboardEventListener * el : mKeyboardListeners) {
					el->OnKeyDown(m_event.keyboard);
				}
				return true;
			}case Event::KEYUP: {
				for (KeyboardEventListener * el : mKeyboardListeners) {
					el->OnKeyUp(m_event.keyboard);
				}
				return true;
			}case Event::MOUSEBUTTONDOWN: {
				for (MouseEventListener * el : mMouseListeners) {
					el->OnMouseButtonDown(m_event.mouseButton);
				}
				return true;
			}case Event::MOUSEBUTTONUP: {
				for (MouseEventListener * el : mMouseListeners) {
					el->OnMouseButtonUp(m_event.mouseButton);
				}
				return true;
			}
		}
	}
	return true;
}

bool EventDispatcher::popEvent(Event& event) {
	if (!m_events.empty()) {
		event = m_events.front();
		m_events.pop();
		return true;
	}
	
	processOSEvents();
	return false;
}

bool EventDispatcher::pollEvent(Event& event) {	
	return popEvent(event);
}

void EventDispatcher::pushEvent(const Event& event) {
	m_events.push(event);
}

void EventDispatcher::setProcessOSEvents(std::function<void()> fun) {
	processOSEvents = fun;
}