#pragma once

#include <States/StateMachine.h>

enum class LevelInteractionState {
	FREE,
	ROTATE,
	INVALID,
	OPTIONS,
	BUILD
};

struct EventListener{
	virtual void OnStateChange(States states) = 0;
	virtual void OnApplicationQuit() {};
	virtual void OnLevelInteractionStateChange(LevelInteractionState state) {};
};

namespace evnt {
	struct ApplicationExit {
		ApplicationExit() {}
	};

	struct ChangeGameStateNew {
		ChangeGameStateNew(States state) : state(state) {}
		States state;
	};

	struct ChangeLevelInteractionState {
		ChangeLevelInteractionState(LevelInteractionState state) : state(state) {}
		LevelInteractionState state;
	};
}