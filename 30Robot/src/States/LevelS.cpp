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
#include "Event/progression-updated.hpp"
#include "Event/Interactions/select-rotation.hpp"

#include "Components/age.hpp"
#include "Components/shoot-laser.hpp"
#include "Components/entity-on.hpp"
#include "Components/animated.hpp"
#include "Components/animation-alpha.hpp"
#include "Components/look-at-mouse.hpp"
#include "Components/shoot-at.hpp"
#include "Components/constrained-rotation.hpp"

#include "maths.hpp"

#include <States/GameOverS.h>
#include <States/LevelExitS.h>

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

	Application::s_Level->setLevel(1);

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
	Keyboard &keyboard = Keyboard::instance();
	
	if (keyboard.keyPressed(Keyboard::KEY_W)) {
		Application::Emitter.publish<evnt::ProgressionUpdated>();
	}
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

	const glm::vec2 normMousePos = glm::vec2(
		imaths::rangeMapping(event.x, 0, Application::Width, 0, 101.3f),
		imaths::rangeMapping(Application::Height - event.y, 0, Application::Height, 0, PROJ_HEIGHT)
	);

	Application::Emitter.mousePos = normMousePos;

	if (Application::Emitter.focus == FocusMode::GAME) {
		switch (m_state) {
		case LevelInteractionState::FREE:{

			std::uint32_t entityId = Application::s_Level->getEntityOnTileFromProjCoord(normMousePos.x, normMousePos.y);
			if (Application::Registry.valid(entityId)) {
				if (Application::Registry.has<entityTag::Mirror>(entityId)) {
					Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
				}else if (Application::Registry.has<towerTag::LaserTower>(entityId)) {
					if (Application::Registry.get<cmpt::ShootLaser>(entityId).isActiv) {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::DESACTIVATE);
					}else {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ACTIVATE);
					}
				}else if (Application::Registry.has<towerTag::SlowTower>(entityId)) {
					if (Application::Registry.has<cmpt::ShootAt>(entityId)) {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::DESACTIVATE);
					}else {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ACTIVATE);
					}
				}
			}else {
				Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ARROW);
			}
			break;
		}

		case LevelInteractionState::ROTATE:
			Application::Emitter.publish<evnt::SelectRotation>(normMousePos);
			break;

		case LevelInteractionState::INVALID:
			break;

		case LevelInteractionState::OPTIONS:
			break;

		case LevelInteractionState::BUILD:
			break;
		default:
			break;
		}
	}
}

