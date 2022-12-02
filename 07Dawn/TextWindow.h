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

class TextWindow {
public:
	void setText(std::string text);
	void setAutocloseTime(unsigned int autocloseTime);
	void setPosition(Enums::PositionType, int x, int y);
	void center();
	void setOnCloseText(std::string onCloseText);
	bool canBeDeleted() const;
	void close();

	void processInput();
	void draw();

	static void FormatMultilineText(std::string textIn, std::vector< std::string > &textLines, int lineWidth, CharacterSet* font);
	static std::vector<TextWindow*>& GetTextWindows();
	static void AddTextWindow(TextWindow* textWindow);
	static void RemoveTextWindow(unsigned short index);
	
private:
	friend TextWindow* DawnInterface::createTextWindow();

	TextWindow();
	bool isMouseOnFrame(int mouseX, int mouseY) const;
	void updateFramesPosition();

	Enums::PositionType positionType;
	int x;
	int y;
	unsigned int autocloseTime;

	std::vector<std::string> textLines;
	std::string executeTextOnClose;
	bool explicitClose;

	static std::vector<TextWindow*> s_textWindows;
	static CharacterSet& Font;
	Clock m_timer;

	short m_posX = 0;
	short m_posY = 0;
	short m_width;
	short m_height;
};
