#include "InteractionRegion.h"
#include "Luafunctions.h"
#include "Zone.h"

InteractionRegion::InteractionRegion()
	: left(0),
	bottom(0),
	width(0),
	height(0),
	playerInside(false),
	markedAsDeletable(false), m_player(Player::Get()) {
}

void InteractionRegion::setPosition(int left_, int bottom_, int width_, int height_) {
	left = left_;
	bottom = bottom_;
	width = width_;
	height = height_;
}

void InteractionRegion::getPosition(int &left_, int &bottom_, int &width_, int &height_) const {
	left_ = left;
	bottom_ = bottom;
	width_ = width;
	height_ = height;
}

void InteractionRegion::setOnEnterText(std::string enterCode) {	
	this->onEnterCode = enterCode;
}

std::string InteractionRegion::getOnEnterText() const {
	return onEnterCode;
}

void InteractionRegion::setOnLeaveText(std::string leaveCode) {
	this->onLeaveCode = leaveCode;
}

std::string InteractionRegion::getOnLeaveText() const {
	return onLeaveCode;
}

void InteractionRegion::interactWithPlayer() {
	int px = m_player.getXPos();
	int pw = m_player.getWidth();
	int py = m_player.getYPos();
	int ph = m_player.getWidth();
	bool playerNowInside = (px >= left && px + pw <= left + width && py >= bottom && py + ph <= bottom + height);
	if (playerNowInside && !playerInside) {
		playerInside = true;
		if (onEnterCode != "") {
			LuaFunctions::executeLuaScript(onEnterCode);
		}
	}else if (!playerNowInside && playerInside) {
		playerInside = false;
		if (onLeaveCode != "") {
			LuaFunctions::executeLuaScript(onLeaveCode);
		}
	}
}

std::string InteractionRegion::getLuaSaveText() const {
	std::ostringstream oss;
	oss << "local curInteractionRegion = DawnInterface.addInteractionRegion();" << std::endl;
	oss << "curInteractionRegion:setPosition( " << left << ", " << bottom << ", " << width << ", " << height << " );" << std::endl;
	if (onEnterCode != "") {
		oss << "curInteractionRegion:setOnEnterText( [[" << onEnterCode << "]] );" << std::endl;
	}
	if (onLeaveCode != "") {
		oss << "curInteractionRegion:setOnLeaveText( [[" << onLeaveCode << "]] );" << std::endl;
	}

	return oss.str();
}

bool InteractionRegion::isMarkedDeletable() const {
	return markedAsDeletable;
}

void InteractionRegion::markAsDeletable() {
	markedAsDeletable = true;
}

void InteractionRegion::AddInteractionRegion(InteractionRegion* interactionRegion) {
	ZoneManager::Get().getCurrentZone()->addInteractionRegion(interactionRegion);
}

std::vector<InteractionRegion*>& InteractionRegion::GetInteractionRegions() {
	return ZoneManager::Get().getCurrentZone()->getInteractionRegions();
}