void LevelS::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	

	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);

		const glm::vec2 normMousePos = glm::vec2(imaths::rangeMapping(event.x, 0, Application::Width, 0, 101.3f),imaths::rangeMapping(Application::Height - event.y, 0, Application::Height, 0, PROJ_HEIGHT));

		if (Application::Emitter.focus == FocusMode::GAME) {
			switch (m_state) {
			case LevelInteractionState::FREE:
			case LevelInteractionState::INVALID:{
				// Get entity.
				int entityId = Application::s_Level->getEntityOnTileFromProjCoord(normMousePos.x, normMousePos.y);
				if (Application::Registry.valid(entityId)) {
					//If valid mirror then rotate.
					if (Application::Registry.has<entityTag::Mirror>(entityId)) {
						changeState(LevelInteractionState::ROTATE);
						Application::Registry.accommodate<stateTag::IsBeingControlled>(entityId);
						Application::Registry.accommodate<cmpt::LookAtMouse>(entityId);

						m_lastSelectedEntity = entityId;
						m_levelHud.setSelectedEntity(entityId);
					}
					//If tower then switch on or off
					if (Application::Registry.has<cmpt::ShootLaser>(entityId)) {
						cmpt::ShootLaser& shootLaser = Application::Registry.get<cmpt::ShootLaser>(entityId);
						shootLaser.isActiv = !shootLaser.isActiv;
					}
					if (Application::Registry.has<towerTag::SlowTower>(entityId)) {
						if (Application::Registry.has<cmpt::ShootAt>(entityId)) {
							Application::Registry.remove<cmpt::ShootAt>(entityId);
						}else {
							Application::Registry.assign<cmpt::ShootAt>(entityId, SLOW_TOWER_TIME_BETWEEN_TWO_SHOTS);
						}
					}
				}else {
					std::cout << "No valid entity on tile" << std::endl;
					changeState(LevelInteractionState::INVALID);
				}
				break;
			}

			case LevelInteractionState::ROTATE:
				break;

			case LevelInteractionState::OPTIONS:
				// Click outside option menu closes it
				changeState(LevelInteractionState::FREE);
				break;

			case LevelInteractionState::BUILD:{
				// Build selected type on tile if valid
				int tileId = Application::s_Level->getTileFromProjCoord(normMousePos.x, normMousePos.y);
				if (Application::Registry.valid(tileId)) {
					if (Application::Registry.has<tileTag::Constructible>(tileId)) {
						Application::Emitter.entityBeingPlaced = false;
						cmpt::Transform trans = Application::Registry.get<cmpt::Transform>(tileId);

						Application::Registry.remove<tileTag::Constructible>(tileId);
						Application::Registry.assign<cmpt::EntityOnTile>(tileId, m_lastSelectedEntity);

						Application::Registry.remove<positionTag::IsOnHoveredTile>(m_lastSelectedEntity);
						Application::Registry.reset<stateTag::IsBeingControlled>(m_lastSelectedEntity);
						Application::Registry.get<cmpt::Transform>(m_lastSelectedEntity).position += trans.position;
						if (Application::Registry.has<cmpt::ShootLaser>(m_lastSelectedEntity)) {
							Application::Registry.get<cmpt::ShootLaser>(m_lastSelectedEntity).isActiv = false;
						}

						// Rotatable on build
						if (Application::Registry.has<stateTag::RotateableByMouse>(m_lastSelectedEntity)) {
							Application::Registry.accommodate<stateTag::IsBeingControlled>(m_lastSelectedEntity);
							Application::Registry.accommodate<cmpt::LookAtMouse>(m_lastSelectedEntity);
							changeState(LevelInteractionState::ROTATE);
						}else {
							changeState(LevelInteractionState::FREE);
						}
					}else {
						std::cout << "Not a constructible tile" << std::endl;
						//changeState(LevelInteractionState::INVALID);
					}
				}else {
					std::cout << "Invalid tile" << std::endl;
					//changeState(LevelInteractionState::INVALID);
				}
				break;
			}

			default:
				break;
			}
		}
		return;
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {

		const glm::vec2 normMousePos = glm::vec2(imaths::rangeMapping(event.x, 0, Application::Width, 0, 101.3f), imaths::rangeMapping(Application::Height - event.y, 0, Application::Height, 0, PROJ_HEIGHT));

		if (Application::Emitter.focus == FocusMode::GAME) {
			switch (m_state) {
			case LevelInteractionState::FREE:
			case LevelInteractionState::INVALID:
			case LevelInteractionState::OPTIONS:
			{
				// Get entity. If valid open options. Else Invalid
				std::uint32_t entityId = Application::s_Level->getEntityOnTileFromProjCoord(normMousePos.x, normMousePos.y);
				if (Application::Registry.valid(entityId)) {
					changeState(LevelInteractionState::OPTIONS);
					m_levelHud.setSelectedEntity(entityId);
					cmpt::Transform trans = Application::Registry.get<cmpt::Transform>(entityId);
					glm::vec2 posWindow = glm::vec2(
						imaths::rangeMapping(trans.position.x, 0.0f, PROJ_WIDTH_RAT, 0.0f, WIN_WIDTH),
						imaths::rangeMapping(trans.position.y, 0.0f, PROJ_HEIGHT, 0.0f, WIN_HEIGHT)
					);

					if (Application::Registry.has<entityTag::Mirror>(entityId)) {
						m_levelHud.setOptionsPosition(posWindow);
					}else if (Application::Registry.has<towerTag::SlowTower>(entityId)) {
						m_levelHud.setOptionsPosition(posWindow);
					}else {
						changeState(LevelInteractionState::INVALID);
					}
				}else {
					std::cout << "No valid entity on tile" << std::endl;
					changeState(LevelInteractionState::INVALID);
				}
				break;
			}

			case LevelInteractionState::ROTATE:
				break;

			case LevelInteractionState::BUILD:
				//Give the count back in the shop
				if (Application::Registry.has<entityTag::Mirror>(m_lastSelectedEntity)) {
					Application::s_Progression.setMirrorNumber(Application::s_Progression.getMirrorNumbers() + 1);
				}
				else if (Application::Registry.has<towerTag::SlowTower>(m_lastSelectedEntity)) {
					Application::s_Progression.setSlowNumber(Application::s_Progression.getSlowNumbers() + 1);
				}
				//Reset
				Application::Emitter.entityBeingPlaced = false;
				changeState(LevelInteractionState::FREE);
				Application::Registry.destroy(m_lastSelectedEntity);
				m_lastSelectedEntity = entt::null;
				//Update info bar
				Application::Emitter.publish<evnt::ProgressionUpdated>();
				break;

			default:
				break;
			}
		}
	}
}

