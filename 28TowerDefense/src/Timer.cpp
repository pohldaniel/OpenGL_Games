#include "Timer.h"

Timer::Timer(float setTimeSMax, float setTimeSCurrent) : 
	timeSMax(setTimeSMax), timeSCurrent(setTimeSCurrent) {

}

void Timer::countUp(float dT) {
	if (timeSCurrent < timeSMax) {
		timeSCurrent += dT;
		if (timeSCurrent > timeSMax)
			timeSCurrent = timeSMax;
	}
}

void Timer::countDown(float dT) {
	if (timeSCurrent > 0.0f) {
		timeSCurrent -= dT;
		if (timeSCurrent < 0.0f)
			timeSCurrent = 0.0f;
	}
}

void Timer::resetToZero() {
	timeSCurrent = 0.0f;
}


void Timer::resetToMax() {
	timeSCurrent = timeSMax;
}



bool Timer::timeSIsZero() {
	return (timeSCurrent <= 0.0f);
}


bool Timer::timeSIsGreaterThanOrEqualTo(float timeSCheck) {
	return (timeSCurrent >= timeSCheck);
}