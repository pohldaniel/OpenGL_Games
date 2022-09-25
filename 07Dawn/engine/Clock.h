#pragma once
#include <chrono>
#include <iostream>
class Clock {
public:
	
	inline void restart() {
		end = std::chrono::high_resolution_clock::now();
		begin = end;
	}

	inline float restartSec() {
		end = std::chrono::high_resolution_clock::now();
		float elapsed_secs = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
		begin = end;
		return elapsed_secs;
	}

	inline unsigned int restartMilli() {
		end = std::chrono::high_resolution_clock::now();
		unsigned int elapsed_microsecs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		begin = end;

		return elapsed_microsecs;
	}

	inline unsigned int restartMicro() {
		end = std::chrono::high_resolution_clock::now();
		unsigned int elapsed_microsecs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		begin = end;

		return elapsed_microsecs;
	}

	inline float getElapsedTimeSec() const {
		return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - initial).count();
	}

	inline unsigned int getElapsedTimeMilli() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initial).count();
	}

	inline unsigned int getElapsedTimeMicro() const {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - initial).count();
	}
	
private:

	std::chrono::steady_clock::time_point initial = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point end = initial;
	std::chrono::steady_clock::time_point begin = end;	
};