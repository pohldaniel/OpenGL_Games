#include <GL/glew.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <engine/Spritesheet.h>
#include "CharacterSet.h"
#include "Texture.h"

CharacterSet::CharacterSet(CharacterSet const& rhs) {
	characters = rhs.characters;
	maxWidth = rhs.maxWidth;
	maxHeight = rhs.maxHeight;
	lineHeight = rhs.lineHeight;
}

CharacterSet& CharacterSet::operator=(const CharacterSet& rhs) {
	characters = rhs.characters;
	maxWidth = rhs.maxWidth;
	maxHeight = rhs.maxHeight;
	lineHeight = rhs.lineHeight;
	return *this;
}

CharacterSet::~CharacterSet() {
	if (spriteSheet) {
		glDeleteTextures(1, &spriteSheet);
		spriteSheet = 0;
	}
}

void CharacterSet::loadFromFile(const std::string& path, unsigned int characterSize, unsigned int paddingX, unsigned int paddingY, unsigned int minHeight, int spacing, const bool flipVertical, unsigned int _frame) {

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}else {
		FT_Set_Pixel_Sizes(face, 0, characterSize);
		FT_GlyphSlot g = face->glyph;
	
		unsigned int roww = 0;
		unsigned int rowh = 0;
		int maxDescent = 0;
		int maxAscent = 0;

		maxWidth = 0;
		maxHeight = 0;
		lineHeight = 0;

		// Find minimum size for a texture holding all visible ASCII characters
		for (int i = 32; i < 128; i++) {

			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}
			if (roww + g->bitmap.width + paddingX >= MAXWIDTH) {
				maxWidth = std::max(maxWidth, roww);
				maxHeight += rowh;
				roww = 0;
				rowh = 0;
			}
			roww += g->bitmap.width + paddingX;
			rowh = std::max(rowh, (int)g->bitmap.rows + paddingY);
			//lineHeight = std::max(lineHeight, g->bitmap.rows);

			maxAscent = std::max(g->bitmap_top, maxAscent);
			maxDescent = std::max((int)g->bitmap.rows - g->bitmap_top, maxDescent);
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
		maxHeight = minHeight == 0 ? p : std::max(p, minHeight);

		/*FT_Error err_code;
		FT_Stroker stroker;
		if (FT_Stroker_New(static_cast<FT_Library>(ft), &stroker) != 0){
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
			return;
		}

		FT_Face face;
		if (FT_New_Face(ft, path.c_str(), 0, &face)) {
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		}else {
			FT_Set_Pixel_Sizes(face, 0, characterSize);
			FT_GlyphSlot g = face->glyph;

			unsigned int roww = 0;
			unsigned int rowh = 0;
			int maxDescent = 0;
			int maxAscent = 0;

			maxWidth = 0;
			maxHeight = 0;
			lineHeight = 0;
			for (int i = 32; i < 128; i++) {

				if (FT_Load_Char(face, i, FT_LOAD_NO_BITMAP | FT_LOAD_TARGET_NORMAL)) {
					fprintf(stderr, "Loading character %c failed!\n", i);
					continue;
				}


				FT_Glyph glyphDescStroke;
				if (FT_Get_Glyph(face->glyph, &glyphDescStroke)) {
					fprintf(stderr, "Loading character %c failed!\n", i);
					continue;
				}

				static double outlineThickness = 2.0;
				FT_Stroker_Set(stroker, static_cast<FT_Fixed>(outlineThickness * static_cast<float>(1 << 6)), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
				if (err_code == 0)
					err_code = FT_Glyph_Stroke(&glyphDescStroke, stroker, true);

				if (err_code == 0)
					err_code = FT_Glyph_To_Bitmap(&glyphDescStroke, FT_RENDER_MODE_NORMAL, 0, 1);

				FT_BitmapGlyph glyph_bitmap;
				FT_Bitmap *bitmap_stroke = nullptr;
				if (err_code == 0){
					glyph_bitmap = (FT_BitmapGlyph)glyphDescStroke;
					bitmap_stroke = &glyph_bitmap->bitmap;

					if (roww + bitmap_stroke->width + paddingX >= MAXWIDTH) {
						maxWidth = std::max(maxWidth, roww);
						maxHeight += rowh;
						roww = 0;
						rowh = 0;
					}
					roww += bitmap_stroke->width + paddingX;
					rowh = std::max(rowh, (int)bitmap_stroke->rows + paddingY);

					maxAscent = std::max(glyph_bitmap->top, maxAscent);
					maxDescent = std::max((int)bitmap_stroke->rows - glyph_bitmap->top, maxDescent);
				}
			}
		}*/

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &spriteSheet);
		glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, maxWidth, maxHeight, 1, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Paste all glyph bitmaps into the texture, remembering the offset 
		unsigned int ox = 0;
		unsigned int oy = paddingY;
		int yOffset = 0;
		rowh = 0;
		
		//FT_Bitmap_Embolden(ft, &g->bitmap, 20, 20);

		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}

			//FT_Glyph glyphDesc = nullptr;
			//FT_Get_Glyph(face->glyph, &glyphDesc);
			//FT_Glyph_To_Bitmap(&glyphDesc, FT_RENDER_MODE_NORMAL, nullptr, 1);			
			//FT_Bitmap bitmap = reinterpret_cast<FT_BitmapGlyph>(glyphDesc)->bitmap;
			FT_Bitmap bitmap = g->bitmap;

			if (ox + bitmap.width >= maxWidth) {
				oy += rowh;
				rowh = 0;
				ox = paddingX;
			}

			if (flipVertical) {
				std::vector<unsigned char> srcPixels(bitmap.width * bitmap.rows);

				for (unsigned int i = 0; i < bitmap.width *bitmap.rows; ++i) {
					srcPixels[i] = bitmap.buffer[i];
				}

				unsigned char *pSrcRow = 0;
				unsigned char *pDestRow = 0;

				for (unsigned int i = 0; i < bitmap.rows; ++i) {

					pSrcRow = &srcPixels[(bitmap.rows - 1 - i) * bitmap.width];
					pDestRow = &bitmap.buffer[i * bitmap.width];
					memcpy(pDestRow, pSrcRow, bitmap.width);
				}
			}

			yOffset = bitmap.rows - g->bitmap_top;

			unsigned int height = yOffset >= 0 ? bitmap.rows + maxDescent : bitmap.rows + maxDescent - yOffset;

			unsigned char* bytes = (unsigned char*)malloc(bitmap.width * height);
			unsigned int index = 0;

			for (unsigned int i = 0; i < bitmap.width * (maxDescent - yOffset); i++, index++) {
				bytes[index] = 0;
			}

			for (unsigned int i = 0; i < bitmap.width * bitmap.rows; i++, index++) {
				bytes[index] = bitmap.buffer[i];				
			}

			for (unsigned int i = 0; i < bitmap.width * std::max(0, yOffset); i++, index++) {
				bytes[index] = 0;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, ox, oy, 0, bitmap.width, height, 1, GL_RED, GL_UNSIGNED_BYTE, bytes);

			free(bytes);
			Char character = {
				{ bitmap.width, height },
				{ (static_cast<float>(ox) + 0.5f) / static_cast<float>(maxWidth), (static_cast<float>(oy) + 0.5f) / static_cast<float>(maxHeight) },
				{ (static_cast<float>(bitmap.width) - 1.0f) / static_cast<float>(maxWidth), (static_cast<float>(height) - 1.0f) / static_cast<float>(maxHeight)  },				
				{0.0f, 0.0f},
				{ (g->advance.x >> 6) + spacing},
			};

			characters.insert(std::pair<char, Char>(i, character));


			rowh = (std::max)(rowh, g->bitmap.rows + paddingY);
			ox += g->bitmap.width + paddingX;
		}

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
	frame = _frame;
	//debugging 
	//safeFont();
}

