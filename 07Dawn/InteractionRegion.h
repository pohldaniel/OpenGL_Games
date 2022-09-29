#pragma once
#include <string>

#include "Luainterface.h"

class InteractionRegion{
public:
	void setPosition(int left, int bottom, int width, int height);
	void getPosition(int &left, int &bottom, int &width, int &height) const;
	void setOnEnterText(std::string enterCode);
	std::string getOnEnterText() const;
	void setOnLeaveText(std::string leaveCode);
	std::string getOnLeaveText() const;
	//void interactWithPlayer(Player *player);

	std::string getLuaSaveText() const;

	bool isMarkedDeletable() const;

private:
	friend InteractionRegion* DawnInterface::addInteractionRegion();
	friend void DawnInterface::removeInteractionRegion(InteractionRegion *regionToRemove);

	InteractionRegion();

	void markAsDeletable();

	int left;
	int bottom;
	int width;
	int height;

	bool playerInside;

	std::string onEnterCode;
	std::string onLeaveCode;

	bool markedAsDeletable;
};