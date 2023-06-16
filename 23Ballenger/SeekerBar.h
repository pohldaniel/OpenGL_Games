#pragma once
#include <functional>
#include "engine/input/Event.h"
#include "engine/CharacterSet.h"
#include "engine/Shader.h"
#include "engine/Batchrenderer.h"
#include "Button.h"

class SeekerBar : public Widget {

public:

	SeekerBar();
	SeekerBar(SeekerBar const& rhs);
	SeekerBar& operator=(const SeekerBar& rhs);
	~SeekerBar();

	void draw() override;
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f& position) override;
	void setLeftFunction(std::function<void()> fun);
	void setRightFunction(std::function<void()> fun);
	void setShader(const Shader* shader);
	void setGridSize(const float sx);
	void setCurrentBlock(unsigned int block);
	

private:

	using Widget::setSize;

	Button* m_buttonLeft = nullptr;
	Button* m_buttonRight = nullptr;
	Batchrenderer* m_batchrenderer = nullptr;
	unsigned int m_blocks;
	unsigned int m_currentBlock;
};