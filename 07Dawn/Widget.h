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

	virtual void addChildWidget(int posX, int posY, std::auto_ptr<Widget> newChild);
	virtual int getPosX() const;
	virtual int getPosY() const;
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual void toggle();

	void addToParent(int posX, int posY, Widget* parent);
	std::vector<Widget*> getChildWidgets();

	void setPosition(int posX, int posY);
	void setSize(int width, int height);
protected:

	short m_posX = 0;
	short m_posY = 0;

	short m_width;
	short m_height;

	std::vector<Widget*> m_childWidgets;
	Widget* m_parentFrame;
};