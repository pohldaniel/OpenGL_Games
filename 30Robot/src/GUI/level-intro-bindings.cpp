#include "level-intro-bindings.hpp"

#include "Event/change-game-state.hpp"
#include "Event/progression-updated.hpp"
#include "Event/selected.hpp"
#include "tags.hpp"
#include "maths.hpp"
#include "constants.hpp"
#include "components/transform.hpp"

NS_IMPLEMENT_REFLECTION(LevelIntroBindings) {
	NsMeta<Noesis::TypeId>("LevelIntroBindings");
	NsProp("Title", &LevelIntroBindings::getTitle, &LevelIntroBindings::setTitle);
	NsProp("ImagePath", &LevelIntroBindings::getImagePath, &LevelIntroBindings::setImagePath);
	NsProp("Text", &LevelIntroBindings::getText, &LevelIntroBindings::setText);
}

LevelIntroBindings::LevelIntroBindings() : m_title("-"), m_imagePath(""), m_text("-") {
}

const char* LevelIntroBindings::getTitle() const {
	return m_title.c_str();
}

void LevelIntroBindings::setTitle(const char* value) {
	if (m_title != value) {
		m_title = value;
		OnPropertyChanged("Title");
	}
}

const char* LevelIntroBindings::getImagePath() const {
	return m_imagePath.c_str();
}

void LevelIntroBindings::setImagePath(const char* value) {
	if (m_imagePath != value) {
		m_imagePath = value;
		OnPropertyChanged("ImagePath");
	}
}

const char* LevelIntroBindings::getText() const {
	return m_text.c_str();
}

void LevelIntroBindings::setText(const char* value) {
	if (m_text != value) {
		m_text = value;
		OnPropertyChanged("Text");
	}
}
