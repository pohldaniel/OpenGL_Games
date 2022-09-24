#ifndef __CHARACTERSETH__
#define __CHARACTERSETH__
#define NOMINMAX
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Extension.h"
#include "Texture.h"

#define MAXWIDTH 1024

struct Char {
	int bearing[2];
	int size[2];
	float textureOffset[2];
	float textureSize[2];
	unsigned int advance[2];	
};

struct CharacterSet {
	CharacterSet() = default;
	CharacterSet(CharacterSet const& rhs);
	CharacterSet& operator=(const CharacterSet& rhs);
	~CharacterSet();

	void loadFromFile(const std::string& path, const float characterSize, unsigned int intspacingX = 1, unsigned int spacingY = 10, const bool flipVertical = true);
	void safeFont();
	std::map<GLchar, Char> characters;
	unsigned int spriteSheet;
	unsigned int maxWidth;
	unsigned int maxHeight;
	unsigned int lineHeight;

	const Char& getCharacter(const char c) const;
};

#endif