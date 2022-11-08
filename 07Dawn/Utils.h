#pragma once

#include <string>
#include <sstream>
#include <algorithm>

namespace Utils {

	static inline std::string ConvertTime(float seconds) {
		// break our seconds down into hours, minutes or seconds and return a pretty string.
		std::string outputString;
		std::stringstream ss;

		unsigned short minutes = 0;
		unsigned short hours = 0;

		seconds = floor(seconds);
		hours = static_cast<unsigned short>(floor(seconds / 3600.0f));
		minutes = static_cast<unsigned short>(ceil(seconds / 60.0f)) - static_cast<unsigned short>(floor(seconds / 3600.0f) * 60.0f);

		if (minutes == 1) { minutes = 0; } // when we're below or at 60 seconds, dont display minutes.

		if (hours > 0) {
			if (hours == 1) {
				ss << hours << " hour ";
			}
			else {
				ss << hours << " hours ";
			}
		}

		if (minutes > 0) {
			ss << minutes << " minutes ";
		}

		if (minutes <= 0 && hours <= 0) {
			if (seconds == 1) {
				ss << seconds << " second";
			}
			else {
				ss << seconds << " seconds";
			}
		}

		outputString = ss.str();
		return outputString;
	}

	static inline std::string ConvertTime(float seconds, unsigned short duration) {
		// break our seconds down into hours, minutes or seconds and return a pretty string.
		std::string outputString;
		std::stringstream ss;

		unsigned short minutes = 0;
		unsigned short hours = 0;
	
		seconds = ceil(duration - seconds);
		//seconds = abs(ceil(duration - seconds));

		hours = static_cast<unsigned short>(floor(seconds / 3600.0f));
		minutes = static_cast<unsigned short>(ceil(seconds / 60.0f)) - static_cast<unsigned short>(floor(seconds / 3600.0f) * 60.0f);

		if (minutes == 1) { minutes = 0; } // when we're below or at 60 seconds, dont display minutes.

		if (hours > 0) {
			ss << hours << "h ";
		}

		if (minutes > 0) {
			ss << minutes << "m ";
		}

		if (minutes <= 0 && hours <= 0) {
			ss << seconds << "s ";
		}
		outputString = ss.str();
		return outputString;
	}

}