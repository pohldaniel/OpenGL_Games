#include "Framesbase.h"

#include "Constants.h"

FramesBase::FramesBase()
	: visible(false),
	moveableFrame(false),
	closeButton(false),
	movingFrame(false),
	framesDawnState(DawnState::NoState),
	name(""){}

FramesBase::FramesBase(int16_t posX_, int16_t posY_, uint16_t frameWidth_, uint16_t frameHeight_, int16_t frameOffsetX_, int16_t frameOffsetY_)
	: posX(posX_),
	posY(posY_),
	frameWidth(frameWidth_),
	frameHeight(frameHeight_),
	frameOffsetX(frameOffsetX_),
	frameOffsetY(frameOffsetY_),
	visible(false),
	moveableFrame(false),
	closeButton(false),
	movingFrame(false),
	framesDawnState(DawnState::NoState),
	name(""){

	if (posY + frameHeight + frameOffsetY > HEIGHT){
		posY = HEIGHT - frameHeight - frameOffsetY;
	}
}

FramesBase::FramesBase(int16_t posX_, int16_t posY_, uint16_t frameWidth_, uint16_t frameHeight_, int16_t frameOffsetX_, int16_t frameOffsetY_, char* name_)
	: posX(posX_),
	posY(posY_),
	frameWidth(frameWidth_),
	frameHeight(frameHeight_),
	frameOffsetX(frameOffsetX_),
	frameOffsetY(frameOffsetY_),
	visible(false),
	moveableFrame(false),
	closeButton(false),
	movingFrame(false),
	framesDawnState(DawnState::NoState),
	name(name_){

	if (posY + frameHeight + frameOffsetY > HEIGHT){
		posY = HEIGHT - frameHeight - frameOffsetY;
	}
}

FramesBase::~FramesBase(){

	for (size_t curChildNr = 0; curChildNr<childFrames.size(); ++curChildNr){
		delete childFrames[curChildNr];
	}

	childFrames.resize(0);
}

void FramesBase::addMoveableFrame(uint16_t titleWidth, uint16_t titleHeight, int16_t titleOffsetX, int16_t titleOffsetY){

	moveableFrame = true;
	this->titleWidth = titleWidth;
	this->titleHeight = titleHeight;
	this->titleOffsetX = titleOffsetX;
	this->titleOffsetY = titleOffsetY;
}

void FramesBase::addCloseButton(uint16_t buttonWidth, uint16_t buttonHeight, int16_t buttonOffsetX, int16_t buttonOffsetY){

	closeButton = true;
	this->buttonWidth = buttonWidth;
	this->buttonHeight = buttonHeight;
	this->buttonOffsetX = buttonOffsetX;
	this->buttonOffsetY = buttonOffsetY;
}

void FramesBase::moveFrame(uint16_t mouseX, uint16_t mouseY){

	// start moving the frame if we're not.
	// if we are already moving the frame then update the coordinates.
	if (movingFrame == false){
		movingFrame = true;
		startMovingFrameXpos = mouseX;
		startMovingFrameYpos = mouseY;

	}else{
		posX += mouseX - startMovingFrameXpos;
		posY += mouseY - startMovingFrameYpos;

		startMovingFrameXpos = mouseX;
		startMovingFrameYpos = mouseY;
	}
}

void FramesBase::stopMovingFrame(uint16_t mouseX, uint16_t mouseY){
	movingFrame = false;
	startMovingFrameXpos = 0;
	startMovingFrameYpos = 0;
}

bool FramesBase::isMovingFrame() const{
	return movingFrame;
}

void FramesBase::draw(int mouseX, int mouseY){
	if (childFrames.size() > 0){
		//glTranslatef(posX, posY, 0.0f);

		for (size_t curChildNr = 0; curChildNr<childFrames.size(); ++curChildNr)
		{
			childFrames[curChildNr]->draw(mouseX - posX, mouseY - posY);
		}

		//glTranslatef(-posX, -posY, 0.0f);
	}
}

