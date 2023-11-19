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
	//paddingX = paddingX == 0 ? 1 : paddingX;

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else {
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

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &spriteSheet);
		glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, maxWidth, maxHeight, 1, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Paste all glyph bitmaps into the texture, remembering the offset 
		unsigned int ox = 0;
		unsigned int oy = paddingY;
		int yOffset = 0;
		rowh = 0;

		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}

			if (ox + g->bitmap.width >= maxWidth) {
				oy += rowh;
				rowh = 0;
				ox = paddingX;
			}

			if (flipVertical) {
				std::vector<unsigned char> srcPixels(g->bitmap.width * g->bitmap.rows);
				//memcpy(&srcPixels[0], g->bitmap.buffer, g->bitmap.width * g->bitmap.rows);

				for (unsigned int i = 0; i < g->bitmap.width * g->bitmap.rows; ++i) {
					srcPixels[i] = g->bitmap.buffer[i];
				}

				unsigned char *pSrcRow = 0;
				unsigned char *pDestRow = 0;

				for (unsigned int i = 0; i < g->bitmap.rows; ++i) {

					pSrcRow = &srcPixels[(g->bitmap.rows - 1 - i) * g->bitmap.width];
					pDestRow = &g->bitmap.buffer[i * g->bitmap.width];
					memcpy(pDestRow, pSrcRow, g->bitmap.width);
				}
			}

			yOffset = g->bitmap.rows - g->bitmap_top;

			unsigned int height = yOffset >= 0 ? g->bitmap.rows + maxDescent : g->bitmap.rows + maxDescent - yOffset;

			unsigned char* bytes = (unsigned char*)malloc(g->bitmap.width * height);
			unsigned int index = 0;

			for (unsigned int i = 0; i < g->bitmap.width * (maxDescent - yOffset); i++, index++) {
				bytes[index] = 0;
			}

			for (unsigned int i = 0; i < g->bitmap.width * g->bitmap.rows; i++, index++) {
				bytes[index] = g->bitmap.buffer[i];
			}

			for (unsigned int i = 0; i < g->bitmap.width * std::max(0, yOffset); i++, index++) {
				bytes[index] = 0;
			}

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, ox, oy, 0, g->bitmap.width, height, 1, GL_RED, GL_UNSIGNED_BYTE, bytes);

			free(bytes);

			Char character = {
				{ g->bitmap_left, g->bitmap_top },
				{ g->bitmap.width, height },				
				//{ static_cast<float>(ox - shiftX) / (float)maxWidth, static_cast<float>(oy) / (float)maxHeight },
				{ static_cast<float>(ox) / (float)maxWidth, static_cast<float>(oy) / (float)maxHeight },
				{ static_cast<float>(g->bitmap.width ) / static_cast<float>(maxWidth), static_cast<float>(height) / static_cast<float>(maxHeight) },
				{ static_cast<unsigned int>((g->advance.x >> 6) +  spacing) }
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

void CharacterSet::safeFont() {
	unsigned char* bytes = (unsigned char*)malloc(maxWidth * maxHeight * sizeof(unsigned char));

	glBindTexture(GL_TEXTURE_2D_ARRAY, spriteSheet);
	//glGetTextureSubImage(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, maxWidth, maxWidth, 1, GL_RED, GL_UNSIGNED_BYTE, sizeof(unsigned char) * maxWidth * maxWidth, bytes);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	
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

	for (unsigned int i = 0, k = 0; i < maxWidth * maxHeight * 4; i = i + 4, k = k++) {
		bytesNew[i] = (int)bytes[k];
		bytesNew[i + 1] = (int)bytes[k];
		bytesNew[i + 2] = (int)bytes[k];
		bytesNew[i + 3] = 255;
	}

	Texture::Safe("font2.png", bytesNew, maxWidth, maxHeight, 4);

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