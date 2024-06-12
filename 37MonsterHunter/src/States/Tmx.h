#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/Texture.h>
#include <engine/Rect.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

struct Cell {
	const TextureRect& rect;
	float posX;
	float posY;
};

class Tmx : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Tmx(StateMachine& machine);
	~Tmx();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();
	void loadMap(const std::string& path);

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;

	
	std::vector<unsigned> m_tileTextures;
	std::vector<std::pair<int, unsigned int>**> m_layer;
	std::vector<Cell> m_cells;
	unsigned int m_atlas;
	float m_mapHeight, m_tileHeight, m_tileWidth;
};