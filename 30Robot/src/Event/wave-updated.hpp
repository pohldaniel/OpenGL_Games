#pragma once

#include "System/wave-system.hpp"

namespace evnt {
	struct WaveUpdated {
		WaveUpdated(int timer, WaveState state) : timer(timer), state(state) {}

		int timer;
		WaveState state;
	};
}
