#include <engine/input/Keyboard.h>
#include <engine/TileSet.h>

#include "Evolve.h"
#include "Globals.h"
#include "MonsterIndex.h"

Evolve::Evolve() :
m_elapsedTime(0.0f),
m_currentFrame(0),
m_frameCount(4),
m_fadeValue(0.0f),
m_fade(m_fadeValue)
{
	m_atlasMonster = TileSetManager::Get().getTileSet("monster").getAtlas();
	m_fade.setTransitionSpeed(0.31372f);
}

Evolve::~Evolve() {

}

void Evolve::draw() {

	auto shader = Globals::shaderManager.getAssetPointer("evolve");
	shader->use();
	shader->loadFloat("u_fade", m_fadeValue);
	shader->unuse();

	Spritesheet::Bind(m_atlasMonster);
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[MonsterIndex::MonsterData[m_currentMonster].graphic * 16 + m_currentFrame];
	m_surface.setShader(shader);
	m_surface.setPosition(0.5f * m_viewWidth - rect.width, 0.5f * m_viewHeight - rect.height, 0.0f);
	m_surface.setScale(2.0f * rect.width, 2.0f *  rect.height, 1.0f);
	m_surface.draw(rect);
}

void Evolve::update(float dt) {
	m_fade.update(dt);
	m_exitTimer.update(dt);
	m_elapsedTime += 6.0f * dt;
	m_currentFrame = static_cast <int>(std::floor(m_elapsedTime));
	if (m_currentFrame > m_frameCount - 1) {
		m_currentFrame = 0;
		m_elapsedTime -= static_cast <float>(m_frameCount);
	}
}

void Evolve::startEvolution() {
	m_fade.fadeIn();
	m_fade.setOnFadeIn([&m_fade = m_fade, &m_currentMonster = m_currentMonster, m_nextMonster = m_nextMonster, &m_exitTimer = m_exitTimer, &OnEvolveEnd = OnEvolveEnd, &m_currentFrame = m_currentFrame, &m_elapsedTime = m_elapsedTime]() {
		m_currentMonster = m_nextMonster;
		m_fade.setTransitionEnd(false);
		m_fade.setFadeValue(0.0f);

		m_exitTimer.setOnTimerEnd([&OnEvolveEnd = OnEvolveEnd, &m_currentFrame = m_currentFrame, &m_elapsedTime = m_elapsedTime] {
			OnEvolveEnd();
			m_currentFrame = 0;
			m_elapsedTime = 0.0f;
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

void Evolve::setNextMonster(const std::string& nextMonster) {
	m_nextMonster = nextMonster;
}

void Evolve::setOnEvolveEnd(std::function<void()> fun) {
	OnEvolveEnd = fun;
}