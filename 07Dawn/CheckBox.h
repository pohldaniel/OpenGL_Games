#pragma once

#include "Widget.h"

class CheckBox : public Widget {

public:
	CheckBox();
	~CheckBox();

	void draw() override;
	void processInput() override;


	void setBaseColor(const Vector4f &color);
	void setSelectColor(const Vector4f &color);
	void setOnChecked(std::function<void()> fun);
	void setOnUnchecked(std::function<void()> fun);

private:

	Vector4f baseColor;
	Vector4f selectColor;
	bool m_checked = false;
	std::function<void()> m_onChecked = 0;
	std::function<void()> m_onUnchecked = 0;
};