#ifndef __CHARACTERSETH__
#define __CHARACTERSETH__

#include <iostream>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Extension.h"

struct Character {
	unsigned int textureID;
	int size[2];
	int bearing[2];
	unsigned int advance;
};

struct CharacterSet {	

	~CharacterSet();
	void loadFromFile(const std::string& path, const float characterSize = 100.0f);
	std::map<GLchar, Character> characters;
	float characterSize;
};

#endif