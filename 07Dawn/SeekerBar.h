#pragma once
#define NOMINMAX
#include <algorithm>
#include "Widget.h"

class SeekerBar : public Widget {

public:
	SeekerBar(float& value);
	~SeekerBar();

	void draw() override;
	void processInput() override;
	void setOnClicked(std::function<void()> fun);

private:

	float& m_value;
	std::function<void()> m_onClicked = 0;
};