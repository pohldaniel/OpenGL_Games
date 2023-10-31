#pragma once

#include <ctime>

class Timer2 {
private:

#ifdef ANDROMEDA_SWITCH
	timeval lastTime;
#else
	clock_t lastTime;
#endif

	float msec;

public:

	Timer2();

	float GetDelta();
};