#pragma once
#include <functional>
#include <UI/WidgetMH.h>

class BarUI : public WidgetMH {

public:

	BarUI(const TextureRect& textureRect);
	BarUI(const BarUI& rhs);
	BarUI(BarUI&& rhs);
	virtual ~BarUI();

	void draw() override;

	void setDrawFunction(std::function<void()> fun);
	void setValue(const float value);
	void setMaxValue(const float maxValue);
	void setWidth(const float width);
	void setHeight(const float height);
	void setRadius(const float radius);
	void setColor(const Vector4f& color);
	void setBgColor(const Vector4f& bgColor);

private:

	void drawDefault();

	std::function<void()> m_draw;
	const TextureRect& textureRect;

	float m_value;
	float m_maxValue;
	float m_height;
	float m_width;
	Vector4f m_bgColor;
	Vector4f m_color;
	float m_radius;
};