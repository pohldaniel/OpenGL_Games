#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <numeric>

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

	static inline bool file_exists(const std::string& file){
		std::ifstream temp(file.c_str());

		if (!temp)
			return false;
		return true;
	}

	static inline std::string replaceEscape(const std::string& input) {
		
		std::vector<std::string> tokens;
		std::string::size_type pos = 0;
		std::string::size_type prev = 0;
		while ((pos = input.find('\n', prev)) != std::string::npos) {
			tokens.push_back(input.substr(prev, pos - prev) + "\\n");
			prev = pos + 1;
		}
		tokens.push_back(input.substr(prev));

		return std::accumulate(tokens.begin(), tokens.end(), std::string(""));
	}
}

namespace currency {
	enum currency {
		COPPER, // 100 COPPER == 1 SILVER
		SILVER, // 100 SILVER == 1 GOLD
		GOLD
	};

	inline void exchangeCoins(std::uint32_t &copper, std::uint32_t &silver, std::uint32_t &gold, std::uint32_t &coins) {
		// exchanging coins to copper coins.
		copper = coins % 100;
		coins -= copper;
		if (coins == 0)
			return;

		coins /= 100;
		silver = coins % 100;
		coins -= silver;
		if (coins == 0)
			return;

		gold = coins / 100;
	}

	inline std::string getLongTextString(std::uint32_t coins) {
		std::stringstream ss;
		ss.clear();

		std::uint32_t copper = 0, silver = 0, gold = 0;
		bool addComma = false;

		exchangeCoins(copper, silver, gold, coins);
		if (gold > 0) {
			ss << gold << " gold";
			addComma = true;
		}

		if (silver > 0) {
			if (addComma == true) {
				ss << ", ";
			}
			ss << silver << " silver";
			addComma = true;
		}

		if (copper > 0) {
			if (addComma == true) {
				ss << ", ";
			}
			ss << copper << " copper";
		}

		return ss.str();;
	}

	inline std::string convertCoinsToString(currency currency, std::uint32_t coins) {
		std::stringstream ss;
		std::string output;

		std::uint32_t copper = 0, silver = 0, gold = 0;

		exchangeCoins(copper, silver, gold, coins);

		switch (currency){
			case currency::COPPER:
				ss << copper;
				break;
			case currency::SILVER:
				ss << silver;
				break;
			case currency::GOLD:
				ss << gold;
				break;
		};

		return ss.str();
	}

}