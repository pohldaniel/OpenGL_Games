#pragma once

#include <glm/glm.hpp>
#include <States/StateMachine.h>

enum WaveState {
	NOT_STARTED,
	PENDING,
	DURING,
	DONE
};

enum EnemyType {
	ROBOT,
	KAMIKAZE
};

enum ConstructibleType {
	TOWER_LASER,
	TOWER_SLOW,
	MIRROR_BASIC
};

enum CursorType {
	ROTATION = 0,
	ARROW = 1,
	NO = 2,
	ACTIVATE = 3,
	DESACTIVATE = 4
};

struct EventListener{
	virtual void OnStateChange(States states) = 0;
	virtual void OnApplicationQuit() {};
};

namespace evnt {

	struct ChangeGameState {
		ChangeGameState(States state, int levelNumber) : state(state), levelNumber(levelNumber){}
		States state;
		int levelNumber;
	};

	struct ApplicationExit {
		ApplicationExit() {}
	};

	struct ProgressionUpdated {
		ProgressionUpdated() {}
	};

	struct LaserParticleDead {
		LaserParticleDead(glm::vec2 position) : position(position) {}
		glm::vec2 position;
	};

	struct Loose {
		Loose() {}
	};

	struct Selected {
		Selected(unsigned int entity) : entity(entity) {}
		unsigned int entity;
	};

	struct StartWave {
		StartWave() {}
	};

	struct TowerDead {
		TowerDead(glm::vec2 position) : position(position) {}
		glm::vec2 position;
	};

	struct VictoryDelayEnds {
		VictoryDelayEnds() {}
	};

	struct WaveUpdated {
		WaveUpdated(int timer, WaveState state) : timer(timer), state(state) {}
		int timer;
		WaveState state;
	};

	struct EntityDamaged {
		EntityDamaged(std::uint32_t targetId, float damage) : entity(targetId), damage(damage) {}
		std::uint32_t entity;
		float damage;
	};

	struct EnemyReachedEnd {
		EnemyReachedEnd() {}
	};

	struct EnemyDead {
		EnemyDead(glm::vec2 position, EnemyType type) : position(position), type(type) {}

		glm::vec2 position;
		EnemyType type;
	};

	struct ConstructSelection {
		ConstructSelection(ConstructibleType type) : type(type) {}
		ConstructibleType type;
	};

	struct ChangeCursor {
		ChangeCursor(CursorType cursor) : cursor(cursor) {}
		CursorType cursor;
	};

	struct DeleteEntity {
		DeleteEntity(std::uint32_t entityId) : entityId(entityId) {}
		std::uint32_t entityId;
	};

	struct SelectRotation {
		SelectRotation(glm::vec2 mousePos) : mousePos(mousePos) {}
		glm::vec2 mousePos;
	};
}