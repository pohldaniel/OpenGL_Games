#pragma once
#include <functional>
#include <engine/CharacterSet.h>
#include <UI/WidgetMH.h>

class Label : public WidgetMH {

public:

	Label(const CharacterSet& characterSet);
	Label(const Label& rhs);
	Label& operator=(const Label& rhs);
	Label(Label&& rhs);
	Label& operator=(Label&& rhs);
	virtual ~Label();

	void draw() override;

	void setDrawFunction(std::function<void()> fun);
	void setLabel(const std::string& label);
	void setColor(const Vector4f& color);
	void setSize(const float size);
	void setOffsetX(const float offsetX);
	void setOffsetY(const float offsetY);

private:

	void drawDefault();

	std::function<void()> m_draw;
	std::string m_label;
	Vector4f m_color;
	float m_size;
	float m_offsetX, m_offsetY;

	const CharacterSet& characterSet;
};