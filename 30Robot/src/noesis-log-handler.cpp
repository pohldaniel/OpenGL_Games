#include <iostream>
#include "noesis-log-handler.hpp"

void noelog::messageCallback(const char* filename, uint32_t line, uint32_t level, const char* channel, const char* message) {
  	switch (level) {
		case 0: std::cout << "[Noesis] {}: " << message << std::endl; break; 	// Trace
		case 1: std::cout << "[Noesis] {}: " << message << std::endl; break; 	// Debug
		case 2: std::cout << "[Noesis] {}: " << message << std::endl; break; 	// Info
		case 3: std::cout << "[Noesis] {}: " << message << std::endl; break; 	// Warning
    	case 4:std::cout << "[Noesis] {}: " << message << std::endl; break; 	// Error
	}
}

void noelog::errorHandler(const char* file, uint32_t line, const char* message, bool fatal) {
	std::cout << "[Noesis] {}: " << message << std::endl;
	
}
