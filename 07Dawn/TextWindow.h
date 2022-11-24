#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "engine/Fontrenderer.h"
#include "engine/Clock.h"

#include "Widget.h"
#include "Dialog.h"
#include "Luainterface.h"

namespace PositionType {

	enum PositionType {
		BOTTOMLEFT,
		BOTTOMCENTER,
		LEFTCENTER,
		CENTER
	};
}

class TextWindow {
public:
	void setText(std::string text);
	void setAutocloseTime(unsigned int autocloseTime);
	void setPosition(PositionType::PositionType, int x, int y);
	void center();
	void setOnCloseText(std::string onCloseText);
	bool canBeDeleted() const;
	void close();

	void clicked(int mouseX, int mouseY, uint8_t mouseState);
	void draw();

	static void FormatMultilineText(std::string textIn, std::vector< std::string > &textLines, int lineWidth, CharacterSet* font);
	static std::vector<TextWindow*> GetTextWindows();
	static void AddTextWindow(TextWindow* textWindow);
	static void RemoveTextWindow(unsigned short index);

private:
	friend TextWindow* DawnInterface::createTextWindow();

	TextWindow();

	void updateFramesPosition();

	PositionType::PositionType positionType;
	int x;
	int y;
	unsigned int autocloseTime;

	std::vector<std::string> textLines;
	std::string executeTextOnClose;
	bool explicitClose;

	static std::vector<TextWindow*> s_textWindows;
	static CharacterSet& Font;
	Clock m_timer;
};
