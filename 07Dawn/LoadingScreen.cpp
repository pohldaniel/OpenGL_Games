#include "LoadingScreen.h"
#include "LoadingManager.h"
#include "Editor.h"

LoadingScreen::LoadingScreen(StateMachine& machine) : State(machine, CurrentState::LOADINGSCREEN), m_characterSet(Globals::fontManager.get("verdana_20")) {
	if (m_backgroundTextures.size() > 0) {
		return;
	}

	TextureAtlasCreator::Get().init("loadingscreen", 1024, 1024);

	TextureManager::Loadimage("res/interface/LoadingScreen/SilverForest.tga", 0, m_backgroundTextures);
	TextureManager::Loadimage("res/interface/LoadingScreen/BelemarMountains.tga", 1, m_backgroundTextures);
	TextureManager::Loadimage("res/lifebar.tga", 2, m_backgroundTextures);
	m_textureAtlas = TextureAtlasCreator::Get().getAtlas();
	
	// randomly choose background and calculate positions for the background
	m_backgroundToDraw = RNG::randomInt(0, 1);

	m_width = m_backgroundTextures[m_backgroundToDraw].width;
	m_height = m_backgroundTextures[m_backgroundToDraw].height;
	m_posX = (static_cast<short>(ViewPort::Get().getWidth()) - m_width) / 2;
	m_posY = (static_cast<short>(ViewPort::Get().getHeight()) - m_height) / 2;

	m_curText = "";
	m_progress = 0.0;

	LoadingManager::Get().startBackgroundThread();
}

LoadingScreen::~LoadingScreen() {}

void LoadingScreen::fixedUpdate() {

}

void LoadingScreen::update() {
	if (!LoadingManager::Get().isFinished()){
		m_curText = LoadingManager::Get().getActivityText();
		m_progress = LoadingManager::Get().getProgress();
	}else{
		m_isRunning = false;
		m_machine.addStateAtTop(new Editor(m_machine));
	}
}

void LoadingScreen::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	int progressBarWidth = static_cast<int>(ViewPort::Get().getWidth()) / 2;
	int progressBarPosX = (static_cast<int>(ViewPort::Get().getWidth()) - progressBarWidth) / 2;
	int progressBarPosY = 20;

	TextureManager::BindTexture(m_textureAtlas, true);
	TextureManager::DrawTextureBatched(m_backgroundTextures[m_backgroundToDraw], m_posX, m_posY, m_width, m_height, false, false);

	TextureManager::DrawTextureBatched(m_backgroundTextures[2], progressBarPosX, progressBarPosY, m_progress * progressBarWidth, 16, Vector4f(0.75f, 0.2f, 0.2f, 1.0f), false, false);
	TextureManager::DrawTextureBatched(m_backgroundTextures[2], progressBarPosX + static_cast<int>(m_progress * progressBarWidth), progressBarPosY, progressBarWidth - m_progress * progressBarWidth, 16, Vector4f(0.5f, 0.1f, 0.1f, 1.0f), false, false);
	TextureManager::DrawBuffer();

	int textX = m_posX + m_width / 2 - m_characterSet.getWidth(m_curText) / 2;
	int textY = 30 + m_characterSet.lineHeight;
	Fontrenderer::Get().drawText(m_characterSet, textX, textY, m_curText);
	TextureManager::UnbindTexture(true);
}