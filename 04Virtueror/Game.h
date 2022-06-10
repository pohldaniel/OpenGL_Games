#pragma once

#include "engine/input/Mouse.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"

#include "Constants.h"
#include "StateMachine.h"

#include "MapLoader.h"
#include "CameraMapController.h"
#include "IsoMap.h"
#include "GameMap.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;

private:
	MapLoader m_mapLoader;
	CameraMapController * m_camController = nullptr;

	Matrix4f m_transform = Matrix4f::IDENTITY;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent & event) override;
	void OnKeyUp(Event::KeyboardEvent & event) override;
	void CenterCameraOverCell(int row, int col);

	IsoMap * mIsoMap = nullptr;
	GameMap * mGameMap = nullptr;
};