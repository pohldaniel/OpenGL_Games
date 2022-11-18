#pragma once
#include <vector>

#include "ViewPort.h"

class Widget {
public:
	Widget() = default;
	Widget(short posX, short posY, short width, short height);
	~Widget();

	virtual void draw(int posX, int posY);
	virtual void update(int mouseX, int mouseY);
	virtual void processInput();

	virtual void addChildWidget(int posX, int posY, std::auto_ptr<Widget> newChild);
	virtual int getPosX() const;
	virtual int getPosY() const;
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual void toggle();

	void addToParent(int posX, int posY, Widget* parent);
	void addMoveableFrame(unsigned short titleWidth, unsigned short titleHeight, short titleOffsetX, short titleOffsetY);
	void addCloseButton(unsigned short buttonWidth, unsigned short buttonHeight, short buttonOffsetX, short buttonOffsetY);
	std::vector<Widget*> getChildWidgets();

	void setPosition(int posX, int posY);
	void setSize(int width, int height);

	bool isVisible() const;
	void setVisible(bool visible);
protected:

	bool isMouseOnTitlebar(int mouseX, int mouseY) const;
	bool isMouseOnCloseButton(int mouseX, int mouseY) const;
	void moveFrame(int mouseX, int mouseY);
	void stopMovingFrame();

	short m_posX = 0;
	short m_posY = 0;

	short m_width;
	short m_height;

	bool m_visible = false;

	std::vector<Widget*> m_childWidgets;
	Widget* m_parentFrame;

	unsigned short m_titleWidth;
	unsigned short m_titleHeight;
	short m_titleOffsetX;
	short m_titleOffsetY;
	unsigned short m_startMovingFrameXpos;
	unsigned short m_startMovingFrameYpos;
	bool m_moveableFrame = false;
	bool m_movingFrame = false;

	unsigned short m_buttonWidth;
	unsigned short m_buttonHeight;
	short m_buttonOffsetX;
	short m_buttonOffsetY;
	bool m_closeButton = false;
};