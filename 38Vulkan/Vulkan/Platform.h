#pragma once
#include "defines.h"

extern "C" {
	char* platform_read_file(char* path, uint32_t* length);
}