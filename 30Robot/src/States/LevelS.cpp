#include "LevelS.h"

#include "Globals.h"
#include "Application.h"
#include "Renderer.h"

#include "tags.hpp"
#include "Event/wave-updated.hpp"
#include "Event/enemy-dead.hpp"
#include "Event/victory-delay-ends.hpp"
#include "Event/change-game-state.hpp"
#include "Event/enemy-reached-end.hpp"
#include "Event/loose.hpp"
#include "Event/Interactions/delete-entity.hpp"
#include "Event/tower-dead.hpp"

#include "Components/age.hpp"
#include "Components/shoot-laser.hpp"
#include "Components/entity-on.hpp"
#include "Components/animated.hpp"
#include "Components/animation-alpha.hpp"
#include "Components/look-at-mouse.hpp"

LevelS::LevelS(StateMachine& machine) : State(machine, States::LEVEL),
	m_levelHud(Application::Emitter, Application::s_Progression), 
	m_state(LevelInteractionState::FREE),
	m_towerFactory(Application::Registry), 
	m_mirrorFactory(Application::Registry), 
	m_invalidTimeCounter(0), 
	m_invalidTimeMax(1 * 60){

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = m_levelHud;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);

	handleVictoryConditions();
	handleConstructions();
}

LevelS::~LevelS() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelS::fixedUpdate() {

}

void LevelS::update() {

}

void LevelS::render() {
	if (m_state == LevelInteractionState::INVALID) {
		m_invalidTimeCounter++;
	}

	if (m_invalidTimeCounter >= m_invalidTimeMax) {
		m_invalidTimeCounter = 0;
		changeState(LevelInteractionState::FREE);
	}


	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	//Updates
	Application::s_AnimationSystem->update(m_dt);
	Application::s_MovementSystem->update(m_dt);
	Application::s_RenderSystem->update(m_dt);
	Application::s_AttackSystem->update(m_dt);
	Application::s_LifeAndDeathSystem->update(m_dt);
	Application::s_WaveSystem->update(m_dt);
	m_ui->GetRenderer()->Render();
}

void LevelS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelS::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void LevelS::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelS::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelS::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void LevelS::OnStateChange(States states) {
	
}

void LevelS::handleVictoryConditions() {
	Application::Emitter.on<evnt::WaveUpdated>([this](const evnt::WaveUpdated & event, EventEmitter & emitter) {
		switch (event.state) {
		case WaveState::NOT_STARTED:
		case WaveState::PENDING:
		case WaveState::DURING:
			this->m_bWaveDone = false;
			break;

		case WaveState::DONE:
			this->m_bWaveDone = true;
			break;

		default:
			break;
		}
	});

	Application::Emitter.on<evnt::EnemyDead>([this](const evnt::EnemyDead & event, EventEmitter & emitter) {
		if (this->m_bWaveDone) {
			int enemyRemaining = 0;
			Application::Registry.view<entityTag::Enemy>().each([this, &enemyRemaining](auto entity, auto) {
				if (!Application::Registry.has<stateTag::IsDisappearing>(entity)) {
					enemyRemaining++;
				}
			});
			if (enemyRemaining == 1) {
				//Set delay before victory and changing game state
				std::uint32_t timer = Application::Registry.create();
				Application::Registry.assign<cmpt::Age>(timer, DELAY_BETWEEN_VICTORY_AND_CHANGE_GAME_STATE);
				Application::Registry.assign<entityTag::VictoryDelayTimer>(timer);
			}
		}
	});

	Application::Emitter.on<evnt::VictoryDelayEnds>([this](const evnt::VictoryDelayEnds & event, EventEmitter & emitter) {
		Application::Emitter.publish<evnt::ChangeGameState>(GameState::LEVEL_EXIT, Application::s_Progression.getLevelNumber());
	});

	Application::Emitter.on<evnt::EnemyReachedEnd>([this](const evnt::EnemyReachedEnd & event, EventEmitter & emitter) {
		if (this->m_bWaveDone) {
			int enemyRemaining = 0;
			Application::Registry.view<entityTag::Enemy>().each([this, &enemyRemaining](auto entity, auto) {
				if (!Application::Registry.has<stateTag::IsDisappearing>(entity)) {
					enemyRemaining++;
				}
			});
			if (enemyRemaining == 0) {
				//Set delay before victory and changing game state
				std::uint32_t timer = Application::Registry.create();
				Application::Registry.assign<cmpt::Age>(timer, DELAY_BETWEEN_VICTORY_AND_CHANGE_GAME_STATE);
				Application::Registry.assign<entityTag::VictoryDelayTimer>(timer);
			}
		}
	});

	Application::Emitter.on<evnt::Loose>([this](const evnt::Loose & event, EventEmitter & emitter) {
		// TODO play an outro
		Application::Emitter.publish<evnt::ChangeGameState>(GameState::GAME_OVER);
	});
}

