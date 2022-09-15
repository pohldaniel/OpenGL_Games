#pragma once

#include "engine/CharacterSet.h"
#include "engine/Batchrenderer.h"

#include "Widget.h"

#include "Constants.h"

class Label: public Widget {

public:
	Label() = default;
	Label(const CharacterSet& characterSet, std::string text = "");
	~Label() = default;

	Label(Label const& rhs);
	Label& operator=(const Label& rhs);

	void draw();
	void draw(std::string text);
	std::string m_text;
	const CharacterSet* m_characterSet;

	virtual int getWidth() const override;
	virtual int getHeight() const override;
};