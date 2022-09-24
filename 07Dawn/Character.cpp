#include "Character.h"
#include "TilesetManager.h"

Character::Character() {
	size_t numActivities = static_cast<size_t>(ActivityType::Count);
	numMoveTexturesPerDirection = new int[numActivities];
}


void Character::setNumMoveTexturesPerDirection(ActivityType::ActivityType activity, int numTextures){
	size_t activityNr = static_cast<size_t>(activity);
	numMoveTexturesPerDirection[activityNr] = numTextures;
}

TileSet* Character::getTileSet(ActivityType::ActivityType activity, Direction direction) {
	return &m_moveTileSets[{activity, direction}];
}

void Character::setTileSet(std::map<std::pair<int, int>, TileSet>& moveTileSets) {
	m_moveTileSets= moveTileSets;
}

unsigned short Character::getNumActivities() {
	return m_moveTileSets.size() / 8;
}

void Character::setMoveTexture(ActivityType::ActivityType activity, Direction direction, int index, std::string filename, int textureOffsetX, int textureOffsetY){
	
	
	
	TileSet* tileSet = getTileSet(activity, direction);

	tileSet->addTile(filename, TileClassificationType::TileClassificationType::FLOOR);
}

ActivityType::ActivityType Character::getCurActivity() const {
	ActivityType::ActivityType curActivity = ActivityType::Walking;
	
	return curActivity;
}

int Character::getXPos() const {
	return x_pos;
}

int Character::getYPos() const {
	return y_pos;
}

void Character::setName(std::string newName){
	name = newName;
}

