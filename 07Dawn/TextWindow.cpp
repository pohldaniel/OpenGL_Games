#include "TextWindow.h"
#include "Constants.h"


std::vector<TextWindow*> TextWindow::s_textWindows = std::vector<TextWindow*>();
CharacterSet& TextWindow::Font = Globals::fontManager.get("verdana_9");

char *strtok_r(char *str, const char *delim, char **nextp){
	char *ret;

	if (str == NULL){
		str = *nextp;
	}

	str += strspn(str, delim);

	if (*str == '\0'){
		return NULL;
	}

	ret = str;
	str += strcspn(str, delim);

	if (*str){
		*str++ = '\0';
	}

	*nextp = str;

	return ret;
}

void TextWindow::FormatMultilineText(std::string textIn, std::vector< std::string > &textLines, int lineWidth, CharacterSet* font) {
	// for strtok_r
	std::string text = std::string("").append(textIn);
	char *some_ptr = NULL;
	char **save_ptr = &some_ptr;
	std::string delim = " ";
	char *nextString = strtok_r(const_cast<char*>(text.c_str()), delim.c_str(), save_ptr);
	std::string curLine = "";
	std::string testCurLine = "";
	while (nextString != NULL) {
		if (testCurLine != "") {
			testCurLine.append(" ");
		}
		testCurLine.append(nextString);
		// check render length of curLine
		int estimatedWidth = font->getWidth(testCurLine);
		if (estimatedWidth <= lineWidth) {
			curLine = testCurLine;
		}
		else {
			textLines.push_back(curLine);
			curLine = std::string("").append(nextString);
			testCurLine = curLine;
		}
		nextString = strtok_r(NULL, delim.c_str(), save_ptr);
	}

	if (curLine != "") {
		textLines.push_back(curLine);
	}
}

std::vector<TextWindow*> TextWindow::GetTextWindows() {
	return s_textWindows;
}

void TextWindow::AddTextWindow(TextWindow* textWindow) {
	s_textWindows.push_back(textWindow);
}

void TextWindow::RemoveTextWindow(unsigned short index) {
	s_textWindows.erase(s_textWindows.begin() + index);
}

TextWindow::TextWindow() : Widget(0, 0, 0, 0),
	positionType(PositionType::CENTER),
	x(0),
	y(0),
	autocloseTime(1),
	executeTextOnClose(""),
	explicitClose(false) {
	updateFramesPosition();
}

void TextWindow::setText(std::string text) {
	//initTextWindowFont();

	textLines.clear();

	// format the text.
	const int lineWidth = 416;

	FormatMultilineText(text, textLines, lineWidth, &Font);

	updateFramesPosition();
}

void TextWindow::setAutocloseTime(unsigned int autocloseTime) {
	this->autocloseTime = autocloseTime;
	m_timer.restart();
}

void TextWindow::setPosition(PositionType::PositionType positionType, int x, int y) {
	this->positionType = positionType;
	this->x = x;
	this->y = y;

	updateFramesPosition();
}

void TextWindow::center() {
	this->positionType = PositionType::CENTER;
	this->x = ViewPort::get().getWidth() / 2;
	this->y = 280;

	updateFramesPosition();
}

void TextWindow::setOnCloseText(std::string onCloseText) {
	this->executeTextOnClose = onCloseText;
}

