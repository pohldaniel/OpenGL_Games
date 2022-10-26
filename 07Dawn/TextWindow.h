#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "engine/CharacterSet.h"

#include "Widget.h"
#include "Luainterface.h"

namespace PositionType {

	enum PositionType {
		BOTTOMLEFT,
		BOTTOMCENTER,
		LEFTCENTER,
		CENTER
	};
}

class TextWindow : public Widget {
public:
	void setText(std::string text);
	void setAutocloseTime(int autocloseTime);
	void setPosition(PositionType::PositionType, int x, int y);
	void center();
	void setOnCloseText(std::string onCloseText);
	bool canBeDeleted() const;
	void close();

	void clicked(int mouseX, int mouseY, uint8_t mouseState);
	void draw();

	static void FormatMultilineText(std::string textIn, std::vector< std::string > &textLines, int lineWidth, CharacterSet* font);

private:
	friend TextWindow* DawnInterface::createTextWindow();

	TextWindow();

	void updateFramesPosition();

	PositionType::PositionType positionType;
	int x;
	int y;
	uint32_t autocloseTime;
	uint32_t creationTime;
	std::vector<std::string> textLines;
	std::string executeTextOnClose;
	bool explicitClose;
};
