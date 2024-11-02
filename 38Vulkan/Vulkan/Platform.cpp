#include <windows.h>
#include <iostream>
#include "platform.h"

char *platform_read_file(char *path, uint32_t *length){
	char *result = 0;

	// This opens the file
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	if (file != INVALID_HANDLE_VALUE){
		LARGE_INTEGER size;
		if (GetFileSizeEx(file, &size)){
			*length = size.QuadPart;
			//TODO: Suballocte from main allocation
			result = new char[*length];

			DWORD bytesRead;
			if (ReadFile(file, result, *length, &bytesRead, 0) && bytesRead == *length){
				// TODO: What to do here?
				// Success
			}else{
				//TODO: Assert and error checking
				std::cout << "Failed to read file" << std::endl;
			}
		}else{
			//TODO: Assert and error checking
			std::cout << "Failed to get file size" << std::endl;
		}
	}else{
		// TODO: Asserts, get error code
		std::cout << "Failed to open the file" << std::endl;
	}

	return result;
}