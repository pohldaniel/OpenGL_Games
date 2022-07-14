#pragma once

#include <iostream>
#include <map>
#include "engine/Texture.h"
#include "Constants.h"

struct CharacterBmp {
	int bearing[2];
	unsigned int size[2];
	float textureOffset[2];
	float textureSize[2];
	unsigned int xAdvance;
	unsigned int lineHeight;
};

struct CharacterSetBmp {
	CharacterSetBmp() = default;
	CharacterSetBmp(CharacterSetBmp const& rhs);
	CharacterSetBmp& operator=(const CharacterSetBmp& rhs);
	~CharacterSetBmp();

	void loadFromFile(const std::string& path);
	std::map<char, CharacterBmp> characters;
	Texture texture;
	unsigned int spriteSheet;
	unsigned int maxWidth;
	unsigned int maxHeight;
	unsigned int lineHeight;
};