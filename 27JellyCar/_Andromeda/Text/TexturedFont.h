#ifndef DynamicTextureFont_H
#define DynamicTextureFont_H

#ifdef ANDROMEDA_GL3
	#include <ft2build.h>
	#include FT_FREETYPE_H
#endif

#include "TextureAtlas.h"
#include "../VertexArrayObject.h"
#include "../Shader2.h"

#include <string>
#include <texture-font.h>
#include <vector>
#include "TextPart.h"
using namespace ftgl;

#include <glm/glm.hpp>

#ifdef ANDROMEDA_SWITCH
	#include <ft2build.h>
	#include FT_FREETYPE_H
#endif



#ifdef ANDROMEDA_VITA
	#include <ft2build.h>
	#include FT_FREETYPE_H
#endif


class TexturedFont{
private:


	//ftgl font
	texture_font_t* _font;

	//atlas
	TextureAtlas* _atlas;

	//vertex array
	VertexArrayObject* _vertexArray;

	//memory buffer
	unsigned char* _buffer;

	//shader
	Shader2* _shader;

	TextureFontAlign _fontAlign;

	std::string _text;

	glm::vec2 _position;
	glm::vec4 _color;

	glm::mat4* _projection;

	std::vector<TextPart> _textParts;
			
	int _lastLenght;
	int _maxLenght;
	float _lastColour;
	float _lastPosition;
	size_t _lastHash;

	bool _draw;

private:

	//distance field stuff
	static float MitchellNetravali(float x);
	static float interpolate(float x, float y0, float y1, float y2, float y3);
	static int resize(double* src_data, size_t src_width, size_t src_height, double* dst_data, size_t dst_width, size_t dst_height);

private:

	//load font from memory
	void LoadFromMemory(float size, void* memory_font, size_t memory_size);

	void LoadDistanceFiledFont(float size, std::string fileName);
			
	//create vertex array object
	void CreateVertexObject();

	float GetTextLenght(std::string text);

	void PrepareBuffer();

	unsigned char ComputeY(int r, int g, int b);

	unsigned char* Convert(unsigned char* data, int img_n, int req_comp, int x, int y);

public:

	TexturedFont(TextureAtlas* atlas, float size, std::string filename);
	TexturedFont(float size, std::string filename);
	~TexturedFont();

	static bool CreateDistanceFieldFont(float size, float bigSize, int atlasSize, std::string cache, std::string filename);
	static texture_glyph_t* CreateDistanceGlymph(FT_Library& library, TextureAtlas* atlas, const char* filename, const char* codepoint, const float   highres_size, const float   lowres_size, const float   padding);


	int CacheGlyphs(std::string cache);

	void SetShader(Shader2* shader);

	void SetAlign(TextureFontAlign fontAlign);

	void SetPosition(glm::vec2 position);
	void SetSolor(glm::vec4 color);
	void SetText(std::string text);

	void AddText(std::string text, int x, int y, TextureFontAlign align);
	void AddText(std::string text, int x, int y, glm::vec3 color, TextureFontAlign align);
	void AddText(std::string text, glm::vec2 position, glm::vec3 color, TextureFontAlign align);

	void Draw(glm::mat4 &projection);
	void Draw(glm::mat4 &projection, std::string text, int x, int y, TextureFontAlign align);
};
#endif
