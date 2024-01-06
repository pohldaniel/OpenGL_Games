#include <limits>
#include <Components/Components.h>
#include <States/GameOverState.h>
#include <States/LevelExitState.h>
#include <States/LevelState.h>
#include <GUI/LevelHud.h>
#include <Services/IHelper.h>
#include <Services/IAudio.h>
#include <Level/Level.h>

#include "Globals.h"
#include "Application.h"
#include "Renderer.h"
#include "Tags.h"
#include "Maths.h"

LevelConnections::LevelConnections(LevelState& level) :
	connection1(Application::Emitter.on<evnt::ConstructSelection>([this, &level](const evnt::ConstructSelection & event, EventEmitter & emitter) {
		switch (state) {
			case LevelInteractionState::INVALID:
			case LevelInteractionState::FREE:
				Application::Emitter.entityBeingPlaced = true;
				level.changeState(LevelInteractionState::BUILD);
				unsigned int entityId;

				switch (event.type) {
					case ConstructibleType::MIRROR_BASIC:
						entityId = level.m_mirrorFactory.create(0, 0);
						Application::Registry.assign<positionTag::IsOnHoveredTile>(entityId);
						Application::s_Progression.reduceMirrorNumberBy1();
						break;

					case ConstructibleType::TOWER_LASER:
						entityId = level.m_towerFactory.createLaser(0, 0);
						Application::Registry.assign<stateTag::IsBeingControlled>(entityId);
						Application::Registry.assign<positionTag::IsOnHoveredTile>(entityId);
						Application::Registry.assign<stateTag::RotateableByMouse>(entityId);
						Application::Registry.get<cmpt::ShootLaser>(entityId).isActiv = false;
						break;

					case ConstructibleType::TOWER_SLOW:
						entityId = level.m_towerFactory.createSlow(0, 0);
						Application::Registry.assign<stateTag::IsBeingControlled>(entityId);
						Application::Registry.assign<positionTag::IsOnHoveredTile>(entityId);
						Application::s_Progression.reduceSlowNumberBy1();
						break;
				}
				lastSelectedEntity = entityId;
				break;
		}
	})), 
	connection2(Application::Emitter.on<evnt::DeleteEntity>([&level](const evnt::DeleteEntity & event, EventEmitter & emitter) {
		if (Application::Registry.valid(event.entityId)) {
			glm::vec2 position = Application::Registry.get<cmpt::Transform>(event.entityId).position;
			Application::Registry.assign<cmpt::Animated>(event.entityId, 1.0f, true);
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
			level.changeState(LevelInteractionState::FREE);


		}
	})),
	connection3(Application::Emitter.on<evnt::TowerDead>([](const evnt::TowerDead & event, EventEmitter & emitter) {
		std::uint32_t tileId = Application::s_Level->getTileFromProjCoord(event.position.x / WIN_RATIO, event.position.y);
		Application::Registry.accommodate<tileTag::Constructible>(tileId);
		Application::Registry.reset<cmpt::EntityOnTile>(tileId);
	})), 
	connection4(Application::Emitter.on<evnt::WaveUpdated>([this](const evnt::WaveUpdated & event, EventEmitter & emitter) {
		switch (event.state) {
			case WaveState::NOT_STARTED:
			case WaveState::PENDING:
			case WaveState::DURING:
				waveDone = false;
				break;

			case WaveState::DONE:
				waveDone = true;
				break;

			default:
				break;
		}
	})), 
	connection5(Application::Emitter.on<evnt::EnemyDead>([this](const evnt::EnemyDead & event, EventEmitter & emitter) {
		if (waveDone) {
			int enemyRemaining = 0;
			Application::Registry.view<entityTag::Enemy>().each([&enemyRemaining](auto entity, auto) {
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
	})), 
	connection6(Application::Emitter.on<evnt::VictoryDelayEnds>([](const evnt::VictoryDelayEnds & event, EventEmitter & emitter) {
		Application::Emitter.publish<evnt::ChangeGameState>(States::LEVELEXIT, 0);
	})), 
	connection7(Application::Emitter.on<evnt::EnemyReachedEnd>([this](const evnt::EnemyReachedEnd & event, EventEmitter & emitter) {
		if (waveDone) {
			int enemyRemaining = 0;
			Application::Registry.view<entityTag::Enemy>().each([&enemyRemaining](auto entity, auto) {
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
	}})), 
	connection8(Application::Emitter.on<evnt::Loose>([this](const evnt::Loose & event, EventEmitter & emitter) {
		// TODO play an outro
		Application::Emitter.publish<evnt::ChangeGameState>(States::GAMEOVER, 0);
	})), 
	waveDone(false),
	lastSelectedEntity(std::numeric_limits<std::uint32_t>::max()),
	state(LevelInteractionState::FREE){}

LevelState::LevelState(StateMachine& machine) : State(machine, States::LEVEL),
m_towerFactory(Application::Registry), 
m_mirrorFactory(Application::Registry), 
m_connections(*this),
m_invalidTimeCounter(0),
m_invalidTimeMax(1 * 60),
m_debugDraw(false){

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	LevelHud::Get().reset();
	
	m_ui = Noesis::GUI::CreateView(LevelHud::Get().m_grid).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);

	m_towerFactory.init();
	m_mirrorFactory.init();
}

LevelState::~LevelState() {
	Application::Emitter.erase(m_connections.connection1);
	Application::Emitter.erase(m_connections.connection2);
	Application::Emitter.erase(m_connections.connection3);
	Application::Emitter.erase(m_connections.connection4);
	Application::Emitter.erase(m_connections.connection5);
	Application::Emitter.erase(m_connections.connection6);
	Application::Emitter.erase(m_connections.connection7);
	Application::Emitter.erase(m_connections.connection8);

	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;

	entt::ServiceLocator<IHelper>::ref().reset();
}

void LevelState::fixedUpdate() {

}

void LevelState::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debugDraw = !m_debugDraw;
	}
}

void LevelState::render() {
	if (m_connections.state == LevelInteractionState::INVALID) {
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

	if (m_debugDraw) {
		Application::s_Level->drawGraph();
		Application::s_Level->drawGrid();
	}
}

void LevelState::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelState::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);

	const glm::vec2 normMousePos = glm::vec2(
		imaths::rangeMapping(event.x, 0, Application::Width, 0, 101.3f),
		imaths::rangeMapping(Application::Height - event.y, 0, Application::Height, 0, PROJ_HEIGHT)
	);

	Application::Emitter.mousePos = normMousePos;

	if (Application::Emitter.focus == FocusMode::GAME) {
		switch (m_connections.state) {
		case LevelInteractionState::FREE: {

			std::uint32_t entityId = Application::s_Level->getEntityOnTileFromProjCoord(normMousePos.x, normMousePos.y);
			if (Application::Registry.valid(entityId)) {
				if (Application::Registry.has<entityTag::Mirror>(entityId)) {
					Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ROTATION);
				}
				else if (Application::Registry.has<towerTag::LaserTower>(entityId)) {
					if (Application::Registry.get<cmpt::ShootLaser>(entityId).isActiv) {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::DESACTIVATE);
					}
					else {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ACTIVATE);
					}
				}
				else if (Application::Registry.has<towerTag::SlowTower>(entityId)) {
					if (Application::Registry.has<cmpt::ShootAt>(entityId)) {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::DESACTIVATE);
					}
					else {
						Application::Emitter.publish<evnt::ChangeCursor>(CursorType::ACTIVATE);
					}
				}
			}
			else {
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

void LevelState::OnMouseButtonDown(Event::MouseButtonEvent& event) {


	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);

		const glm::vec2 normMousePos = glm::vec2(imaths::rangeMapping(event.x, 0, Application::Width, 0, 101.3f), imaths::rangeMapping(Application::Height - event.y, 0, Application::Height, 0, PROJ_HEIGHT));

		if (Application::Emitter.focus == FocusMode::GAME) {
			switch (m_connections.state) {
			case LevelInteractionState::FREE:
			case LevelInteractionState::INVALID: {
				// Get entity.
				int entityId = Application::s_Level->getEntityOnTileFromProjCoord(normMousePos.x, normMousePos.y);
				if (Application::Registry.valid(entityId)) {
					//If valid mirror then rotate.
					if (Application::Registry.has<entityTag::Mirror>(entityId)) {
						changeState(LevelInteractionState::ROTATE);
						Application::Registry.accommodate<stateTag::IsBeingControlled>(entityId);
						Application::Registry.accommodate<cmpt::LookAtMouse>(entityId);

						m_connections.lastSelectedEntity = entityId;
						LevelHud::Get().setSelectedEntity(entityId);
					}
					//If tower then switch on or off
					if (Application::Registry.has<cmpt::ShootLaser>(entityId)) {
						cmpt::ShootLaser& shootLaser = Application::Registry.get<cmpt::ShootLaser>(entityId);
						shootLaser.isActiv = !shootLaser.isActiv;
					}
					if (Application::Registry.has<towerTag::SlowTower>(entityId)) {
						if (Application::Registry.has<cmpt::ShootAt>(entityId)) {
							Application::Registry.remove<cmpt::ShootAt>(entityId);
						}
						else {
							Application::Registry.assign<cmpt::ShootAt>(entityId, SLOW_TOWER_TIME_BETWEEN_TWO_SHOTS);
						}
					}
				}
				else {
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

			case LevelInteractionState::BUILD: {
				// Build selected type on tile if valid
				int tileId = Application::s_Level->getTileFromProjCoord(normMousePos.x, normMousePos.y);
				if (Application::Registry.valid(tileId)) {
					if (Application::Registry.has<tileTag::Constructible>(tileId)) {
						Application::Emitter.entityBeingPlaced = false;
						cmpt::Transform trans = Application::Registry.get<cmpt::Transform>(tileId);

						Application::Registry.remove<tileTag::Constructible>(tileId);
						Application::Registry.assign<cmpt::EntityOnTile>(tileId, m_connections.lastSelectedEntity);

						Application::Registry.remove<positionTag::IsOnHoveredTile>(m_connections.lastSelectedEntity);
						Application::Registry.reset<stateTag::IsBeingControlled>(m_connections.lastSelectedEntity);
						Application::Registry.get<cmpt::Transform>(m_connections.lastSelectedEntity).position += trans.position;
						if (Application::Registry.has<cmpt::ShootLaser>(m_connections.lastSelectedEntity)) {
							Application::Registry.get<cmpt::ShootLaser>(m_connections.lastSelectedEntity).isActiv = false;
						}

						// Rotatable on build
						if (Application::Registry.has<stateTag::RotateableByMouse>(m_connections.lastSelectedEntity)) {
							Application::Registry.accommodate<stateTag::IsBeingControlled>(m_connections.lastSelectedEntity);
							Application::Registry.accommodate<cmpt::LookAtMouse>(m_connections.lastSelectedEntity);
							changeState(LevelInteractionState::ROTATE);
						}
						else {
							changeState(LevelInteractionState::FREE);
						}
					}
					else {
						std::cout << "Not a constructible tile" << std::endl;
						//changeState(LevelInteractionState::INVALID);
					}
				}
				else {
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
			switch (m_connections.state) {
			case LevelInteractionState::FREE:
			case LevelInteractionState::INVALID:
			case LevelInteractionState::OPTIONS:
			{
				// Get entity. If valid open options. Else Invalid
				std::uint32_t entityId = Application::s_Level->getEntityOnTileFromProjCoord(normMousePos.x, normMousePos.y);
				if (Application::Registry.valid(entityId)) {
					changeState(LevelInteractionState::OPTIONS);
					LevelHud::Get().setSelectedEntity(entityId);
					cmpt::Transform trans = Application::Registry.get<cmpt::Transform>(entityId);
					glm::vec2 posWindow = glm::vec2(
						imaths::rangeMapping(trans.position.x, 0.0f, PROJ_WIDTH_RAT, 0.0f, WIN_WIDTH),
						imaths::rangeMapping(trans.position.y, 0.0f, PROJ_HEIGHT, 0.0f, WIN_HEIGHT)
					);

					if (Application::Registry.has<entityTag::Mirror>(entityId)) {
						LevelHud::Get().setOptionsPosition(posWindow);
					}
					else if (Application::Registry.has<towerTag::SlowTower>(entityId)) {
						LevelHud::Get().setOptionsPosition(posWindow);
					}
					else {
						changeState(LevelInteractionState::INVALID);
					}
				}
				else {
					std::cout << "No valid entity on tile" << std::endl;
					changeState(LevelInteractionState::INVALID);
				}
				break;
			}

			case LevelInteractionState::ROTATE:
				break;

			case LevelInteractionState::BUILD:
				//Give the count back in the shop
				if (Application::Registry.has<entityTag::Mirror>(m_connections.lastSelectedEntity)) {
					Application::s_Progression.setMirrorNumber(Application::s_Progression.getMirrorNumbers() + 1);
				}
				else if (Application::Registry.has<towerTag::SlowTower>(m_connections.lastSelectedEntity)) {
					Application::s_Progression.setSlowNumber(Application::s_Progression.getSlowNumbers() + 1);
				}
				//Reset
				Application::Emitter.entityBeingPlaced = false;
				changeState(LevelInteractionState::FREE);
				Application::Registry.destroy(m_connections.lastSelectedEntity);
				m_connections.lastSelectedEntity = entt::null;
				//Update info bar
				Application::Emitter.publish<evnt::ProgressionUpdated>();
				break;

			default:
				break;
			}
		}
	}
}

void LevelState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
		if (Application::Emitter.focus == FocusMode::GAME) {
			switch (m_connections.state) {
			case LevelInteractionState::FREE:
				break;

			case LevelInteractionState::ROTATE:
				// Stop rotating when mouse not pressed
				changeState(LevelInteractionState::FREE);
				if (Application::Registry.has<cmpt::ShootLaser>(m_connections.lastSelectedEntity)) {
					Application::Registry.get<cmpt::ShootLaser>(m_connections.lastSelectedEntity) = true;
				}

				if (Application::Registry.has<entityTag::Tower>(m_connections.lastSelectedEntity)) {
					Application::Registry.reset<stateTag::RotateableByMouse>(m_connections.lastSelectedEntity);
				}
				m_connections.lastSelectedEntity = entt::null;
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

void LevelState::OnMouseWheel(Event::MouseWheelEvent& event) {
	std::uint32_t entity;
	if (Application::Registry.valid(m_connections.lastSelectedEntity)) {
		entity = m_connections.lastSelectedEntity;
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
		}
		else {
			Application::Registry.get<cmpt::Transform>(entity).rotation += imaths::TAU / 32 * event.delta;

		}
	}
}

LevelInteractionState LevelState::getInteractionState() const {
	return m_connections.state;
}

void LevelState::OnStateChange(States states) {
	entt::ServiceLocator<IAudio>::ref().stopAllSounds();

	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::GAMEOVER ? static_cast<State*>(new GameOverState(m_machine)) : static_cast<State*>(new LevelExitState(m_machine)));
}

void LevelState::changeState(LevelInteractionState state) {
	// Exit current state
	switch (m_connections.state) {
		case LevelInteractionState::FREE:
			break;

		case LevelInteractionState::ROTATE:
			Application::Registry.remove<stateTag::IsBeingControlled>(m_connections.lastSelectedEntity);
			Application::Registry.remove<cmpt::LookAtMouse>(m_connections.lastSelectedEntity);
			break;

		case LevelInteractionState::INVALID:
			break;

		case LevelInteractionState::OPTIONS:
			LevelHud::Get().setOptionsVisibilityTo(false);
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
			LevelHud::Get().setSelectedEntity(entt::null);
			break;

		case LevelInteractionState::INVALID:
			Application::Emitter.publish<evnt::ChangeCursor>(CursorType::NO);
			LevelHud::Get().setSelectedEntity(entt::null);
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
	m_connections.state = state;
}