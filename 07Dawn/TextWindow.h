#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "engine/Fontrenderer.h"
#include "engine/Clock.h"

#include "Widget.h"
#include "Dialog.h"
#include "Luainterface.h"
#include "Enums.h"

class TextWindow : public Widget {

public:

	void setText(std::string text);
	void setAutocloseTime(unsigned int autocloseTime);
	void setPosition(Enums::PositionType, int x, int y);
	void center();
	void setOnCloseText(std::string onCloseText);
	bool canBeDeleted() const;
	void processInput();
	void draw();
	void close() override;

	static void  Update();
	static void FormatMultilineText(std::string textIn, std::vector< std::string > &textLines, int lineWidth, CharacterSet* font);
	static std::vector<TextWindow*>& GetTextWindows();
	static void AddTextWindow(TextWindow* textWindow);
	static void RemoveTextWindow(unsigned short index);
	static void Resize(int deltaW, int deltaH);

private:
	friend TextWindow* DawnInterface::createTextWindow();

	TextWindow();
	bool isMouseOnFrame(int mouseX, int mouseY) const override;
	void updateFramesPosition();

	Enums::PositionType positionType;
	int x;
	int y;
	unsigned int autocloseTime;

	std::vector<std::string> textLines;
	std::string executeTextOnClose;
	bool explicitClose;

	Clock m_timer;

	const int blockSizeX = 32;
	const int blockSizeY = 32;
	const int lineWidth = 416;
	int neededInnerBlocksX;
	int neededInnerBlocksY;

	static std::vector<TextWindow*> s_textWindows;
	static CharacterSet& Font;
};
