#include <engine/input/Keyboard.h>
#include <engine/TileSet.h>
#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>

#include "Evolve.h"
#include "Globals.h"
#include "MonsterIndex.h"

Evolve::Evolve() :
m_elapsedTime(0.0f),
m_currentFrame(0),
m_frameCount(28),
m_fadeValue(0.0f),
m_fade(m_fadeValue),
m_displayStar(true),
m_activate(false),
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

	if (m_currentMonster == m_endMonster) {
		m_activate = true;
		displayStars();
	}

	Spritesheet::Bind(m_atlasMonster);
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[MonsterIndex::MonsterData[m_currentMonster].graphic * 16];
	m_surface.setShader(shader);
	m_surface.setPosition(0.5f * m_viewWidth - rect.width, 0.5f * m_viewHeight - rect.height, 0.0f);
	m_surface.setScale(2.0f * rect.width, 2.0f *  rect.height, 1.0f);
	m_surface.draw(rect);
	if (m_currentMonster == m_startMonster) {
		float width = Globals::fontManager.get("bold").getWidth(m_startMonster + " is evolving") * 0.05f + 20.0f;
		float lineHeight = Globals::fontManager.get("bold").lineHeight * 0.05f;
		float height = lineHeight + 20.0f;
		shader = Globals::shaderManager.getAssetPointer("list");
		shader->use();
		shader->loadFloat("u_edge", Edge::ALL);
		shader->loadVector("u_dimensions", Vector2f(width, height));
		shader->loadFloat("u_radius", 5.0f);
		shader->unuse();
		m_surface.setShader(shader);
		m_surface.setPosition(0.5f * m_viewWidth - 0.5f * width - 10.0f, 0.5f * m_viewHeight - rect.height - lineHeight - 30.0f, 0.0f);
		m_surface.setScale(width, height, 1.0f);
		m_surface.draw(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

		Globals::fontManager.get("bold").bind();
		Fontrenderer::Get().addText(Globals::fontManager.get("bold"), 0.5f * m_viewWidth - 0.5f * width, 0.5f * m_viewHeight - rect.height - lineHeight - 20.0f, m_startMonster + " is evolving", Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.05f);
	}else {
		float width = Globals::fontManager.get("bold").getWidth(m_startMonster + " evolved into " + m_endMonster) * 0.05f + 20.0f;
		float lineHeight = Globals::fontManager.get("bold").lineHeight * 0.05f;
		float height = lineHeight + 20.0f;
		shader = Globals::shaderManager.getAssetPointer("list");
		shader->use();
		shader->loadFloat("u_edge", Edge::ALL);
		shader->loadVector("u_dimensions", Vector2f(width, height));
		shader->loadFloat("u_radius", 5.0f);
		shader->unuse();
		m_surface.setShader(shader);
		m_surface.setPosition(0.5f * m_viewWidth - 0.5f * width - 10.0f, 0.5f * m_viewHeight - rect.height - lineHeight - 30.0f, 0.0f);
		m_surface.setScale(width, height, 1.0f);
		m_surface.draw(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

		Globals::fontManager.get("bold").bind();
		Fontrenderer::Get().addText(Globals::fontManager.get("bold"), 0.5f * m_viewWidth - 0.5f * width, 0.5f * m_viewHeight - rect.height - lineHeight - 20.0f, m_startMonster + " evolved into " + m_endMonster, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.05f);
	}
		Fontrenderer::Get().drawBuffer();
}

void Evolve::update(float dt) {
	m_fade.update(dt);
	m_exitTimer.update(dt);

	if (m_displayStar && m_activate) {
		m_progress = (m_elapsedTime / static_cast<float>(m_frameCount));
		m_progress *= m_progress;
		m_elapsedTime += 20.0f * dt;		
		m_currentFrame = static_cast<int>(std::floor(m_elapsedTime));
		if (m_currentFrame > m_frameCount - 1) {
			m_currentFrame = 0;
			m_elapsedTime = 0.0f;
			m_displayStar = false;
			m_progress = 0.0f;
		}
	}
}

void Evolve::startEvolution() {
	m_fade.fadeIn();
	m_fade.setOnFadeIn([&m_fade = m_fade, &m_currentMonster = m_currentMonster, m_endMonster = m_endMonster, &m_exitTimer = m_exitTimer, &OnEvolveEnd = OnEvolveEnd, &m_activate = m_activate, &m_displayStar = m_displayStar, &m_curentMonsterIndex = m_curentMonsterIndex]() {
		m_currentMonster = m_endMonster;
		m_fade.setTransitionEnd(false);
		m_fade.setFadeValue(0.0f);

		m_exitTimer.setOnTimerEnd([&OnEvolveEnd = OnEvolveEnd, &m_activate = m_activate, &m_displayStar = m_displayStar, &m_curentMonsterIndex = m_curentMonsterIndex, m_endMonster = m_endMonster] {
			OnEvolveEnd();
			m_activate = false;
			m_displayStar = true;
			if (m_curentMonsterIndex > 0) {
				MonsterIndex::Monsters[m_curentMonsterIndex].name = m_endMonster;
				MonsterIndex::Monsters[m_curentMonsterIndex].resetStates();
			}
			m_curentMonsterIndex = -1;
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

void Evolve::displayStars() {
	if (!m_displayStar)
		return;
	TileSetManager::Get().getTileSet("star").bind();
	const TextureRect& rect = TileSetManager::Get().getTileSet("star").getTextureRects()[m_currentFrame];
	//Batchrenderer::Get().addQuadAA(Vector4f(0.5f * m_viewWidth - rect.width, 0.5f * m_viewHeight - rect.height, 2.0f * rect.width, 2.0f * rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	//Batchrenderer::Get().drawBuffer();

	m_surface.resetShader();
	m_surface.setPosition(0.5f * m_viewWidth - rect.width, 0.5f * m_viewHeight - rect.height, 0.0f);
	m_surface.setScale(2.0f * rect.width, 2.0f *  rect.height, 1.0f);
	m_surface.draw(rect, Vector4f(1.0f, 1.0f, 1.0f, 1.0f - m_progress));
}

void Evolve::setCurentMonsterIndex(int curentMonsterIndex) {
	m_curentMonsterIndex = curentMonsterIndex;
}