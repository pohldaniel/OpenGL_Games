#pragma once
#include "Vector.h"
#include "Button.h"

class SeekerBar{
	friend class Settings;
public:
	SeekerBar() = default;
	SeekerBar(Vector2f position, const unsigned blocks, const unsigned currentBlock);
	SeekerBar(SeekerBar const& rhs);
	SeekerBar& operator=(const SeekerBar& rhs);
	~SeekerBar();

	void render();
	void update();

	void setLeftFunction(std::function<void()> fun);
	void setRightFunction(std::function<void()> fun);

private:

	Shader *m_shader;

	unsigned int m_blocks = 10;
	unsigned int m_currentBlock = 10;

	Vector2f m_size = Vector2f(73, 73);
	std::vector<float> m_vertices;

	Button* m_buttonLeft = nullptr;
	Button* m_buttonRight = nullptr;

	void addQuad(const Vector2f& pos);
	void createBuffer(unsigned int& vao, unsigned int& vbo, std::vector<float>& vertices);
	void setQuadColor(Vector4f& color1, Vector4f& color2);
	void setQuadColor(unsigned int currentBlock, Vector4f& color);

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
};