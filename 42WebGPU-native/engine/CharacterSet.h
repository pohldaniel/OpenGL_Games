#pragma once

#include <map>
#include <string>
#include <WebGPU/WgpTexture.h>

struct Char {
	float size[2];
	float textureOffset[2];
	float textureSize[2];
	float offset[2];
	float advance;
};

struct Kerning {
	char nextChar;
	float amount;
};

struct CharacterSet {

	void loadMsdfFromFile(const std::string& pathJson, const std::string& pathTexture);
	void loadMsdfBmFromFile(const std::string& pathJson, const std::string& pathTexture);
	const Char& getCharacter(const char c) const;
	const std::vector<Kerning>& getKernings(const char c) const;
	const bool hasKernings() const;
	const bool kerningsHasChar(const char c) const;
	const float getWidth(const std::string& text) const;

	std::map<char, Char> characters;
	std::map<char, std::vector<Kerning>> kernings;
	uint32_t layer;
	float distanceRange;
	float lineHeight;

	WgpTexture m_texture;
};