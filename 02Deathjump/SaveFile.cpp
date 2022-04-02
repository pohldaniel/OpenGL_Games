#include "SaveFile.h"

void SaveFile::load(const std::string path) {
	s_file.open(path, std::ios::in);

	if (!s_file.good()) {
		s_file.close();
		save(path);
		return;
	}

	std::string lines[3];

	for (int i = 0; i < 3; i++) {
		std::getline(s_file, lines[i]);
	}

	Globals::bestTime = atof(lines[0].c_str());
	Globals::soundVolume = atof(lines[1].c_str());
	Globals::musicVolume = atof(lines[2].c_str());
	s_file.close();
}

void SaveFile::save(const std::string path) {
	s_file.open(path, std::ios::out);
	s_file << Globals::bestTime << '\n';
	s_file << Globals::soundVolume << '\n';
	s_file << Globals::musicVolume << '\n';
	s_file.close();
}

std::fstream SaveFile::s_file;