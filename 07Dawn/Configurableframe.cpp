#include "Configurableframe.h"

#include "frames.h"

#include <iostream>
#include <algorithm>
static const size_t usedBlockWidth = 64;
static const size_t usedBlockHeight = 64;

ConfigurableFrame::ConfigurableFrame(int contentX, int contentY, int contentWidth, int contentHeight, DawnState::DawnState dawnState)
	: FramesBase(contentX - usedBlockWidth, contentY - usedBlockHeight,
		Frames::calculateNeededBlockWidth(contentWidth, usedBlockWidth) * usedBlockWidth,
		Frames::calculateNeededBlockHeight(contentHeight, usedBlockHeight) * usedBlockHeight, 0, 0),
	//blocksX(Frames::calculateNeededBlockWidth(contentWidth, usedBlockWidth)),
	//blocksY(Frames::calculateNeededBlockHeight(contentHeight, usedBlockHeight)),
	autoResize(false),
	centeringLayout(false),
	centerOnScreen(false)
{
	setPosition(contentX, contentY);
	setFramesDawnState(dawnState);

}

void ConfigurableFrame::draw(int mouseX, int mouseY){
	//glColor4f(1, 1, 1, 1);


	Frames::drawFrame(getPosX(), getPosY(), blocksX, blocksY, usedBlockWidth, usedBlockHeight);
	glTranslatef(0, 0, 0);
	FramesBase::draw(0, 0);
	glTranslatef(0, 0, 0);
}

void ConfigurableFrame::clicked(int mouseX, int mouseY, uint8_t mouseState)
{
	FramesBase::clicked(mouseX - usedBlockWidth, mouseY - usedBlockHeight, mouseState);
}

void ConfigurableFrame::setAutoresize()
{
	this->autoResize = true;
	applyLayout();
}

void ConfigurableFrame::setCenteringLayout()
{
	this->centeringLayout = true;
	applyLayout();
}

void ConfigurableFrame::setCenterOnScreen()
{
	this->centerOnScreen = true;
	recalculatePosition();
}

int ConfigurableFrame::getWidth() const
{
	return (2 + blocksX) * usedBlockWidth;
}

int ConfigurableFrame::getHeight() const
{
	return (2 + blocksY) * usedBlockHeight;
}

void ConfigurableFrame::addChildFrame(int relPosX, int relPosY, std::auto_ptr<FramesBase> newChild)
{
	FramesBase::addChildFrame(relPosX, relPosY, newChild);
	applyLayout();

	
}

void ConfigurableFrame::applyLayout()
{
	std::vector<FramesBase*> childFrames = getChildFrames();

	if (centeringLayout && autoResize)
	{
		int maxWidth = 0;
		int totalHeight = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild)
		{
			maxWidth = std::max(maxWidth, childFrames[curChild]->getWidth());
			totalHeight += childFrames[curChild]->getHeight();
		}
		totalHeight += (childFrames.size() + 1) * 2;
		resize(maxWidth, totalHeight);
	}
	else if (autoResize)
	{
		int maxX = 0;
		int maxY = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild)
		{
			maxX = std::max(maxX, childFrames[curChild]->getPosX() + childFrames[curChild]->getWidth());
			maxY = std::max(maxY, childFrames[curChild]->getPosY() + childFrames[curChild]->getHeight());
		}
		resize(maxX, maxY);
	}

	if (centeringLayout)
	{
		int totalChildHeight = 0;
		int maxChildWidth = 0;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild)
		{
			totalChildHeight += childFrames[curChild]->getHeight();
			maxChildWidth = std::max(maxChildWidth, childFrames[curChild]->getWidth());
		}
		int skipPerElement = ((blocksY*usedBlockHeight - totalChildHeight) / (childFrames.size() + 1));
		int curY = skipPerElement;
		for (size_t curChild = 0; curChild < childFrames.size(); ++curChild)
		{

			childFrames[curChild]->setPosition((blocksX*usedBlockWidth - maxChildWidth) / 2, curY);
			curY += skipPerElement + childFrames[curChild]->getHeight();
		}
	}
}

void ConfigurableFrame::resize(int width, int height){
	blocksX = Frames::calculateNeededBlockWidth(width, usedBlockWidth);
	blocksY = Frames::calculateNeededBlockHeight(height, usedBlockHeight);
	recalculatePosition();	
}

void ConfigurableFrame::recalculatePosition()
{
	if (centerOnScreen)
		setPosition((WIDTH - getWidth()) / 2, (HEIGHT - getHeight()) / 2);
}
