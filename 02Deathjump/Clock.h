#pragma once
#include <chrono>

class Clock {
public:
	
	inline float restartSec() {
		end = std::chrono::high_resolution_clock::now();
		float elapsed_secs = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
		begin = end;

		return elapsed_secs;
	}

	inline int restartMicro() {
		end = std::chrono::high_resolution_clock::now();
		int elapsed_microsecs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		begin = end;

		return elapsed_microsecs;
	}

	inline float getElapsedTimeSec() const {
		return std::chrono::duration_cast<std::chrono::duration<float>>(begin - std::chrono::high_resolution_clock::now()).count();
	}

	inline int getElapsedTimeMicro() const {
		return std::chrono::duration_cast<std::chrono::microseconds>(begin - std::chrono::high_resolution_clock::now()).count();
	}
	
private:
	std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point begin = end;
};