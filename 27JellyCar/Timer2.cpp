#include "Timer2.h"

Timer2::Timer2()
{
	msec = 0.0f;

#ifdef ANDROMEDA_SWITCH
	gettimeofday(&lastTime, NULL);
#else
	lastTime = clock();
#endif

}

float Timer2::GetDelta()
{
#ifdef ANDROMEDA_SWITCH
	timeval curTime;
	gettimeofday(&curTime, NULL);

	double time_in_mill = (curTime.tv_sec) * 1000 + (curTime.tv_usec) / 1000;
	double time_in_mill_last = (lastTime.tv_sec) * 1000 + (lastTime.tv_usec) / 1000;

	msec = (time_in_mill - time_in_mill_last);

	msec = msec * 0.001f;
	lastTime = curTime;

	if (msec < 0.0f)
		return 0.0f;
#else
	clock_t now = clock();
	clock_t delta = now - lastTime;
	lastTime = now;

	msec = ((float)delta) / CLOCKS_PER_SEC;
#endif


	return msec;
}