#pragma once
#include <functional>
#include "engine/input/Event.h"
#include "engine/CharacterSet.h"
#include "engine/Shader.h"
#include "engine/Batchrenderer.h"
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
	void initButtons(const CharacterSet& charset, const Shader* shader);
	void initRenderer(const Shader* shader, unsigned int blocks = 10u);

private:

	using Widget::setSize;
	Button* m_buttonLeft = nullptr;
	Button* m_buttonRight = nullptr;
	Batchrenderer* m_batchrenderer = nullptr;
	unsigned int m_blocks;
	unsigned int m_currentBlock;
	float m_spacing;
};