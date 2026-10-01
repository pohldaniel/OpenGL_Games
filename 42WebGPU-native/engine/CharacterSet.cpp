#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <ft2build.h>
#include <freetype/freetype.h>

#include <WebGPU/WgpContext.h>

#include "CharacterSet.h"

const Char& CharacterSet::getCharacter(const char c) const {
	return characters.at(c);
}

const std::vector<Kerning>& CharacterSet::getKernings(const char c) const {
	return kernings.at(c);
}

bool CharacterSet::hasKernings() const {
	return !kernings.empty();
}

bool CharacterSet::kerningsHasChar(const char c) const {
	return kernings.count(c);
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
			{ pleft * size, pbottom * size + distanceRange,
			  (pright - pleft) * size, (ptop - pbottom) * size,
			  (aleft + 0.5f) / width, (abottom + 0.5f) / height,
			  ((aright - aleft) - 1.0f) / width, ((atop - abottom) - 1.0f) / height,			  
			  advance
			}));
	}
	texture.loadFromFile(pathTexture);
	texture.markForDelete();
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
		char code = (*glyph)["char"].GetString()[0];
		float advance = glyph->HasMember("xadvance") ? (*glyph)["xadvance"].GetFloat() : 0.0f;
		float posX = glyph->HasMember("x") ? (*glyph)["x"].GetFloat() : 0.0f;
		float posY = glyph->HasMember("y") ? (*glyph)["y"].GetFloat() : 0.0f;
		float width = glyph->HasMember("width") ? (*glyph)["width"].GetFloat() : 0.0f;
		float height = glyph->HasMember("height") ? (*glyph)["height"].GetFloat() : 0.0f;
		float offsetX = glyph->HasMember("xoffset") ? (*glyph)["xoffset"].GetFloat() : 0.0f;
		float offsetY = glyph->HasMember("yoffset") ? (*glyph)["yoffset"].GetFloat() : 0.0f;

		heightMax = (std::max)(height, heightMax);

		characters.insert(std::pair<char, Char>(code,
			{ offsetX, -offsetY,
			  width, height,
			  (posX - 0.5f) / widthT, (heightT - posY - height - 0.5f) / heightT,
			  (width + 1.0f) / widthT, (height + 1.0f) / heightT,			  
			  advance
			}));
	}

	for (auto& pair : characters) {
		pair.second.pos[1] += (heightMax - pair.second.size[1]);
	}

	for (rapidjson::Value::ConstValueIterator kerning = doc["kernings"].GetArray().Begin(); kerning != doc["kernings"].GetArray().End(); ++kerning) {
		char first = kerning->HasMember("first") ? (*kerning)["first"].GetUint() : 0;
		char second = kerning->HasMember("second") ? (*kerning)["second"].GetUint() : 0;
		float advance = kerning->HasMember("amount") ? (*kerning)["amount"].GetFloat() : 0.0f;
		kernings[first].push_back({ second , advance });
	}

	texture.loadFromFile(pathTexture);
	texture.markForDelete();
}