void FramesBase::clicked(int mouseX, int mouseY, uint8_t mouseState){
	//bool clickHandled = false;
	for (size_t curChildNr = 0; curChildNr<childFrames.size(); ++curChildNr){
		// clickHandled |= childControls[ curChildNr ]->click( mouseX-relPosX, mouseY-relPosY, mouseState );
		childFrames[curChildNr]->clicked(mouseX - posX, mouseY - posY, mouseState);
	}
	//return clickHandled;
}

bool FramesBase::isMouseOnTitlebar(int mouseX, int mouseY) const{
	// if the frame has no titlebar we return false right away...
	if (moveableFrame == false){
		return false;
	}

	if (mouseX < posX + titleOffsetX
		|| mouseY < posY + titleOffsetY
		|| mouseX > posX + titleOffsetX + titleWidth
		|| mouseY > posY + titleOffsetY + titleHeight) {
		return false;
	}
	return true;
}

bool FramesBase::isMouseOnCloseButton(int mouseX, int mouseY) const{
	// if the frame has no closebutton we return false right away...
	if (closeButton == false){
		return false;
	}

	if (mouseX < posX + buttonOffsetX
		|| mouseY < posY + buttonOffsetY
		|| mouseX > posX + buttonOffsetX + buttonWidth
		|| mouseY > posY + buttonOffsetY + buttonHeight) {
		return false;
	}
	return true;
}

bool FramesBase::isMouseOnFrame(int mouseX, int mouseY) const
{
	if (!isVisible())
	{
		return false;
	}

	if (mouseX < posX + frameOffsetX
		|| mouseY < posY + frameOffsetY
		|| mouseX > posX + frameOffsetX + getWidth()
		|| mouseY > posY + frameOffsetY + getHeight()) {
		return false;
	}

	return true;
}

bool FramesBase::isVisible() const{
	return visible;
}

void FramesBase::setVisible(bool visible){
	// Activate the frames dawnstate, only if we have set the frame visible and it's state is
	if (visible == true && framesDawnState != DawnState::NoState){
		DawnState::setDawnState(framesDawnState);
	}
	this->visible = visible;
}

void FramesBase::toggle(){
	// is the frame visible?
	if (isVisible()){
		// then loop through all frames and remove the frame and make it unvisible.
		for (size_t curFrame = 0; curFrame < Globals::activeFrames.size(); curFrame++){

			if (this == Globals::activeFrames[curFrame]){

				setVisible(false);
				Globals::activeFrames.erase(Globals::activeFrames.begin() + curFrame);
				return;
			}
		}
	}else{
		// else add it to the frame vector and make it visible.
		Globals::activeFrames.push_back(this);
		setVisible(true);
	}
}

void FramesBase::setOnTop(){
	// loop through all frames and put that on top.
	for (size_t curFrame = 0; curFrame < Globals::activeFrames.size(); curFrame++){

		if (this == Globals::activeFrames[curFrame]){

			Globals::activeFrames.erase(Globals::activeFrames.begin() + curFrame);
			Globals::activeFrames.push_back(this);
			return;
		}
	}
}

void FramesBase::setPosition(int parentOffsetX, int parentOffsetY){

	posX = parentOffsetX;
	posY = parentOffsetY;
}

void FramesBase::addToParent(int posOffsetX, int posOffsetY, FramesBase *parent){

	this->parentFrame = parent;
	setPosition(posOffsetX, posOffsetY);
}

void FramesBase::setFramesDawnState(DawnState::DawnState framesDawnState){
	this->framesDawnState = framesDawnState;
}

void FramesBase::addChildFrame(int posOffsetX, int posOffsetY, std::auto_ptr<FramesBase> newChild){
	newChild->addToParent(posOffsetX, posOffsetY, this);
	childFrames.push_back(newChild.release());
}

int FramesBase::getPosX() const{
	return posX;
}

int FramesBase::getPosY() const{
	return posY;
}

int FramesBase::getWidth() const{
	return frameWidth;
}

int FramesBase::getHeight() const{
	return frameHeight;
}

char* FramesBase::getName() const{
	return name;
}

std::vector<FramesBase*> FramesBase::getChildFrames(){
	return childFrames;
}