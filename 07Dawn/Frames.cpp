#include <memory>

#include "frames.h"
#include "TextureManager.h"

#include "Constants.h"

namespace Frames{
	std::auto_ptr<TextureManager> frameTextures(NULL);

	void initFrameTextures(){

		if (frameTextures.get() != NULL){
			return;
		}

		TextureAtlasCreator::get().init(Globals::textureAtlas, 576, 64);
		frameTextures = std::auto_ptr<TextureManager>(new TextureManager());
		frameTextures->loadimage("res/interface/tooltip/lower_left2.tga", 9);
		frameTextures->loadimage("res/interface/tooltip/lower_right2.tga", 1);
		frameTextures->loadimage("res/interface/tooltip/upper_left2.tga", 2);
		frameTextures->loadimage("res/interface/tooltip/upper_right2.tga", 3);
		frameTextures->loadimage("res/interface/tooltip/background2.tga", 4);
		frameTextures->loadimage("res/interface/tooltip/upper2.tga", 5);
		frameTextures->loadimage("res/interface/tooltip/lower2.tga", 6);
		frameTextures->loadimage("res/interface/tooltip/left2.tga", 7);
		frameTextures->loadimage("res/interface/tooltip/right2.tga", 8);

		TextureAtlasCreator::get().shutdown();
	}

	void drawFrame(int leftX, int bottomY, int numBlocksX, int numBlocksY, int blockWidth, int blockHeight){
				
		glBindTexture(GL_TEXTURE_2D_ARRAY, Globals::textureAtlas);
		Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("batch"));

		// draw the corners
		TextureManager::DrawTextureBatched(frameTextures->getTexture(9), leftX, bottomY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
		TextureManager::DrawTextureBatched(frameTextures->getTexture(1), leftX + blockWidth + (numBlocksX*blockWidth), bottomY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
		TextureManager::DrawTextureBatched(frameTextures->getTexture(2), leftX,  bottomY + blockHeight + (numBlocksY * blockHeight), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
		TextureManager::DrawTextureBatched(frameTextures->getTexture(3), leftX + blockWidth + (numBlocksX*blockWidth), bottomY + blockHeight + (numBlocksY * blockHeight), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);

		// draw the top and bottom borders
		for (int blockX = 0; blockX < numBlocksX; blockX++){
			TextureManager::DrawTextureBatched(frameTextures->getTexture(5), leftX + blockWidth + (blockX*blockWidth), bottomY + blockHeight + (numBlocksY*blockHeight), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
			TextureManager::DrawTextureBatched(frameTextures->getTexture(6), leftX + blockWidth + (blockX*blockWidth), bottomY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
		}

		// draw the right and left borders
		for (int blockY = 0; blockY < numBlocksY; blockY++){
			TextureManager::DrawTextureBatched(frameTextures->getTexture(7), leftX, bottomY + blockHeight + (blockY*blockHeight), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
			TextureManager::DrawTextureBatched(frameTextures->getTexture(8), leftX + blockWidth + (numBlocksX*blockWidth), bottomY + blockHeight + (blockY*blockHeight), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
		}

		// draw the background
		for (int blockY = 0; blockY < numBlocksY; blockY++){
			for (int blockX = 0; blockX < numBlocksX; blockX++){
				TextureManager::DrawTextureBatched(frameTextures->getTexture(4), leftX + blockWidth + (blockX*blockWidth), bottomY + blockHeight + (blockY*blockHeight), 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false);
			}
		}

		Batchrenderer::get().drawBuffer(false);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	int calculateNeededBlockWidth(int contentWidth, int singleBlockWidth){
		if (contentWidth % singleBlockWidth == 0){
			return (contentWidth / singleBlockWidth);

		}else{
			return (contentWidth / singleBlockWidth + 1);
		}
	}

	int calculateNeededBlockHeight(int contentHeight, int singleBlockHeight){
		if (contentHeight % singleBlockHeight == 0){
			return (contentHeight / singleBlockHeight);

		}else{
			return (contentHeight / singleBlockHeight + 1);
		}
	}
}