void CharacterSet::loadFromFile(const std::string& path, uint32_t characterSize) {
	unsigned int paddingX = 0u;
	unsigned int paddingY = 10u;
	bool flipVertical = false;
	int spacing = 0;

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return;
	}
	
	FT_Set_Pixel_Sizes(face, 0, characterSize);
	FT_GlyphSlot glyph = face->glyph;

	unsigned int roww = 0;
	unsigned int rowh = 0;
	int maxDescent = 0;
	int maxAscent = 0;

	unsigned int maxWidth = 0;
	unsigned int maxHeight = 0;
	lineHeight = 0.0f;

	for (int i = 32; i < 128; i++) {

		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			fprintf(stderr, "Loading character %c failed!\n", i);
			continue;
		}
		if (roww + glyph->bitmap.width + paddingX >= MAXWIDTH) {
			maxWidth = std::max(maxWidth, roww);
			maxHeight += rowh;
			roww = 0;
			rowh = 0;
		}
		roww += glyph->bitmap.width + paddingX;
		rowh = std::max(rowh, glyph->bitmap.rows + paddingY);

		maxAscent = std::max(glyph->bitmap_top, maxAscent);
		maxDescent = std::max((int)glyph->bitmap.rows - glyph->bitmap_top, maxDescent);
	}

	lineHeight = maxAscent + maxDescent;
	maxWidth = std::max(maxWidth, roww);
	maxHeight += rowh;

	unsigned int p = 1;
	while (p < maxWidth)
		p <<= 1;
	maxWidth = p;

	p = 1;
	while (p < maxHeight)
		p <<= 1;
	maxHeight =  p;

	std::vector<uint8_t> atlasBuffer(maxWidth * maxHeight, 0);
	unsigned int ox = 0u;
	unsigned int oy = paddingY;
	int yOffset = 0;
	rowh = 0u;
	
	for (int i = 32; i < 128; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			continue;
		}

		if (ox + glyph->bitmap.width >= maxWidth) {
			oy += rowh;
			rowh = 0;
			ox = paddingX;
		}

		if (flipVertical) {
			std::vector<unsigned char> srcPixels(glyph->bitmap.width * glyph->bitmap.rows);

			for (unsigned int i = 0; i < glyph->bitmap.width * glyph->bitmap.rows; ++i) {
				srcPixels[i] = glyph->bitmap.buffer[i];
			}

			unsigned char* pSrcRow = 0;
			unsigned char* pDestRow = 0;

			for (unsigned int i = 0; i < glyph->bitmap.rows; ++i) {

				pSrcRow = &srcPixels[(glyph->bitmap.rows - 1 - i) * glyph->bitmap.width];
				pDestRow = &glyph->bitmap.buffer[i * glyph->bitmap.width];
				memcpy(pDestRow, pSrcRow, glyph->bitmap.width);
			}
		}

		for (unsigned int r = 0; r < glyph->bitmap.rows; ++r) {
			uint8_t* destRow = &atlasBuffer[(oy + r) * maxWidth + ox];
			uint8_t* srcRow = &glyph->bitmap.buffer[r * glyph->bitmap.width];
			std::memcpy(destRow, srcRow, glyph->bitmap.width);
		}

		Char character = {
			{ (float)glyph->bitmap_left, (float)glyph->bitmap_top - (float)lineHeight },
			{ (float)glyph->bitmap.width, (float)glyph->bitmap.rows },
			{ (static_cast<float>(ox) + 0.5f) / (float)maxWidth, (static_cast<float>(oy) + 0.5f) / (float)maxHeight },
			{ (static_cast<float>(glyph->bitmap.width) - 1.0f) / static_cast<float>(maxWidth), (static_cast<float>(glyph->bitmap.rows) - 1.0f) / static_cast<float>(maxHeight) },
			static_cast<float>((glyph->advance.x >> 6) + spacing)
		};

		characters.insert(std::pair<char, Char>(i, character));

		rowh = (std::max)(rowh, glyph->bitmap.rows + paddingY);
		ox += glyph->bitmap.width + paddingX;
	}

	texture.createEmpty(maxWidth, maxHeight, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat_R8Unorm, 1u, 1u, true);

	WGPUTexelCopyTextureInfo destination = {};
	destination.texture = texture.getTexture();
	destination.mipLevel = 0u;
	destination.origin = { 0u, 0u, 0u };
	destination.aspect = WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout dataLayout = {};
	dataLayout.offset = 0u;
	dataLayout.bytesPerRow = maxWidth;
	dataLayout.rowsPerImage = maxHeight;

	WGPUExtent3D writeSize = { maxWidth, maxHeight, 1u };

	wgpuQueueWriteTexture(wgpContext.queue, &destination, atlasBuffer.data(), atlasBuffer.size(), &dataLayout, &writeSize);
	//WgpTexture::Safe("tmp.png", atlasBuffer.data(), maxWidth, maxHeight, 1u);
	texture.markForDelete();
}

float CharacterSet::getWidth(const std::string& text) const {
	float sizeX = 0.0f;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		float kerningAmount = 0.0f;
		if (hasKernings() && kerningsHasChar(*c) && (c + 1) != text.end()) {
			const std::vector<Kerning>& kernings = getKernings(*c);
			for (const Kerning& kerning : kernings) {
				if (kerning.nextChar == *(c + 1)) {
					kerningAmount = kerning.amount;
				}
			}
		}
		const Char ch = getCharacter(*c);
		sizeX = sizeX + ch.advance + kerningAmount;
	}
	return  sizeX;
}