#pragma once
#include <functional>
#include <UI/WidgetMH.h>

class BarUI : public WidgetMH {

public:

	BarUI(const TextureRect& textureRect);
	BarUI(const BarUI& rhs);
	BarUI& operator=(const BarUI& rhs);
	BarUI(BarUI&& rhs);
	BarUI& operator=(BarUI&& rhs);
	virtual ~BarUI();

	void draw() override;

	void setDrawFunction(std::function<void()> fun);
	
private:

	void drawDefault();

	std::function<void()> m_draw;
	const TextureRect& textureRect;
};