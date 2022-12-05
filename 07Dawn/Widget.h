#pragma once

#include <vector>
#include <functional>
#include "ViewPort.h"
#include "Constants.h"

class Widget {

public:

	Widget() = default;
	Widget(short posX, short posY, short width, short height, short offsetX, short offsetY);
	~Widget();

	virtual void draw();
	virtual void processInput();
	virtual void addChildWidget(int posX, int posY, std::auto_ptr<Widget> newChild);
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual bool isMouseOnFrame(int mouseX, int mouseY) const;
	virtual void close();

	int getPosX() const;
	int getPosY() const;

	void addToParent(int posX, int posY, Widget* parent);
	void addMoveableFrame(unsigned short titleWidth, unsigned short titleHeight, short titleOffsetX, short titleOffsetY);
	void addCloseButton(unsigned short buttonWidth, unsigned short buttonHeight, short buttonOffsetX, short buttonOffsetY);
	std::vector<Widget*> getChildWidgets();

	void setPosition(int posX, int posY);
	void setSize(int width, int height);

	bool isVisible() const;
	void setVisible(bool visible);
	void resize(int deltaW, int deltaH);
	void setOnClose(std::function<void()> fun);
	void setOnActivate(std::function<void()> fun);
	void activate(); 
	
protected:

	bool isMouseOnTitlebar(int mouseX, int mouseY) const;
	bool isMouseOnCloseButton(int mouseX, int mouseY) const;
	
	void moveFrame(int mouseX, int mouseY);
	void stopMovingFrame();
	std::function<void()> m_onClose = 0;
	std::function<void()> m_onActivate = 0;

	short m_posX = 0;
	short m_posY = 0;

	short m_offsetX = 0;
	short m_offsetY = 0;

	short m_width;
	short m_height;

	bool m_visible = false;
	bool m_active = false;
	std::vector<Widget*> m_childWidgets;
	Widget* m_parentWidget;

	unsigned short m_titleWidth;
	unsigned short m_titleHeight;
	short m_titleOffsetX;
	short m_titleOffsetY;
	unsigned short m_startMovingXpos;
	unsigned short m_startMovingYpos;
	bool m_moveableFrame = false;
	bool m_movingFrame = false;

	unsigned short m_buttonWidth;
	unsigned short m_buttonHeight;
	short m_buttonOffsetX;
	short m_buttonOffsetY;
	bool m_closeButton = false;
};