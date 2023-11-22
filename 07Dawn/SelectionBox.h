#pragma once

#include "engine/CharacterSet.h"
#include "engine/Fontrenderer.h"
#include "Widget.h"


class SelectionBox : public Widget {
public:
	SelectionBox(const CharacterSet& font, const CharacterSet& selectFont, int& selected);
	~SelectionBox();

	void draw() override;
	void processInput() override;
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	void setOnSelected(std::function<void(int)> fun);

	int getWidth() const override;
	int getHeight() const override;

	void setBaseColor(const Vector4f &color);
	void setSelectColor(const Vector4f &color);
	void setFont(const CharacterSet& font);
	void setSelectFont(const CharacterSet& selectFont);
	void setEntries(std::vector<std::string> entries, int initialSelected);
	int getSelected() const;
	void setSelected(int selected);

private:
	const CharacterSet* font;
	const CharacterSet* selectFont;
	Vector4f baseColor;
	Vector4f selectColor;
	std::vector<std::string> entries;
	
	int maxWidth;
	int maxSelectedWidth;
	enum State { OPEN, CLOSED } state;
	std::function<void(int)> m_onSelected = 0;

	int& m_selected;
};