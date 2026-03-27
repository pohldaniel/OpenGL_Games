#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "CharacterSet.h"

const Char& CharacterSet::getCharacter(const char c) const {
	return characters.at(c);
}

void CharacterSet::loadMsdfFromFile(const std::string& pathJson, const std::string& pathTexture) {
	std::ifstream file(pathJson, std::ios::in);
	if (!file.is_open()) {
		std::cout << "Could not open file: " << pathJson << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	float size = doc["atlas"]["size"].GetFloat();
	float width = doc["atlas"]["width"].GetFloat();
	float height = doc["atlas"]["height"].GetFloat();

	distanceRange = doc["atlas"]["distanceRange"].GetFloat();
	lineHeight = static_cast<unsigned int>(doc["metrics"]["lineHeight"].GetFloat() * size) - distanceRange * 2.5f;

	for (rapidjson::Value::ConstValueIterator glyph = doc["glyphs"].GetArray().Begin(); glyph != doc["glyphs"].GetArray().End(); ++glyph) {
		unsigned int code = glyph->HasMember("unicode") ? (*glyph)["unicode"].GetUint() : glyph->HasMember("index") ? (*glyph)["index"].GetUint() : 0;

		float advance = glyph->HasMember("advance") ? (*glyph)["advance"].GetFloat() : 0.0f;

		float pleft = glyph->HasMember("planeBounds") ? (*glyph)["planeBounds"]["left"].GetFloat() : 0.0f;
		float pright = glyph->HasMember("planeBounds") ? (*glyph)["planeBounds"]["right"].GetFloat() : 0.0f;
		float ptop = glyph->HasMember("planeBounds") ? (*glyph)["planeBounds"]["top"].GetFloat() : 0.0f;
		float pbottom = glyph->HasMember("planeBounds") ? (*glyph)["planeBounds"]["bottom"].GetFloat() : 0.0f;

		float aleft = glyph->HasMember("atlasBounds") ? (*glyph)["atlasBounds"]["left"].GetFloat() : 0.0f;
		float aright = glyph->HasMember("atlasBounds") ? (*glyph)["atlasBounds"]["right"].GetFloat() : 0.0f;
		float atop = glyph->HasMember("atlasBounds") ? (*glyph)["atlasBounds"]["top"].GetFloat() : 0.0f;
		float abottom = glyph->HasMember("atlasBounds") ? (*glyph)["atlasBounds"]["bottom"].GetFloat() : 0.0f;

		characters.insert(std::pair<char, Char>(code,
			{ (pright - pleft) * size, (ptop - pbottom) * size,
			  (aleft + 0.5f) / width, (abottom + 0.5f) / height,
			  ((aright - aleft) - 1.0f) / width, ((atop - abottom) - 1.0f) / height,
			  pleft * size, pbottom * size + distanceRange,
			  advance * size + advance * distanceRange * 0.75f
			}));
	}
	m_texture.loadFromFile(pathTexture);
}

void CharacterSet::loadMsdfBmFromFile(const std::string& pathJson, const std::string& pathTexture) {
	std::ifstream file(pathJson, std::ios::in);
	if (!file.is_open()) {
		std::cout << "Could not open file: " << pathJson << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	float size = doc["info"]["size"].GetFloat();
	float widthT = doc["common"]["scaleW"].GetFloat();
	float heightT = doc["common"]["scaleH"].GetFloat();

	distanceRange = doc["distanceField"]["distanceRange"].GetFloat();
	lineHeight = static_cast<unsigned int>(doc["common"]["lineHeight"].GetFloat()) + distanceRange * 0.5f;
	float heightMax = -FLT_MAX;

	for (rapidjson::Value::ConstValueIterator glyph = doc["chars"].GetArray().Begin(); glyph != doc["chars"].GetArray().End(); ++glyph) {
		char code = glyph->HasMember("id") ? (*glyph)["id"].GetUint() : 0;

		float advance = glyph->HasMember("xadvance") ? (*glyph)["xadvance"].GetFloat() : 0.0f;


		float posX = glyph->HasMember("x") ? (*glyph)["x"].GetFloat() : 0.0f;
		float posY = glyph->HasMember("y") ? (*glyph)["y"].GetFloat() : 0.0f;
		float width = glyph->HasMember("width") ? (*glyph)["width"].GetFloat() : 0.0f;
		float height = glyph->HasMember("height") ? (*glyph)["height"].GetFloat() : 0.0f;

		float offsetX = glyph->HasMember("xoffset") ? (*glyph)["xoffset"].GetFloat() : 0.0f;
		float offsetY = glyph->HasMember("yoffset") ? (*glyph)["yoffset"].GetFloat() : 0.0f;

		heightMax = (std::max)(height, heightMax);

		characters.insert(std::pair<char, Char>(code,
			{ width, height,
			  (posX + 0.5f) / widthT, (heightT - posY - height + 0.5f) / heightT,
			  (width - 1.0f) / widthT, (height - 1.0f) / heightT,
			  offsetX, -offsetY,
			  advance + distanceRange * 0.5f
			}));
	}

	for (auto& pair : characters) {
		pair.second.offset[1] += (heightMax - pair.second.size[1]);
	}
	m_texture.loadFromFile(pathTexture);
}