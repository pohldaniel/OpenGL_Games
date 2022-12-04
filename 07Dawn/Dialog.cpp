#include "Dialog.h"

unsigned int  DialogCanvas::textureAtlas;
std::vector<TextureRect>  DialogCanvas::m_textureBases;

void DialogCanvas::initTextures() {

	if (m_textureBases.size() > 0) {
		return;
	}

	TextureAtlasCreator::get().init("dialog", 576, 64);

	TextureManager::Loadimage("res/interface/tooltip/lower_left2.tga", 0, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/lower_right2.tga", 1, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/upper_left2.tga", 2, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/upper_right2.tga", 3, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/background2.tga", 4, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/upper2.tga", 5, m_textureBases);

	TextureManager::Loadimage("res/interface/tooltip/lower2.tga", 6, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/left2.tga", 7, m_textureBases);
	TextureManager::Loadimage("res/interface/tooltip/right2.tga", 8, m_textureBases);

	textureAtlas = TextureAtlasCreator::get().getAtlas();
}

void DialogCanvas::drawCanvas(int left, int bottom, int colummns, int rows, int tileWidth, int tileHeight, bool updateView, bool drawInChunk) {
	float _tileWidth = static_cast<float>(tileWidth);
	float _tileHeight = static_cast<float>(tileHeight);

	TextureManager::BindTexture(textureAtlas, true, 0);

	// draw the corners
	TextureManager::DrawTextureBatched(m_textureBases[0], left, bottom, _tileWidth, _tileHeight,  false, updateView);
	TextureManager::DrawTextureBatched(m_textureBases[1], left + tileWidth + (colummns * tileWidth), bottom, _tileWidth, _tileHeight, false, updateView);
	TextureManager::DrawTextureBatched(m_textureBases[2], left, bottom + tileHeight + (rows * tileHeight), _tileWidth, _tileHeight, false, updateView);
	TextureManager::DrawTextureBatched(m_textureBases[3], left + tileWidth + (colummns * tileWidth), bottom + tileHeight + (rows * tileHeight), _tileWidth, _tileHeight, false, updateView);

	// draw the top and bottom borders
	for (unsigned short column = 0; column < colummns; column++) {
		TextureManager::DrawTextureBatched(m_textureBases[5], left + tileWidth + (column * tileWidth), bottom + tileHeight + (rows * tileHeight), _tileWidth, _tileHeight, false, updateView);
		TextureManager::DrawTextureBatched(m_textureBases[6], left + tileWidth + (column * tileWidth), bottom, _tileWidth, _tileHeight, false, updateView);
	}

	// draw the right and left borders
	for (unsigned short row = 0; row < rows; row++) {
		TextureManager::DrawTextureBatched(m_textureBases[7], left, bottom + tileHeight + (row * tileHeight), _tileWidth, _tileHeight, false, updateView);
		TextureManager::DrawTextureBatched(m_textureBases[8], left + tileWidth + (colummns * tileWidth), bottom + tileHeight + (row * tileHeight), _tileWidth, _tileHeight, false, updateView);
	}

	// draw the background
	for (unsigned short row = 0; row < rows; row++) {
		for (int column = 0; column < colummns; column++) {
			TextureManager::DrawTextureBatched(m_textureBases[4], left + tileWidth + (column * tileWidth), bottom + tileHeight + (row * tileHeight), _tileWidth, _tileHeight, false, updateView);
		}
	}

	if (!drawInChunk) {
		TextureManager::DrawBuffer();
		TextureManager::UnbindTexture(true, 0);
	}
}

int DialogCanvas::calculateNeededBlockWidth(int width, int tileWidth) {
	if (width % tileWidth == 0) {
		return (width / tileWidth);

	}else {
		return (width / tileWidth + 1);
	}
}

int DialogCanvas::calculateNeededBlockHeight(int height, int tileHeight) {
	if (height % tileHeight == 0) {
		return (height / tileHeight);

	}else {
		return (height / tileHeight + 1);
	}
}

/////////////////////////////////////////////////////////////DIALOG///////////////////////////////////////////////////////////////////
Dialog::Dialog(int posX, int posY, int width, int height)
	: Widget(posX - TILE_WIDTH, posY - TILE_HEIGHT, DialogCanvas::calculateNeededBlockWidth(width, TILE_WIDTH) * TILE_WIDTH, DialogCanvas::calculateNeededBlockHeight(height, TILE_HEIGHT) * TILE_HEIGHT, 0, 0),
	m_columns(DialogCanvas::calculateNeededBlockWidth(width, TILE_WIDTH)),
	m_rows(DialogCanvas::calculateNeededBlockHeight(height, TILE_HEIGHT)),
	autoResize(false),
	centeringLayout(false),
	centerOnScreen(false){
	setPosition(posX, posY);

	DialogCanvas::initTextures();
}

void Dialog::draw() {

	DialogCanvas::drawCanvas(getPosX(), getPosY(), m_columns, m_rows, TILE_WIDTH, TILE_HEIGHT);
	Widget::draw();
}

void Dialog::processInput() {
	Widget::processInput();
}

void Dialog::setAutoresize(){
	autoResize = true;
	applyLayout();
}

void Dialog::setCenteringLayout(){
	centeringLayout = true;
	applyLayout();
}

void Dialog::setCenterOnScreen(){
	centerOnScreen = true;
	recalculatePosition();
}

int Dialog::getWidth() const{
	return (2 + m_columns) * TILE_WIDTH;
}

int Dialog::getHeight() const{
	return (2 + m_rows) * TILE_HEIGHT;
}

void Dialog::addChildWidget(int relPosX, int relPosY, std::auto_ptr<Widget> newChild){
	Widget::addChildWidget(relPosX, relPosY, newChild);
	applyLayout();	
}

void Dialog::applyLayout() {
	std::vector<Widget*> childFrames = getChildWidgets();

	if (centeringLayout && autoResize){
		int maxWidth = 0;
		int totalHeight = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild){
			maxWidth = std::max(maxWidth, childFrames[curChild]->getWidth());
			totalHeight += childFrames[curChild]->getHeight();	
		}
		totalHeight += (static_cast<int>(childFrames.size()) + 1) * 2;
		resize(maxWidth, totalHeight);

	}else if (autoResize){
		int maxX = 0;
		int maxY = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild){
			maxX = std::max(maxX, childFrames[curChild]->getPosX()  + childFrames[curChild]->getWidth());
			maxY = std::max(maxY, childFrames[curChild]->getPosY()  + childFrames[curChild]->getHeight());
		}
		resize(maxX, maxY);
	}

	if (centeringLayout){
		int totalChildHeight = 0;
		int maxChildWidth = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild){
			totalChildHeight += childFrames[curChild]->getHeight();
			maxChildWidth = std::max(maxChildWidth, childFrames[curChild]->getWidth());
		}
		int skipPerElement = ((m_rows * TILE_HEIGHT - totalChildHeight) / (static_cast<int>(childFrames.size()) + 1));
		int curY = skipPerElement;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild){
			childFrames[curChild]->setPosition((m_columns * TILE_WIDTH - maxChildWidth) / 2 + TILE_WIDTH, curY + TILE_HEIGHT);
			curY += skipPerElement + childFrames[curChild]->getHeight();
		}
	}	
}

void Dialog::resize(int width, int height){

	m_columns = DialogCanvas::calculateNeededBlockWidth(width, TILE_WIDTH);
	m_rows = DialogCanvas::calculateNeededBlockHeight(height, TILE_HEIGHT);
	recalculatePosition();
}

void Dialog::recalculatePosition(){
	if (centerOnScreen) {
		setPosition((ViewPort::get().getWidth() - getWidth()) / 2, (ViewPort::get().getHeight() - getHeight()) / 2);

		/*std::vector<Widget*> childFrames = getChildWidgets();
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild) {
			childFrames[curChild]->setParentPosition(m_posX + TILE_WIDTH, m_posY + TILE_HEIGHT);
		}*/
	}
}
