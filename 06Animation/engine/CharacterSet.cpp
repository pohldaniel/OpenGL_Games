#include "CharacterSet.h"

CharacterSetOld::~CharacterSetOld() {

}

void CharacterSetOld::loadFromFile(const std::string& path, const float _characterSize) {
	characterSize = _characterSize;
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}else {
		
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, characterSize);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		int x = 0;
		// load first 128 characters of ASCII set
		for (unsigned char p = 0; p < 128; p++) {
			// Load character glyph 
			if (FT_Load_Char(face, p, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			

			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);

			
			x += face->glyph->bitmap.width;
			
			// now store character for later use			
			CharacterOld character = { texture,
			{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
			{ face->glyph->bitmap_left, face->glyph->bitmap_top },
				static_cast<unsigned int>(face->glyph->advance.x) };

			characters.insert(std::pair<char, CharacterOld>(p, character));
		}
		
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

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

void CharacterSet::loadFromFile(const std::string& path, const float characterSize) {
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
		maxWidth = 0;
		maxHeight = 0;
		lineHeight = 0;


		// Find minimum size for a texture holding all visible ASCII characters
		for (int i = 0; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}
			if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
				maxWidth = (std::max)(maxWidth, roww);
				maxHeight += rowh;
				roww = 0;
				rowh = 0;
			}
			roww += g->bitmap.width + 1;
			rowh = (std::max)(rowh, g->bitmap.rows);
			lineHeight = std::max(lineHeight, face->glyph->bitmap.rows);
			
		}

		maxWidth = (std::max)(maxWidth, roww);
		maxHeight += rowh;

		// Create a texture that will be used to hold all ASCII glyphs
		//glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &spriteSheet);
		glBindTexture(GL_TEXTURE_2D, spriteSheet);
		//glUniform1i(uniform_tex, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, maxWidth, maxHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		// We require 1 byte alignment when uploading texture data 
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Clamping to edges is important to prevent artifacts when scaling 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Linear filtering usually looks best for text 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Paste all glyph bitmaps into the texture, remembering the offset 
		int ox = 0;
		int oy = 0;

		rowh = 0;

		for (int i = 32; i < 128; i++) {
			if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
				fprintf(stderr, "Loading character %c failed!\n", i);
				continue;
			}

			if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
				oy += rowh;
				rowh = 0;
				ox = 0;
			}

			glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
			
			Character character = {				
				{g->bitmap_left, g->bitmap_top },
				{g->bitmap.width , g->bitmap.rows },
				{ox / (float)maxWidth, oy / (float)maxHeight },
				{g->bitmap.width / (float)maxWidth, g->bitmap.rows / (float)maxHeight},
				{g->advance.x >> 6, g->advance.y >> 6}
				
			};
			characters.insert(std::pair<char, Character>(i, character));

			rowh = (std::max)(rowh, g->bitmap.rows);
			ox += g->bitmap.width + 1;
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", maxWidth, maxHeight, maxWidth * maxHeight / 1024);
	}
}