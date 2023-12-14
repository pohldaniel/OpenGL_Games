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
#include "Event/EventEmitter.h"

enum class LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};



class LevelS : public State, public MouseEventListener, public KeyboardEventListener, public EventListener {
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

	// Getters
	LevelInteractionState getInteractionState() const;

	// Setters
	static void ChangeState(LevelInteractionState state);

	void OnStateChange(States states) override;

private:

	void handleVictoryConditions();
	void handleConstructions();

	Noesis::Ptr<Noesis::FrameworkElement> m_xaml;
	Noesis::IView* m_ui;


	unsigned int m_invalidTimeCounter;
	unsigned int m_invalidTimeMax;


	bool m_bWaveDone;

	static bool Init;
	static bool WaveDone;
	static TowerFactory TowerFactory;
	static MirrorFactory MirrorFactory;
	static LevelInteractionState _State;
	static std::uint32_t LastSelectedEntity;

	//entt::Emitter<EventEmitter>::Connection<evnt::DeleteEntity> connection;
};