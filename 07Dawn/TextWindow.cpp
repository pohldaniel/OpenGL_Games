#include "TextWindow.h"
#include "Interface.h"
#include "Constants.h"


std::vector<TextWindow*> TextWindow::s_textWindows = std::vector<TextWindow*>();
CharacterSet& TextWindow::Font = Globals::fontManager.get("verdana_14");

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

std::vector<TextWindow*>& TextWindow::GetTextWindows() {
	return s_textWindows;
}

TextWindow::TextWindow() : Widget(0, 0, 0, 0, 0, 0),
	positionType(Enums::PositionType::CENTER),
	x(0),
	y(0),
	autocloseTime(1),
	executeTextOnClose(""),
	explicitClose(false) {
	updateFramesPosition();
}

void TextWindow::setText(std::string text) {
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

void TextWindow::setPosition(Enums::PositionType positionType, int x, int y) {
	this->positionType = positionType;
	this->x = x;
	this->y = y;

	updateFramesPosition();
}

void TextWindow::center() {
	this->positionType = Enums::PositionType::CENTER;
	this->x = ViewPort::Get().getWidth() / 2;
	this->y = 280;

	updateFramesPosition();
}

void TextWindow::setOnCloseText(std::string onCloseText) {
	this->executeTextOnClose = onCloseText;
}

void TextWindow::updateFramesPosition() {

	int neededWidth = lineWidth;
	int neededHeight = 0;
	if (textLines.size() > 0) {
		const int lineSpace = Font.lineHeight  / 2;
		neededHeight = Font.lineHeight * static_cast<int>(textLines.size())
			+ lineSpace * (static_cast<int>(textLines.size()) - 1);
	}

	neededInnerBlocksX = neededWidth / blockSizeX;
	if (neededWidth % blockSizeX != 0) {
		++neededInnerBlocksX;
	}

	neededInnerBlocksY = neededHeight / blockSizeY;
	if (neededHeight % blockSizeY != 0) {
		++neededInnerBlocksY;
	}

	int leftX = 0;
	int bottomY = 0;

	switch (positionType){
		case Enums::PositionType::CENTER:
			leftX = x - (neededInnerBlocksX * blockSizeX / 2);
			bottomY = y - (neededInnerBlocksY * blockSizeY / 2);
			break;
		case Enums::PositionType::BOTTOMLEFT:
			leftX = x;
			bottomY = y;
			break;
		case Enums::PositionType::LEFTCENTER:
			leftX = x;
			bottomY = y - (neededInnerBlocksY * blockSizeY / 2);
			break;
		case Enums::PositionType::BOTTOMCENTER:
			leftX = x + (neededInnerBlocksX * blockSizeX / 2);
			bottomY = y;
			break;
	}

	m_posX = leftX;
	m_posY = bottomY;
	m_width = (neededInnerBlocksX + 2) * blockSizeX;
	m_height = (neededInnerBlocksY + 2) * blockSizeY;
}

bool TextWindow::canBeDeleted() const {
	return (explicitClose || (autocloseTime > 0 && m_timer.getElapsedTimeSinceRestartMilli() > autocloseTime));
}

void TextWindow::close() {
	if (executeTextOnClose != "") {
		LuaFunctions::executeLuaScript(executeTextOnClose);
	}
	
	Widget::close();
}

bool TextWindow::isMouseOnFrame(int mouseX, int mouseY) const {

	if (!m_visible) {
		return false;
	}
	
	if (mouseX < m_posX + 8
		|| mouseY > m_posY + m_height - 8
		|| mouseX >  m_posX + m_width - 10
		|| mouseY < m_posY + 11) {
		return false;
	}
	return true;
}

void TextWindow::processInput() {
	if (!m_visible) return;
	Widget::processInput();

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT) && isMouseOnFrame(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY())) {
		explicitClose = true;
	}
}

void TextWindow::draw() {
	if (!m_visible) return;

	// draw the frame
	DialogCanvas::DrawCanvas(m_posX, m_posY, neededInnerBlocksX, neededInnerBlocksY, blockSizeX, blockSizeY, false, true);
	// draw the text
	const int lineSpace = Font.lineHeight * 0.5;
	int curX = m_posX + blockSizeX;
	int curY = m_posY + neededInnerBlocksY * blockSizeY + Font.lineHeight;
	for (size_t curLineNr = 0; curLineNr < textLines.size(); ++curLineNr) {
		std::string curLine = textLines[curLineNr];
		Fontrenderer::Get().addText(Font, curX, curY, curLine, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		curY -= Font.lineHeight;
		curY -= lineSpace;
	}
}

void TextWindow::Update() {
	for (auto it = s_textWindows.begin(); it != s_textWindows.end();) {
		short index = static_cast<short>(std::distance(s_textWindows.begin(), it));
		TextWindow *curTextWindow = *it;

		if (curTextWindow->canBeDeleted() == true) {
			curTextWindow->close();
			delete curTextWindow;
			it = s_textWindows.erase(s_textWindows.begin() + index);
		}else {
			++it;
		}
	}
}

void TextWindow::AddTextWindow(TextWindow* textWindow) {
	s_textWindows.push_back(textWindow);
	Interface::Get().connectWidget(*textWindow, true);
}

void TextWindow::RemoveTextWindow(unsigned short index) {
	s_textWindows.erase(s_textWindows.begin() + index);
}

void TextWindow::Resize(int deltaW, int deltaH) {
	for (size_t curWindow = 0; curWindow < s_textWindows.size(); curWindow++) {
		s_textWindows[curWindow]->resize(deltaW, deltaH);
	}
}
