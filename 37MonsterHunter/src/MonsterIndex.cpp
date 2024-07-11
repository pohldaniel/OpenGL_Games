#include <engine/Fontrenderer.h>

#include "MonsterIndex.h"
#include "Globals.h"

MonsterIndex::MonsterIndex() : m_visibleItems(6), m_viewWidth(0.0f), m_viewHeight(0.0f){
	m_names.push_back("Ivieron");
	m_names.push_back("Atrox");
	m_names.push_back("Cindrill");
	m_names.push_back("Atrox");
	m_names.push_back("Sparchu");
	m_names.push_back("Gulfin");
	m_names.push_back("Jacana");
}

MonsterIndex::~MonsterIndex() {

}

void MonsterIndex::draw() {
	Globals::spritesheetManager.getAssetPointer("null")->bind();
	m_surface.setPosition(m_viewWidth * 0.2f, m_viewHeight * 0.1f, 0.0f);
	m_surface.setScale(m_viewWidth * 0.6f, m_viewHeight * 0.8f, 1.0f);
	m_surface.draw(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));

	float itemHeigt = (0.8f * m_viewHeight) / static_cast<float>(m_visibleItems);
	float top = 0.9f * m_viewHeight - itemHeigt;
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	for (int i = 0; i < 6; i++) {
		m_surface.setPosition(m_viewWidth * 0.2f, top - i * itemHeigt, 0.0f);
		m_surface.setScale(m_viewWidth * 0.2f, itemHeigt, 1.0f);
		m_surface.draw(Vector4f(0.0f, 1.0f / (i + 1), 0.0f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), m_viewWidth * 0.2f + 90.0f, top - i * itemHeigt + (0.5f * itemHeigt - lineHeight * 0.5f), m_names[i], Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	}
	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().drawBuffer();
}

void MonsterIndex::setViewWidth(float viewWidth) {
	m_viewWidth = viewWidth;
}

void MonsterIndex::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}