void TextWindow::updateFramesPosition() {
	const int blockSizeX = 32;
	const int blockSizeY = 32;
	const int lineWidth = 416;

	int neededWidth = lineWidth;
	int neededHeight = 0;
	if (textLines.size() > 0) {
		const int lineSpace = Font.lineHeight  / 2;
		neededHeight = Font.lineHeight * static_cast<int>(textLines.size())
			+ lineSpace * (static_cast<int>(textLines.size()) - 1);
	}
	int neededInnerBlocksX = neededWidth / blockSizeX;
	if (neededWidth % blockSizeX != 0) {
		++neededInnerBlocksX;
	}
	int neededInnerBlocksY = neededHeight / blockSizeY;
	if (neededHeight % blockSizeY != 0) {
		++neededInnerBlocksY;
	}

	int leftX = 0;
	int bottomY = 0;

	switch (positionType){
	case PositionType::CENTER:
		leftX = x - (neededInnerBlocksX * blockSizeX / 2);
		bottomY = y - (neededInnerBlocksY * blockSizeY / 2);
		break;
	case PositionType::BOTTOMLEFT:
		leftX = x;
		bottomY = y;
		break;
	case PositionType::LEFTCENTER:
		leftX = x;
		bottomY = y - (neededInnerBlocksY * blockSizeY / 2);
		break;
	case PositionType::BOTTOMCENTER:
		leftX = x + (neededInnerBlocksX * blockSizeX / 2);
		bottomY = y;
		break;
	}

	Widget::m_posX = leftX;
	Widget::m_posY = bottomY;
	Widget::m_width = (neededInnerBlocksX + 2) * blockSizeX;
	Widget::m_height = (neededInnerBlocksY + 2) * blockSizeY;
}

bool TextWindow::canBeDeleted() const {

	return (explicitClose || (autocloseTime > 0 && m_timer.getElapsedTimeSinceRestartMilli() > autocloseTime));
}

void TextWindow::close() {
	if (executeTextOnClose != "") {
		LuaFunctions::executeLuaScript(executeTextOnClose);
	}
}

void TextWindow::clicked(int mouseX, int mouseY, uint8_t mouseState) {
	/*if ((mouseState == SDL_BUTTON_LEFT) && isMouseOnFrame(mouseX, mouseY)) {
		explicitClose = true;
	}*/
}

void TextWindow::draw() {
	if (explicitClose || (autocloseTime > 0 && m_timer.getElapsedTimeSinceRestartMilli() > autocloseTime)) {
		return;
	}
	const int blockSizeX = 32;
	const int blockSizeY = 32;
	const int lineWidth = 416;
	const int lineSpace = Font.lineHeight * 0.5;

	int neededWidth = lineWidth;
	int neededHeight = 0;
	if (textLines.size() > 0) {
		neededHeight = Font.lineHeight * textLines.size()
			+ lineSpace * (textLines.size() - 1);
	}
	int neededInnerBlocksX = neededWidth / blockSizeX;
	if (neededWidth % blockSizeX != 0) {
		++neededInnerBlocksX;
	}
	int neededInnerBlocksY = neededHeight / blockSizeY;
	if (neededHeight % blockSizeY != 0) {
		++neededInnerBlocksY;
	}

	int leftX = 0;
	int bottomY = 0;

	switch (positionType) {
	case PositionType::CENTER:
		leftX = x - (neededInnerBlocksX * blockSizeX / 2);
		bottomY = y - (neededInnerBlocksY * blockSizeY / 2);
		break;
	case PositionType::BOTTOMLEFT:
		leftX = x;
		bottomY = y;
		break;
	case PositionType::LEFTCENTER:
		leftX = x;
		bottomY = y - (neededInnerBlocksY * blockSizeY / 2);
		break;
	case PositionType::BOTTOMCENTER:
		leftX = x + (neededInnerBlocksX * blockSizeX / 2);
		bottomY = y;
		break;
	}
	
	leftX += ViewPort::get().getLeft();
	bottomY += ViewPort::get().getBottom();

	// draw the frame
	DialogCanvas::drawCanvas(leftX, bottomY, neededInnerBlocksX, neededInnerBlocksY, blockSizeX, blockSizeY, true);
	// draw the text
	int curX = leftX + blockSizeX;
	int curY = bottomY + neededInnerBlocksY * blockSizeY + Font.lineHeight;
	for (size_t curLineNr = 0; curLineNr < textLines.size(); ++curLineNr) {
		std::string curLine = textLines[curLineNr];
		Fontrenderer::Get().drawText(Font, curX, curY, curLine, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
		curY -= Font.lineHeight;
		curY -= lineSpace;
	}
}
