#pragma once
#include <functional>
#include "../input/Event.h"
#include "../CharacterSet.h"
#include "../Shader.h"
#include "../Batchrenderer.h"
#include "Button.h"

class SeekerBar : public Widget {

public:

	SeekerBar(unsigned int blocks = 10u);
	SeekerBar(SeekerBar const& rhs);
	SeekerBar& operator=(const SeekerBar& rhs);
	~SeekerBar();

	void draw() override;
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f& position) override;
	void setLeftFunction(std::function<void()> fun);
	void setRightFunction(std::function<void()> fun);
	void setGridSize(const float sx);
	void setCurrentBlock(unsigned int block);
	void setSpacing(float spacing);
	void initRenderer(unsigned int blocks = 10u, const Shader* shader = nullptr);	
	void initButtons(const CharacterSet& charset, const Shader* shader = nullptr);

private:

	using Widget::setSize;
	Button* m_buttonLeft = nullptr;
	Button* m_buttonRight = nullptr;
	Batchrenderer* m_batchrenderer = nullptr;
	unsigned int m_blocks;
	unsigned int m_currentBlock;
	float m_spacing;
};