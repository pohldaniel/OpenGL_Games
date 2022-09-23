#include "Character.h"

CCharacter::CCharacter() {
	size_t numActivities = static_cast<size_t>(ActivityType::Count);
	numMoveTexturesPerDirection = new int[numActivities];
	texture = new TextureRect*[numActivities];

	for (size_t curActivity = 0; curActivity<numActivities; ++curActivity) {
		numMoveTexturesPerDirection[curActivity] = 0;
		texture[curActivity] = NULL;
	}
}


void CCharacter::setNumMoveTexturesPerDirection(ActivityType::ActivityType activity, int numTextures){
	size_t activityNr = static_cast<size_t>(activity);
	numMoveTexturesPerDirection[activityNr] = numTextures;


	texture[activityNr] = new TextureRect();
}

TileSet* CCharacter::getTileSet(ActivityType::ActivityType activity) {
	return &m_tileSets[activity];
}

void CCharacter::setTileSet(ActivityType::ActivityType activity, TileSet& value) {
	m_tileSets[activity] = value;
}

unsigned short CCharacter::getNumActivities() {
	return m_tileSets.size();
}

void CCharacter::setMoveTexture(ActivityType::ActivityType activity, int direction, int index, std::string filename, int textureOffsetX, int textureOffsetY){
	
	
	size_t activityNr = static_cast<size_t>(activity);
	TileSet* tileSet = getTileSet(activity);

	tileSet->addTile(filename, TileClassificationType::TileClassificationType::FLOOR);
}

ActivityType::ActivityType CCharacter::getCurActivity() const {
	ActivityType::ActivityType curActivity = ActivityType::Walking;
	
	return curActivity;
}

int CCharacter::getXPos() const {
	return x_pos;
}

int CCharacter::getYPos() const {
	return y_pos;
}

void CCharacter::setName(std::string newName){
	name = newName;
}

