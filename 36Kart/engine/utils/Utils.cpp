#include <string.h>
# include <filesystem>
#include "Utils.h"

long Utils::floatToLong(float f) {
	// Converts a floating point number into an integer.
	// Fractional values are truncated as in ANSI C.
	// About 5 to 6 times faster than a standard typecast to an integer.

	long fpBits = *reinterpret_cast<const long*>(&f);
	long shift = 23 - (((fpBits & 0x7fffffff) >> 23) - 127);
	long result = ((fpBits & 0x7fffff) | (1 << 23)) >> shift;

	result = (result ^ (fpBits >> 31)) - (fpBits >> 31);
	result &= ~((((fpBits & 0x7fffffff) >> 23) - 127) >> 31);

	return result;
}

int Utils::whitespaces(char c[]) {
	int count = 0;
	char *ptr = c;
	while ((ptr = strchr(ptr, ' ')) != NULL) {
		count++;
		ptr++;
	}

	return count;
}

bool Utils::fileExist(const char *fileName) {
	return std::filesystem::exists(fileName);
}