#pragma once

namespace Frames{

	void initFrameTextures();
	void drawFrame(int topX, int topY, int numBlocksX, int numBlocksY, int blockWidth, int blockHeight);
	int calculateNeededBlockWidth(int contentWidth, int singleBlockWidth);
	int calculateNeededBlockHeight(int contentHeight, int singleBlockHeight);
}