#pragma once
#include <chrono>
#include <iostream>

class Clock {

public:

	inline void restart() {
		initial = std::chrono::high_resolution_clock::now();
		end = initial;
		begin = initial;
	}

	inline void reset() {
		end = std::chrono::high_resolution_clock::now();
		begin = end;
	}

	inline void reset(unsigned int intervall) {
		end = end + std::chrono::milliseconds(intervall);
		begin = end;
	}

	inline float resetSec() {
		end = std::chrono::high_resolution_clock::now();
		float elapsed_secs = std::chrono::duration_cast<std::chrono::duration<float>>(end - begin).count();
		begin = end;
		return elapsed_secs;
	}

	inline unsigned int resetMilli() {
		end = std::chrono::high_resolution_clock::now();
		unsigned int elapsed_millisecs = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
		begin = end;

		return elapsed_millisecs;
	}

	inline unsigned int resetMicro() {
		end = std::chrono::high_resolution_clock::now();
		unsigned int elapsed_microsecs = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
		begin = end;

		return elapsed_microsecs;
	}

	inline float getElapsedTimeSec() const {
		return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - begin).count();
	}

	inline float getElapsedTimeSinceRestartSec() const {
		return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - initial).count();
	}

	inline unsigned int getElapsedTimeMilli() const {
		return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin).count());
	}

	inline unsigned int getElapsedTimeSinceRestartMilli() const {
		return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - initial).count());
	}

	inline unsigned int getElapsedTimeMicro() const {
		return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - begin).count());
	}

	inline unsigned int getElapsedTimeSinceRestartMicro() const {
		return static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - initial).count());
	}

private:

	std::chrono::steady_clock::time_point initial = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point end = initial;
	std::chrono::steady_clock::time_point begin = end;
};