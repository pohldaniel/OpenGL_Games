#include "Modal.h"

static const float usedBlockWidth = 64.0f;
static const float usedBlockHeight = 64.0f;

Modal::Modal(int contentX, int contentY, int contentWidth, int contentHeight)
	: Widget(contentX - usedBlockWidth, contentY - usedBlockHeight, Frames::calculateNeededBlockWidth(contentWidth, usedBlockWidth) * usedBlockWidth, Frames::calculateNeededBlockHeight(contentHeight, usedBlockHeight) * usedBlockHeight),
	blocksX(Frames::calculateNeededBlockWidth(contentWidth, usedBlockWidth)),
	blocksY(Frames::calculateNeededBlockHeight(contentHeight, usedBlockHeight)),
	autoResize(false),
	centeringLayout(false),
	centerOnScreen(false)
{
	setPosition(contentX, contentY);



}

/*Modal::Modal(int posX, int posY, int width, int height) : Widget(posX, posY, width, height), autoResize(false), centeringLayout(false), centerOnScreen(false){
	setPosition(posX, posY);

}*/

void Modal::draw() {

	Frames::drawFrame(getPosX(), getPosY(), blocksX, blocksY, usedBlockWidth, usedBlockHeight);

	m_transform.translate(usedBlockWidth, usedBlockHeight, 0.0f);
	Widget::draw();
	m_transform.translate(-usedBlockWidth, -usedBlockHeight, 0.0f);
}

void Modal::setAutoresize(){
	this->autoResize = true;
	applyLayout();
}

void Modal::setCenteringLayout(){
	this->centeringLayout = true;
	applyLayout();
}

void Modal::setCenterOnScreen()
{
	this->centerOnScreen = true;
	recalculatePosition();
}

int Modal::getWidth() const{
	return (2 + blocksX) * usedBlockWidth;
}

int Modal::getHeight() const{
	return (2 + blocksY) * usedBlockHeight;
}

void Modal::addChildFrame(int relPosX, int relPosY, std::auto_ptr<Widget> newChild){
	Widget::addChildWidget(relPosX, relPosY, newChild);
	applyLayout();	
}

void Modal::applyLayout() {
	std::vector<Widget*> childFrames = getChildWidgets();

	if (centeringLayout && autoResize)
	{
		int maxWidth = 0;
		int totalHeight = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild){
			maxWidth = std::max(maxWidth, childFrames[curChild]->getWidth());
			totalHeight += childFrames[curChild]->getHeight();	
		}
		totalHeight += (childFrames.size() + 1) * 2;
		resize(maxWidth, totalHeight);

	}else if (autoResize){

		int maxX = 0;
		int maxY = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild)
		{
			maxX = std::max(maxX, childFrames[curChild]->getPosX() + childFrames[curChild]->getWidth());
			maxY = std::max(maxY, childFrames[curChild]->getPosY() + childFrames[curChild]->getHeight());
		}
		resize(maxX, maxY);
	}

	if (centeringLayout){
		int totalChildHeight = 0;
		int maxChildWidth = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild)
		{
			totalChildHeight += childFrames[curChild]->getHeight();
			maxChildWidth = std::max(maxChildWidth, childFrames[curChild]->getWidth());
		}
		int skipPerElement = ((blocksY*usedBlockHeight - totalChildHeight) / (childFrames.size() + 1));
		int curY = skipPerElement;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild){
			childFrames[curChild]->setPosition((blocksX*usedBlockWidth - maxChildWidth) / 2, curY);
			curY += skipPerElement + childFrames[curChild]->getHeight();
		}
	}

	
}

void Modal::resize(int width, int height){

	blocksX = Frames::calculateNeededBlockWidth(width, usedBlockWidth);
	blocksY = Frames::calculateNeededBlockHeight(height, usedBlockHeight);
	recalculatePosition();
}

void Modal::recalculatePosition(){
	if (centerOnScreen)
		setPosition((WIDTH - getWidth()) / 2, (HEIGHT - getHeight()) / 2);
}
