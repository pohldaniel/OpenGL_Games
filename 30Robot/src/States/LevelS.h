#pragma once

#include <NsGui/IView.h>
#include <NsGui/FrameworkElement.h>

#include <glm/gtx/transform.hpp>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>

#include <States/StateMachine.h>
#include <entt/entt.hpp>


#include "Entity/tower-factory.hpp"
#include "Entity/mirror-factory.hpp"

#include "EventListener.h"
#include "Event/EventEmitter.h"
#include "Event/Interactions/construct-selection.hpp"
#include "Event/Interactions/delete-entity.hpp"
#include "Event/loose.hpp"
#include "Event/tower-dead.hpp"
#include "Event/wave-updated.hpp"
#include "Event/enemy-dead.hpp"
#include "Event/EventEmitter.h"
#include "Event/victory-delay-ends.hpp"
#include "Event/enemy-reached-end.hpp"

enum class LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};

class LevelS;

struct LevelConnections {
	LevelConnections(LevelS& level);

	entt::Emitter<EventEmitter>::Connection<evnt::ConstructSelection> connection1;
	entt::Emitter<EventEmitter>::Connection<evnt::DeleteEntity> connection2;
	entt::Emitter<EventEmitter>::Connection<evnt::TowerDead> connection3;
	entt::Emitter<EventEmitter>::Connection<evnt::WaveUpdated> connection4;
	entt::Emitter<EventEmitter>::Connection<evnt::EnemyDead> connection5;
	entt::Emitter<EventEmitter>::Connection<evnt::VictoryDelayEnds> connection6;
	entt::Emitter<EventEmitter>::Connection<evnt::EnemyReachedEnd> connection7;
	entt::Emitter<EventEmitter>::Connection<evnt::Loose> connection8;

	bool waveDone;
	LevelInteractionState state;
	std::uint32_t lastSelectedEntity;
};

class LevelS : public State, public MouseEventListener, public KeyboardEventListener, public EventListener {

	friend struct LevelConnections;

public:

	LevelS(StateMachine& machine);
	virtual ~LevelS();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;

	LevelInteractionState getInteractionState() const;

private:

	void OnStateChange(States states) override;
	void changeState(LevelInteractionState state);

	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;


	unsigned int m_invalidTimeCounter;
	unsigned int m_invalidTimeMax;
	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;
	LevelConnections m_connections;
};