void LevelS::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
		if (Application::Emitter.focus == FocusMode::GAME) {
			switch (m_state) {
			case LevelInteractionState::FREE:
				break;

			case LevelInteractionState::ROTATE:
				// Stop rotating when mouse not pressed
				changeState(LevelInteractionState::FREE);
				if (Application::Registry.has<cmpt::ShootLaser>(m_lastSelectedEntity)) {
					Application::Registry.get<cmpt::ShootLaser>(m_lastSelectedEntity) = true;
				}
				if (Application::Registry.has<entityTag::Tower>(m_lastSelectedEntity)) {
					Application::Registry.reset<stateTag::RotateableByMouse>(m_lastSelectedEntity);
				}
				m_lastSelectedEntity = entt::null;
				break;

			case LevelInteractionState::INVALID:
				break;

			case LevelInteractionState::OPTIONS:
				break;

			case LevelInteractionState::BUILD:
				break;

			default:
				break;
			}
		}
	}
	
}

void LevelS::OnMouseWheel(Event::MouseWheelEvent& event) {
	std::uint32_t entity;
	if (Application::Registry.valid(m_lastSelectedEntity)) {
		entity = m_lastSelectedEntity;
	}else {
		glm::vec2 mousePos = Application::Emitter.mousePos;
		entity = Application::s_Level->getEntityOnTileFromProjCoord(mousePos.x, mousePos.y);
	}
	if (Application::Registry.valid(entity) && Application::Registry.has<stateTag::RotateableByMouse>(entity)) {
		if (Application::Registry.has<cmpt::ConstrainedRotation>(entity)) {
			cmpt::ConstrainedRotation& constRot = Application::Registry.get<cmpt::ConstrainedRotation>(entity);
			constRot.angleIndex = (constRot.angleIndex + event.delta + constRot.nbAngles) % constRot.nbAngles;
			// Rotate
			Application::Registry.get<cmpt::Transform>(entity).rotation += event.delta*constRot.angleStep;
			// Update sprite
			if (Application::Registry.has<cmpt::SpriteAnimation>(entity)) {
				cmpt::SpriteAnimation& spriteAnim = Application::Registry.get<cmpt::SpriteAnimation>(entity);
				spriteAnim.activeTile = constRot.angleIndex;
				spriteAnim.startTile = constRot.angleIndex;
				spriteAnim.endTile = constRot.angleIndex;
			}
		}else {			
			Application::Registry.get<cmpt::Transform>(entity).rotation += imaths::TAU / 32 * event.delta;

		}
	}
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
		Application::Emitter.publish<evnt::ChangeGameStateNew>(States::GAMEOVER);
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


void LevelS::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtTop(states == States::GAMEOVER ? static_cast<State*>(new GameOverS(m_machine)) : static_cast<State*>(new LevelExitS(m_machine)));
}