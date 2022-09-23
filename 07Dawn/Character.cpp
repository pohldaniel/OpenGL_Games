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

void CCharacter::setMoveTexture(ActivityType::ActivityType activity, int direction, int index, std::string filename, int textureOffsetX, int textureOffsetY){
	size_t activityNr = static_cast<size_t>(activity);
	TileSet* tileSet = getTileSet(activity);

	tileSet->addTile(filename, TileClassificationType::TileClassificationType::FLOOR);

	
}
