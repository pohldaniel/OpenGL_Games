#include "Text.h"

 void CharacterSet::load(std::string path) {
	if (init) return;

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
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, m_characterSize);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++) {
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
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
			// now store character for later use			
			Character character = {
				texture,
				{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
				{ face->glyph->bitmap_left, face->glyph->bitmap_top },
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

CharacterSet Text::characterSet = CharacterSet();

Text::Text(std::string label, float scale) {
	Text::characterSet.load("res/fonts/upheavtt.ttf");
	m_characters = characterSet.characters;
	

	m_shaderText = new Shader("shader/text.vs", "shader/text.fs");

	m_label = label;
	m_scale = scale;
	calcSize();
	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};
	
	short stride = 4;

	unsigned int indexQuad, m_quadVBO;
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);

	glGenBuffers(1, &indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &indexQuad);
}

Text::~Text() {}

void Text::render(Vector4f color) {

	glUseProgram(m_shaderText->m_program);
	m_shaderText->loadMatrix("projection", Globals::projection);
	m_shaderText->loadVector("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_vao);

	float x = m_position[0];
	float y = m_position[1];
	// iterate through all characters
	std::string::const_iterator c;
	for (c = m_label.begin(); c != m_label.end(); c++) {
		Character ch = m_characters[*c];

		float w = ch.size[0] * m_scale;
		float h = ch.size[1] * m_scale;


		float xpos = x;
		float ypos = (HEIGHT - characterSet.m_characterSize * 0.5f) - (y + (ch.size[1] - ch.bearing[1]) * m_scale);

		// update vbo for each character
		float vertices[] = {
			xpos,        ypos,      0.0f, 1.0f,
			xpos,       (ypos + h), 0.0f, 0.0f,
			(xpos + w), (ypos + h), 1.0f, 0.0f,
			(xpos + w),  ypos,      1.0f, 1.0f
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of vbo memory
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * m_scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::render(std::string text, Vector4f color) {

	glUseProgram(m_shaderText->m_program);
	m_shaderText->loadMatrix("projection", Globals::projection);
	m_shaderText->loadVector("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_vao);
	
	float x = m_position[0];
	float y = m_position[1];
	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++){
		Character ch = m_characters[*c];

		float w = ch.size[0] * m_scale;
		float h = ch.size[1] * m_scale;


		float xpos = x ;
		float ypos = (HEIGHT - characterSet.m_characterSize * 0.5f) - (y + (ch.size[1] - ch.bearing[1]) * m_scale);
	
		// update vbo for each character
		float vertices[] = {
			xpos,        ypos,      0.0f, 1.0f,
			xpos,       (ypos + h), 0.0f, 0.0f,
			(xpos + w), (ypos + h), 1.0f, 0.0f,
			(xpos + w),  ypos,      1.0f, 1.0f
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update content of vbo memory
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.advance >> 6) * m_scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::setPosition(const Vector2f &position) {
	m_position = position;
}

void Text::setOrigin(const Vector2f &origin) {
	m_origin = origin;
}

const Vector2f &Text::getPosition() const {
	return m_position;
}

const Vector2f &Text::getSize() const {
	return m_size;
}

void Text::calcSize() {
	
	int sizeX = 0;
	Character ch;

	std::string::const_iterator c;
	for (c = m_label.begin(); c != m_label.end(); c++) {
		ch = m_characters[*c];

		sizeX = sizeX + ((ch.advance >> 6) * m_scale);
	}

	m_size = Vector2f(sizeX, ch.size[1]);
}