void CharacterSet::loadMsdfFromFile(const std::string& pathJson, const std::string& pathTexture) {
	std::ifstream file(pathJson, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << pathJson << std::endl;
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
		unsigned int code = glyph->HasMember("unicode") ?  (*glyph)["unicode"].GetUint() : glyph->HasMember("index") ? (*glyph)["index"].GetUint() : 0;

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
	Spritesheet::LoadFromFile(spriteSheet, pathTexture, true);
}

void CharacterSet::loadMsdfBmFromFile(const std::string& pathJson, const std::string& pathTexture) {
	std::ifstream file(pathJson, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << pathJson << std::endl;
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

	Spritesheet::LoadFromFile(spriteSheet, pathTexture, true);
}

void CharacterSet::safeFont(std::string name) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char* bytes = (unsigned char*)malloc(maxWidth * maxHeight * sizeof(unsigned char));
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	std::vector<unsigned char> srcPixels(maxWidth * maxHeight);
	memcpy(&srcPixels[0], bytes, maxWidth * maxHeight);

	unsigned char *pSrcRow = 0;
	unsigned char *pDestRow = 0;

	for (unsigned int i = 0; i < maxHeight; ++i) {
		pSrcRow = &srcPixels[(maxHeight - 1 - i) * maxWidth];
		pDestRow = &bytes[i * maxWidth];
		memcpy(pDestRow, pSrcRow, maxWidth);
	}

	unsigned char* bytesNew = (unsigned char*)malloc(maxWidth * maxHeight * 4);

	for (unsigned int i = 0, k = 0; i < maxWidth * maxHeight * 4; i = i + 4, k++) {
		bytesNew[i] = (int)bytes[k];
		bytesNew[i + 1] = (int)bytes[k];
		bytesNew[i + 2] = (int)bytes[k];
		bytesNew[i + 3] = 255;
	}

	Texture::Safe((name + ".png").c_str(), bytesNew, maxWidth, maxHeight, 4);

	free(bytes);
	free(bytesNew);	
}

void CharacterSet::addSpacing(std::string chars, int spacing) {

	std::string::iterator c;
	for (c = chars.begin(); c != chars.end(); c++) {
		Char& ch = characters.at(*c);
		ch.advance += spacing;
	}
}

const Char& CharacterSet::getCharacter(const char c) const {
	return characters.at(c);
}

int CharacterSet::getWidth(std::string text) const {
	int sizeX = 0;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		const Char ch = getCharacter(*c);
		sizeX = sizeX + ((ch.advance));
	}
	return  sizeX;
}

void CharacterSet::bind(unsigned int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
}

void CharacterSet::setRepeat() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void CharacterSet::setNearest() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void CharacterSet::setLinear() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void CharacterSet::setLinearMipMap() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}