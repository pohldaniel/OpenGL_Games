#pragma once

#include <States/StateMachine.h>

struct EventListener{
	virtual void OnStateChange(States states) = 0;
	virtual void OnApplicationQuit() {};
};

namespace evnt {
	struct ApplicationExit {
		ApplicationExit() {}
	};

	struct ChangeGameStateNew {
		ChangeGameStateNew(States state) : state(state) {}
		States state;
	};
}