void LevelS::handleConstructions() {
	Application::Emitter.on<evnt::ConstructSelection>([this](const evnt::ConstructSelection & event, EventEmitter & emitter) {
		switch (m_state) {
		case LevelInteractionState::INVALID:
		case LevelInteractionState::FREE:
			Application::Emitter.entityBeingPlaced = true;
			this->m_constructType = event.type;
			this->changeState(LevelInteractionState::BUILD);
			unsigned int entityId;

			switch (m_constructType) {
			case ConstructibleType::MIRROR_BASIC:
				entityId = m_mirrorFactory.create(0, 0);
				Application::Registry.assign<positionTag::IsOnHoveredTile>(entityId);
				Application::s_Progression.reduceMirrorNumberBy1();
				break;

			case ConstructibleType::TOWER_LASER:
				entityId = m_towerFactory.createLaser(0, 0);
				Application::Registry.assign<stateTag::IsBeingControlled>(entityId);
				Application::Registry.assign<positionTag::IsOnHoveredTile>(entityId);
				Application::Registry.assign<stateTag::RotateableByMouse>(entityId);
				Application::Registry.get<cmpt::ShootLaser>(entityId).isActiv = false;
				break;

			case ConstructibleType::TOWER_SLOW:
				entityId = m_towerFactory.createSlow(0, 0);
				Application::Registry.assign<stateTag::IsBeingControlled>(entityId);
				Application::Registry.assign<positionTag::IsOnHoveredTile>(entityId);
				Application::s_Progression.reduceSlowNumberBy1();
				break;
			}
			m_lastSelectedEntity = entityId;
			break;
		}
	});

	// TODO use a safer and more global way, because if tile is invalid, it will cause a problem
	// Start by using only one type of event for entity deletion, and see if there is a way to check the grid for deletion
	Application::Emitter.on<evnt::DeleteEntity>([this](const evnt::DeleteEntity & event, EventEmitter & emitter) {
		if (Application::Registry.valid(event.entityId)) {
			glm::vec2 position = Application::Registry.get<cmpt::Transform>(event.entityId).position;
			//this->m_game.registry.destroy(event.entityId);
			Application::Registry.assign<cmpt::Animated>(event.entityId, 1, true);
			Application::Registry.assign<cmpt::AnimationAlpha>(event.entityId, false);

			if (Application::Registry.has<entityTag::Mirror>(event.entityId)) {
				Application::s_Progression.increaseMirrorNumberBy1();
			}
			if (Application::Registry.has<entityTag::Tower>(event.entityId)) {
				Application::s_Progression.increaseSlowNumberBy1();
			}

			std::uint32_t tileId = Application::s_Level->getTileFromProjCoord(position.x / WIN_RATIO, position.y);
			Application::Registry.assign<tileTag::Constructible>(tileId);
			Application::Registry.remove<cmpt::EntityOnTile>(tileId);

			this->changeState(LevelInteractionState::FREE);
		}
	});

	Application::Emitter.on<evnt::TowerDead>([this](const evnt::TowerDead & event, EventEmitter & emitter) {
		std::uint32_t tileId = Application::s_Level->getTileFromProjCoord(event.position.x / WIN_RATIO, event.position.y);
		Application::Registry.accommodate<tileTag::Constructible>(tileId);
		Application::Registry.reset<cmpt::EntityOnTile>(tileId);
	});
}

void LevelS::changeState(LevelInteractionState state) {
	// Exit current state
	switch (m_state) {
	case LevelInteractionState::FREE:
		break;

	case LevelInteractionState::ROTATE:
		Application::Registry.remove<stateTag::IsBeingControlled>(m_lastSelectedEntity);
		Application::Registry.remove<cmpt::LookAtMouse>(m_lastSelectedEntity);
		break;

	case LevelInteractionState::INVALID:
		break;

	case LevelInteractionState::OPTIONS:
		m_levelHud.setOptionsVisibilityTo(false);
		break;

	case LevelInteractionState::BUILD:
		break;
	default:
		break;
	}

	// Enter new state
	switch (state) {
	case LevelInteractionState::FREE:
		Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case LevelInteractionState::INVALID:
		Application::Emitter.publish<evnt::ChangeCursor>(CursorType::NO);
		m_levelHud.setSelectedEntity(entt::null);
		break;

	case LevelInteractionState::ROTATE:
		Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
		break;

	case LevelInteractionState::OPTIONS:
		Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		break;

	case LevelInteractionState::BUILD:
		Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
		break;

	default:
		break;
	}

	// Change state
	m_state = state;
}

LevelInteractionState LevelS::getInteractionState() const {
	return m_state;
}