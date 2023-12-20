#pragma once

#include <NsGui/IView.h>
#include <NsGui/FrameworkElement.h>

#include <entt/entt.hpp>
#include <glm/gtx/transform.hpp>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <Event/EventListener.h>
#include <Event/EventEmitter.h>

#include <EntityFactories/TowerFactory.h>
#include <EntityFactories/MirrorFactory.h>

enum class LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};

class LevelState;

struct LevelConnections {
	LevelConnections(LevelState& level);

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

class LevelState : public State, public MouseEventListener, public KeyboardEventListener, public EventListener {

	friend struct LevelConnections;

public:

	LevelState(StateMachine& machine);
	virtual ~LevelState();

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

	Noesis::IView* m_ui;

	unsigned int m_invalidTimeCounter;
	unsigned int m_invalidTimeMax;
	TowerFactory m_towerFactory;
	MirrorFactory m_mirrorFactory;
	LevelConnections m_connections;
	bool m_debugDraw;
};