#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cstdarg>
#include <sstream>
#include <algorithm>

#include "engine/Extension.h"

class FTLibraryContainer{

public:
	FTLibraryContainer();
	~FTLibraryContainer();

	FT_Library& getLibrary();

private:
	static FT_Library library_;
};

class StreamFlusher { };
std::ostream& operator<<(std::ostream& os, const StreamFlusher& rhs);

class GLFT_Font{

public:
	GLFT_Font();
	GLFT_Font(const std::string& filename, unsigned int size);

	~GLFT_Font();

	void open(const std::string& filename, unsigned int size, bool isOpenGLThreadInThreadedMode = false);
	void release();

	bool isValid() const;

	void drawText(int x, int y, const char *str, ...) const;
	void drawText(int x, int y, const std::string& str) const;

	std::ostream& beginDraw(float x, float y);
	StreamFlusher endDraw();

	unsigned int calcStringWidth(const char *str, ...) const;
	unsigned int calcStringWidth(const std::string& str) const;
	unsigned int getHeight() const;

private:
	// leave copy constructor and operator= undefined to make noncopyable
	GLFT_Font(const GLFT_Font&);
	const GLFT_Font& operator=(const GLFT_Font&);

private:
	// font data
	unsigned int texID_;
	unsigned int listBase_;
	std::vector<unsigned char> widths_;
	unsigned char height_;
	// stream drawing stuff
	std::ostringstream ss_;
	float drawX_;
	float drawY_;

	static const unsigned int SPACE = 32;
	static const unsigned int NUM_CHARS = 96;

	static FTLibraryContainer library_;
};
