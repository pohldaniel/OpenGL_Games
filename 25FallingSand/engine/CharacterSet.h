#ifndef __CHARACTERSETH__
#define __CHARACTERSETH__

#include <GL/glew.h>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <ft2build.h>
#include <freetype/freetype.h>

#define MAXWIDTH 1024

struct Char {
	int bearing[2];
	unsigned int size[2];
	float textureOffset[2];
	float textureSize[2];
	unsigned int advance;
};

struct CharacterSet {
	CharacterSet() = default;
	CharacterSet(CharacterSet const& rhs);
	CharacterSet& operator=(const CharacterSet& rhs);
	~CharacterSet();

	void loadFromFile(const std::string& path, unsigned int characterSize, unsigned int intspacingX = 1u, unsigned int spacingY = 10u, unsigned int minHeight = 0u, int shiftX = 0, const bool flipVertical = true, unsigned int frame = 0u);
	void safeFont();
	std::map<GLchar, Char> characters;
	unsigned int spriteSheet;
	unsigned int maxWidth;
	unsigned int maxHeight;
	unsigned int lineHeight;
	unsigned int frame;

	const Char& getCharacter(const char c) const;
	int getWidth(std::string text) const;
	void bind(unsigned int unit = 0u) const;
};

#endif