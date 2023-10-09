#include "Animation.h"

void Animation::loadAnimation16(std::string name, const std::vector<TextureRect>& textureRects) {
	std::ifstream readAnimations(name);

	char resourceFilename[128];

	Enums::Direction16 direction = Enums::Direction16::E;
	while (!readAnimations.eof()) {
		readAnimations >> resourceFilename;

		std::ifstream readAnimationDef(resourceFilename);
		int numAnimationFrames = 0;
		int framesPerSecond = 0;
		int loopInt = 0;

		readAnimationDef >> numAnimationFrames >> framesPerSecond >> loopInt;

		m_duration = (float)(1000.0f * static_cast<float>(numAnimationFrames)) / (float)framesPerSecond;

		switch (loopInt) {
			case 1: m_animationLoopState = AnimationLoopState::ONCE; break;
			case 2: m_animationLoopState = AnimationLoopState::REPEAT; break;
			default: m_animationLoopState = AnimationLoopState::REPEAT; break;
		}

		

		readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '{');
		while (!readAnimationDef.eof() && readAnimationDef.peek() != '}') {
			int subframeIndex;
			float normalizedTime;
			readAnimationDef >> subframeIndex >> normalizedTime;
			readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			const TextureRect& rect = textureRects[direction * numAnimationFrames + subframeIndex];

			m_animationFrames.push_back({ rect , normalizedTime });
		}
		readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		m_texturesPerDirection[direction] = numAnimationFrames;
		direction = static_cast<Enums::Direction16>(static_cast<int>(direction) + 1);
	}
}

void Animation::loadAnimation8(std::string name, const std::vector<TextureRect>& textureRects) {
	std::ifstream readAnimations(name);

	char resourceFilename[128];

	Enums::Direction8 direction = Enums::Direction8::s;
	while (!readAnimations.eof()) {
		readAnimations >> resourceFilename;

		std::ifstream readAnimationDef(resourceFilename);
		int numAnimationFrames = 0;
		int framesPerSecond = 0;
		int loopInt = 0;

		readAnimationDef >> numAnimationFrames >> framesPerSecond >> loopInt;

		m_duration = (float)(1000.0f * static_cast<float>(numAnimationFrames)) / (float)framesPerSecond;
		switch (loopInt) {
			case 1: m_animationLoopState = AnimationLoopState::ONCE; break;
			case 2: m_animationLoopState = AnimationLoopState::REPEAT; break;
			default: m_animationLoopState = AnimationLoopState::REPEAT; break;
		}

		readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '{');
		while (!readAnimationDef.eof() && readAnimationDef.peek() != '}') {
			int subframeIndex;
			float normalizedTime;
			readAnimationDef >> subframeIndex >> normalizedTime;
			readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			const TextureRect& rect = textureRects[direction * numAnimationFrames + subframeIndex];

			m_animationFrames.push_back({ rect , normalizedTime });
		}
		readAnimationDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		m_texturesPerDirection[direction] = numAnimationFrames;
		direction = static_cast<Enums::Direction8>(static_cast<int>(direction) + 1);
	}
}

const float Animation::getDuration() const {
	return m_duration;
}

const std::vector<AnimationFrame>& Animation::getAnimationFrames() const {
	return m_animationFrames;
}

const unsigned short Animation::getTexturesPerDirection(int direction) const {
	return m_texturesPerDirection.at(direction);
}

const AnimationLoopState Animation::getAnimationLoopState() const {
	return m_animationLoopState;
}

///////////////////////TileSetManager//////////////////////////
AnimationManager AnimationManager::s_instance;

AnimationManager& AnimationManager::Get() {
	return s_instance;
}

Animation& AnimationManager::getAnimation(std::string name) {
	return m_tileSets[name];
}