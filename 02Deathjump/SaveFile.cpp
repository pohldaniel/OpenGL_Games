#include "SaveFile.h"

void SaveFile::load(const std::string path) {
	s_file.open(path, std::ios::in);

	if (!s_file.good()) {
		s_file.close();
		save(path);
		return;
	}

	std::string lines[1];

	for (int i = 0; i < 1; i++) {
		std::getline(s_file, lines[i]);
	}

	Globals::bestTime = atof(lines[0].c_str());

	/*Extern::sound_volume = atoi(lines[1].c_str());

	sf::Color colors[2];

	for (int i = 2; i < 4; i++) {
		int color[3] = { 0 };
		int j = 0;

		std::string str;
		for (char& key : lines[i]) {
			if (key != ' ') {
				str += key;
			}
			else {
				color[j] = atoi(str.c_str());
				j++;
				str.clear();
			}
		}

		colors[i - 2] = sf::Color(color[0], color[1], color[2], 255);
	}

	Extern::particle_birth_color = colors[0];
	Extern::particle_death_color = colors[1];

	Extern::best_time = atof(lines[4].c_str());*/

	s_file.close();
}

void SaveFile::save(const std::string path) {
	s_file.open(path, std::ios::out);

	/*s_file << Extern::music_volume << '\n';
	s_file << Extern::sound_volume << '\n';
	s_file << (int)Extern::particle_birth_color.r << ' ' << (int)Extern::particle_birth_color.g << ' ' << (int)Extern::particle_birth_color.b << ' ' << '\n';
	s_file << (int)Extern::particle_death_color.r << ' ' << (int)Extern::particle_death_color.g << ' ' << (int)Extern::particle_death_color.b << ' ' << '\n';*/
	s_file << Globals::bestTime << '\n';
	std::cout << Globals::bestTime << std::endl;
	s_file.close();
}

std::fstream SaveFile::s_file;