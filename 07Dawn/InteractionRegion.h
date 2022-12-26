#pragma once
#include <string>
#include <vector>

#include "Luainterface.h"

class InteractionRegion {

public:
	InteractionRegion();

	void setPosition(int left, int bottom, int width, int height);
	void getPosition(int &left, int &bottom, int &width, int &height) const;
	void setOnEnterText(std::string enterCode);
	std::string getOnEnterText() const;
	void setOnLeaveText(std::string leaveCode);
	std::string getOnLeaveText() const;
	void interactWithPlayer();

	std::string getLuaSaveText() const;

	bool isMarkedDeletable() const;
	void markAsDeletable();

	int getLeft();
	int getBottom();
	int getWidth();
	int getHeight();

	static std::vector<InteractionRegion*>& GetInteractionRegions();
	static void AddInteractionRegion(InteractionRegion* interactionRegion);

private:

	int left;
	int bottom;
	int width;
	int height;

	bool playerInside;

	std::string onEnterCode;
	std::string onLeaveCode;

	bool markedAsDeletable;
	Player& m_player;
};