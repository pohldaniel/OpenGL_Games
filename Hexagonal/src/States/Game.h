#pragma once

#include <fstream>
#include <sstream>
#include <memory>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>

#include "StateMachine.h"
#include "Background.h"
#include "TileSet.h"
#include "Enums.h"
#include "Animation.h"
#include "Map.h"

#include "Entity.h"
#include "AnimationController.h"
#include "Prefab.h"
#include "Movement.h"

enum SelectionMode {
	BOXSELECTION,
	ISOSELECTION,
	MARKER,
	RASTERIZER,
	ENTITY
};

class Game : public State, public MouseEventListener, public KeyboardEventListener {

	friend eMovementPlanner;

public:

	Game(StateMachine& machine);
	~Game();

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

	static void DrawIsometricRect(float posX, float posY, const Vector4f& sizeOffset, const Vector4f& color);
	static void DrawIsometricRect(float posX, float posY, const Vector4f& bounds, const Vector2f& offset, const Vector4f& color);
	static void DrawIsometricRect(int posX, int posY, const Vector4f& color);
	static void DrawIsometricLine(const Vector2f& start, const Vector2f& end, const Vector2f& offset, const Vector4f& color);

private:

	void applyTransformation(TrackBall& arc);
	void renderUi();

	bool isValid(const int row, const int column) const;

	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_autoRedraw = false;
	bool m_drawCullingRect = false;
	bool m_redrawMap = true;

	Background m_background;
	ZoomableQuad m_zoomableQuad;
	Framebuffer m_mainRT;

	unsigned int m_atlas;

	std::vector<Entity*> m_entities;

	bool move;
	bool m_mouseDown = false;
	bool m_mouseMove = false;
	float m_mouseX, m_mouseY;
	float m_curMouseX, m_curMouseY;

	SelectionMode m_selectionMode = SelectionMode::ENTITY;

	
	void drawIsometricRect(float posX, float posY, Vector4f sizeOffset, Vector4f color);
	void drawIsometricRect(float posX, float posY, Vector4f bounds, Vector2f offset, Vector4f color);
	void drawIsometricRect(int posX, int posY, Vector4f color);

	void drawClickBox(float posX, float posY, float width, float height);

	static Camera _Camera;
	static float ZoomFactor;
	static float FocusPointY;
	static float FocusPointX;

	Map m_map;
};