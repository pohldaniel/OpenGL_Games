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

struct CharacterSet {

	void loadMsdfFromFile(const std::string& pathJson, const std::string& pathTexture);
	void loadMsdfBmFromFile(const std::string& pathJson, const std::string& pathTexture);
	const Char& getCharacter(const char c) const;

	std::map<char, Char> characters;
	uint32_t layer;
	float distanceRange;
	uint32_t lineHeight;

	WgpTexture m_texture;
};