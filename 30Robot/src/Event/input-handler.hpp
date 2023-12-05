#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "EventEmitter.h"
#include "Event/Inputs/left-click-up.hpp"
#include "Event/Inputs/left-click-down.hpp"
#include "Event/Inputs/right-click-up.hpp"
#include "Event/Inputs/right-click-down.hpp"
#include "Event/Inputs/mouse-move.hpp"
#include "Event/Inputs/mouse-scrolled.hpp"

class InputHandler {
public:
	// Subscription
	void connectInputs();
	void disconnectInputs();

	// Events
	virtual void onMouseMove(const evnt::MouseMove& event);

	virtual void onLeftClickUp(const evnt::LeftClickUp& event);
	virtual void onLeftClickDown(const evnt::LeftClickDown& event);

	virtual void onRightClickUp(const evnt::RightClickUp& event);
	virtual void onRightClickDown(const evnt::RightClickDown& event);

	virtual void onMouseScrolled(const evnt::MouseScrolled& event);

protected:
	InputHandler(EventEmitter& emitter) : m_emitter(emitter), m_bConnected(false) {}
	EventEmitter& m_emitter;

private:
	bool m_bConnected;

	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickUp>> m_leftClickUp;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::LeftClickDown>> m_leftClickDown;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickUp>> m_rightClickUp;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::RightClickDown>> m_rightClickDown;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::MouseMove>> m_mouseMove;
	std::unique_ptr<entt::Emitter<EventEmitter>::Connection<evnt::MouseScrolled>> m_mouseScrolled;
};
