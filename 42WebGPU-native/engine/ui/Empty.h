#pragma once

#include <engine/ui/Widget.h>


class Empty : public Widget {
	enum class DragMode {
		NONE,
		HORIZONTAL,
		VERTICAL
	};

public:

	Empty();
	Empty(const Empty& rhs);
	Empty(Empty&& rhs) noexcept;
	virtual ~Empty();

	void setWidth(float width, bool silent = false) override;
	void setHeight(float width, bool silent = false) override;
	//void updateScrollPositions(float percent);
	//void scrollToBottom();
	float m_contentScrollY = 0.0f;
	float m_contentScrollX = 0.0f;
	float m_contentHeight = 0.0f;
	/*void scrollToPositions(float percent);
	void setContentHeight(float contentHeight);
	void updateContentHeightDirect(float contentHeight);
	
	
	float getContentScrollY();
	void scrollByPixels(float pixels);
	float getSliderPosY();*/

	float getHeight() override;
	float getContentHeight();
	void expandAndScrollWithWidget(float newContentHeight);

private:
	
	bool OnInput(int mouseX, int mouseY, bool buttonLeft) override;
	void OnDraw() override;
	bool OnMouseOver(int mouseX, int mouseY) override;
	void OnLayoutChanged() override;
	void OnMouseWheel(int mouseX, int mouseY, float delta) override;
	bool OnOverControl(int mouseX, int mouseY) override;

	bool isOverSliderBottom(int mouseX, int mouseY);
	bool isOverSliderRight(int mouseX, int mouseY);
	
	float m_sliderPosX, m_sliderPosY;
	bool m_isDragged;
	int m_mouseX, m_mouseY;
	DragMode m_dragMode;
	Vector4f m_borderColor;
	Vector4f m_sliderColor;
};