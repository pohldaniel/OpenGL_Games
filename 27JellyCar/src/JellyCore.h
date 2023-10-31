#pragma once

#include <_Andromeda/Text/TextureAtlas.h>
#include <_Andromeda/Text/TexturedFont.h>
#include <_Andromeda/Shader2.h>

class JellyCore {

public:

	const char * _cache = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	//const char * _titleCache = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	//atlases
	TextureAtlas* _menuAtlas;

	//fonts
	TexturedFont* _menuFont;

	//
	TexturedFont* _smallFont;

	//title
	TexturedFont* _titleFont;

	//shader
	Shader2* _spriteShader;

public:

	JellyCore();
	~JellyCore();

	void Init();

};
