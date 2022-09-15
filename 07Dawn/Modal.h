#pragma once

#include "Widget.h"
#include "Frames.h"

class Modal : public Widget {
public:

	Modal() = default;

	Modal(int posX, int posY, int width, int height);
	virtual void draw() override;
	//virtual void clicked(int mouseX, int mouseY, uint8_t mouseState);

	void setAutoresize();
	void setCenteringLayout();
	void setCenterOnScreen();

	virtual void addChildFrame(int relPosX, int relPosY, std::auto_ptr<Widget> newChild);
	virtual int getWidth() const;
	virtual int getHeight() const;

private:
	void applyLayout();
	void resize(int width, int height);
	void recalculatePosition();

	//int contentX;
	//int contentY;
	int blocksX;
	int blocksY;

	bool autoResize;
	bool centeringLayout;
	bool centerOnScreen;
};