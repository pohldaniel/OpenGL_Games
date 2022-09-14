#pragma once

#include "engine/CharacterSet.h"
#include "engine/Batchrenderer.h"

#include "Constants.h"

class LabelNew {

public:
	LabelNew() = default;
	LabelNew(const CharacterSet& characterSet, std::string text = "");
	~LabelNew() = default;

	LabelNew(LabelNew const& rhs);
	LabelNew& operator=(const LabelNew& rhs);

	void draw();
	void draw(std::string text);
	std::string m_text = "New Game";
	const CharacterSet* m_characterSet;
};