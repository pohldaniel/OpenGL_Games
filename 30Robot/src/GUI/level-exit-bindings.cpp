#include "level-exit-bindings.hpp"

#include "Event/change-game-state.hpp"
#include "Event/progression-updated.hpp"
#include "Event/selected.hpp"
#include "tags.hpp"
#include "maths.hpp"
#include "constants.hpp"
#include "components/transform.hpp"

NS_IMPLEMENT_REFLECTION(LevelExitBindings) {
	NsMeta<Noesis::TypeId>("LevelExitBindings");
	NsProp("Text", &LevelExitBindings::getText, &LevelExitBindings::setText);
}

LevelExitBindings::LevelExitBindings() : m_text("") {
}

const char* LevelExitBindings::getText() const {
	return m_text.c_str();
}

void LevelExitBindings::setText(const char* value) {
	if (m_text != value) {
		m_text = value;
		OnPropertyChanged("Text");
	}
}
