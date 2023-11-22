#pragma once
#include <fstream>
#include <sstream>

#include "Constants.h"

class SaveFile {
public:
	static void load(const std::string path);
	static void save(const std::string path);
private:
	static std::fstream s_file;
};