#ifndef __CHARACTERSETH__
#define __CHARACTERSETH__
#define NOMINMAX

#include <GL/glew.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <ft2build.h>
#include FT_FREETYPE_H

#define MAXWIDTH 4096

struct CharacterOld {
	unsigned int textureID;
	int size[2];
	int bearing[2];
	unsigned int advance;
};

struct CharacterSetOld {	
	~CharacterSetOld();
	void loadFromFile(const std::string& path, const float characterSize = 100.0f);
	std::map<GLchar, CharacterOld> characters;
	float characterSize;
};

struct Character {
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

	void loadFromFile(const std::string& path, const float characterSize = 100.0f);
	std::map<GLchar, Character> characters;
	unsigned int spriteSheet;
	unsigned int maxWidth;
	unsigned int maxHeight;
	unsigned int lineHeight;
};

#endif