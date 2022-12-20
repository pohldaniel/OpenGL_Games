#pragma once
#define NOMINMAX
#define TILE_WIDTH 64
#define TILE_HEIGHT 64

#include "Widget.h"
#include "TextureManager.h"

struct DialogCanvas {

	friend class Dialog;
	friend class SpellTooltip;
	friend class ItemTooltip;
	friend class TextWindow;
	friend class SelectionBox;
	friend class CheckBox;

	static void Init();
	static void Init(std::vector<TextureRect> textures);
private:
	
	static void DrawCanvas(int left, int bottom, int colummns, int rows, int tileWidth, int tileHeight, bool updateView = false, bool drawInChunk = false);
	static int CalculateNeededBlockWidth(int width, int tileWidth);
	static int CalculateNeededBlockHeight(int height, int tileHeight);

	static unsigned int TextureAtlas;
	static std::vector<TextureRect> Textures;
};

class Dialog : public Widget {
public:

	Dialog() = default;
	Dialog(int posX, int posY, int width, int height);	

	void draw() override;
	void processInput() override;

	void setAutoresize();
	void setCenteringLayout();
	void setCenterOnScreen();
	void applyLayout();

	void addChildWidget(int relPosX, int relPosY, std::auto_ptr<Widget> newChild) override;
	int getWidth() const override;
	int getHeight() const override;

private:
	
	void resize(int width, int height);
	void recalculatePosition();

	int m_columns;
	int m_rows;

	bool autoResize;
	bool centeringLayout;
	bool centerOnScreen;
};