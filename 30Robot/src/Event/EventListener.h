#pragma once

#include <States/StateMachine.h>

struct EventListener{
	virtual void OnStateChange(States states) = 0;
	virtual void OnApplicationQuit() {};
};

namespace evnt {

	struct ChangeGameState {
		ChangeGameState(States state) : state(state) {}
		States state;
	};

	struct ApplicationExit {
		ApplicationExit() {}
	};
}