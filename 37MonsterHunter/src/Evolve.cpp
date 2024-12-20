#include <engine/input/Keyboard.h>
#include <engine/TileSet.h>
#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>

#include <UI/Label.h>

#include "Evolve.h"
#include "Globals.h"
#include "MonsterIndex.h"
#include "Application.h"

Evolve::Evolve() :
m_elapsedTime(0.0f),
m_currentFrame(-1),
m_frameCount(28),
m_fadeValue(0.0f),
m_fade(m_fadeValue),
m_displayStar(false),
m_progress(0.0f),
m_curentMonsterIndex(-1)
{
	m_atlasMonster = TileSetManager::Get().getTileSet("monster").getAtlas();
	m_fade.setTransitionSpeed(0.31372f);

	TextureAtlasCreator::Get().init(2560u, 1440u);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00002.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00003.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00004.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00005.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00006.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00007.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00008.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00009.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00010.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00011.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00012.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00013.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00014.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00015.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00016.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00017.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00018.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00019.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00020.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00021.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00022.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00023.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00024.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00025.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00026.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00027.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00028.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileCpu("res/tmx/graphics/other/star animation/00029.png", false, true, false);
	TileSetManager::Get().getTileSet("star").loadTileSetGpu();
	//Spritesheet::Safe("star", TileSetManager::Get().getTileSet("star").getAtlas());
}

Evolve::~Evolve() {

}

void Evolve::draw() {

	auto shader = Globals::shaderManager.getAssetPointer("evolve");
	shader->use();
	shader->loadFloat("u_fade", m_fadeValue);
	shader->unuse();

	ui::Widget::draw();
}

void Evolve::update(float dt) {
	m_fade.update(dt);
	m_exitTimer.update(dt);

	if (m_displayStar) {
		ui::IconAnimated* iconAnimated = findChild<ui::IconAnimated>("star");
		m_progress = (m_elapsedTime / static_cast<float>(m_frameCount));
		m_progress *= m_progress;
		m_elapsedTime += 20.0f * dt;	
		m_currentFrame = static_cast<int>(std::floor(m_elapsedTime));
		if (m_currentFrame > m_frameCount - 1) {
			m_currentFrame = -1;
			m_elapsedTime = 0.0f;
			m_displayStar = false;
			m_progress = 0.0f;
		}
		iconAnimated->setCurrentFrame(m_currentFrame);
		iconAnimated->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f - m_progress));
	}
}

void Evolve::startEvolution() {
	
	m_fade.fadeIn();
	m_fade.setOnFadeIn([&m_fade = m_fade, &m_currentMonster = m_currentMonster, &m_startMonster = m_startMonster, &m_endMonster = m_endMonster, &m_exitTimer = m_exitTimer, &OnEvolveEnd = OnEvolveEnd, &m_displayStar = m_displayStar, &m_curentMonsterIndex = m_curentMonsterIndex, iconAnimated = findChild<ui::IconAnimated>("icon"), textFiled = findChild<ui::TextField>("textField")]() {
		m_currentMonster = m_endMonster;
		m_fade.setTransitionEnd(false);
		m_fade.setFadeValue(0.0f);

		iconAnimated->setCurrentFrame(MonsterIndex::MonsterData[m_currentMonster].graphic * 16);
		textFiled->setLabel(m_startMonster + " evolved into " + m_endMonster);
		textFiled->setOffsetX(-0.5f * (Globals::fontManager.get("bold").getWidth(m_startMonster + " evolved into " + m_endMonster) * 0.05f + 20.0f));
		m_displayStar = true;

		m_exitTimer.setOnTimerEnd([&OnEvolveEnd = OnEvolveEnd, &m_curentMonsterIndex = m_curentMonsterIndex, m_endMonster = m_endMonster] {				
			if (m_curentMonsterIndex > 0) {			
				MonsterIndex::Monsters[m_curentMonsterIndex].name = m_endMonster;
				MonsterIndex::Monsters[m_curentMonsterIndex].resetStats();
			}
			m_curentMonsterIndex = -1;
			OnEvolveEnd();
		});
		m_exitTimer.start(1800u, false);
	});
}

void Evolve::setViewWidth(float viewWidth) {
	m_viewWidth = viewWidth;
}

void Evolve::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}

void Evolve::setCurrentMonster(std::string currentMonster) {
	m_currentMonster = currentMonster;
}

void Evolve::setStartMonster(std::string startMonster) {
	m_startMonster = startMonster;
}

void Evolve::setEndMonster(const std::string& endMonster) {
	m_endMonster = endMonster;
}

void Evolve::setOnEvolveEnd(std::function<void()> fun) {
	OnEvolveEnd = fun;
}

void Evolve::setCurentMonsterIndex(int curentMonsterIndex) {
	m_curentMonsterIndex = curentMonsterIndex;
	findChild<ui::IconAnimated>("icon")->setCurrentFrame(MonsterIndex::MonsterData[m_currentMonster].graphic * 16);
	findChild<ui::TextField>("textField")->setLabel(m_startMonster + " is evolving");

	float width = Globals::fontManager.get("bold").getWidth(m_startMonster + " is evolving") * 0.05f + 20.0f;
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[MonsterIndex::MonsterData[m_currentMonster].graphic * 16];
	findChild<ui::TextField>("textField")->setOffsetX(-0.5f * width);
	findChild<ui::TextField>("textField")->setOffsetY(-rect.height);
}

void Evolve::initUI(float viewWidth, float viewHeight) {
	setOrigin(static_cast<float>(Application::Width) * 0.5f, static_cast<float>(Application::Height) * 0.5f);

	ui::IconAnimated* iconAnimated = addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("monster").getTextureRects());
	iconAnimated->setPosition(0.5f * m_viewWidth, 0.5f * m_viewHeight);
	iconAnimated->setScale(2.0f, 2.0f);
	iconAnimated->setShader(Globals::shaderManager.getAssetPointer("evolve"));
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("monster").getAtlas());
	iconAnimated->setName("icon");
	iconAnimated->setAligned(true);

	
	float lineHeight = Globals::fontManager.get("bold").lineHeight * 0.05f;
	float height = lineHeight + 20.0f;
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[MonsterIndex::MonsterData[m_currentMonster].graphic * 16];
	ui::TextField* textField = addChild<ui::TextField>(Globals::fontManager.get("bold"));
	textField->setEdge(ui::Edge::ALL);
	textField->setBorderRadius(5.0f);
	textField->setShader(Globals::shaderManager.getAssetPointer("list"));
	textField->setBackgroundColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	textField->setTextColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	textField->setPosition(0.5f * m_viewWidth, 0.5f * m_viewHeight - lineHeight - 30.0f);
	textField->setSize(0.05f);
	textField->setPaddingX(20.0f);
	textField->setPaddingY(20.0f);
	textField->setName("textField");

	iconAnimated = addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("star").getTextureRects());
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("star").getAtlas());
	iconAnimated->setName("star");
	iconAnimated->setPosition(0.5f * m_viewWidth, 0.5f * m_viewHeight);
	iconAnimated->setScale(2.0f, 2.0f);
	iconAnimated->setAligned(true);
}