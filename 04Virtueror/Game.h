#pragma once

#include "engine/input/Mouse.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"

#include "Constants.h"
#include "StateMachine.h"

#include "GameObjectAction.h"
#include "MapLoader.h"
#include "CameraMapController.h"
#include "IsoMap.h"
#include "GameMap.h"
#include "IsometricMouse.h"

enum UnitType : unsigned int;

struct ObjectData;
class Pathfinder;

class Game : public State, public MouseEventListener, public KeyboardEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;

	void SetObjectActionCompleted(GameObject * obj);

private:
	MapLoader m_mapLoader;
	CameraMapController * m_camController = nullptr;
	IsometricMouse* m_isometricMouse = nullptr;

	Matrix4f m_transform = Matrix4f::IDENTITY;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent & event) override;
	void OnKeyUp(Event::KeyboardEvent & event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;

	void HandleSelectionClick(Event::MouseButtonEvent& event);
	void HandleActionClick(Event::MouseButtonEvent& event);

	void CenterCameraOverCell(int row, int col);
	void SelectObject(GameObject * obj);
	void ClearSelection();

	bool SetupNewUnit(UnitType type, GameObject * gen);
	bool SetupStructureConquest(Unit * unit, const Cell2D & start, const Cell2D & end);

	void HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell);
	void SetupUnitMove(Unit * unit, const Cell2D & start, const Cell2D & end, const std::function<void()> & onCompleted = [] {});
	void ClearCellOverlays();

	IsoMap * mIsoMap = nullptr;
	GameMap * mGameMap = nullptr;
	Pathfinder * mPathfinder = nullptr;

	float mouseX = 0.0f;
	float mouseY = 0.0f;

	GameObject* selectedObj = nullptr;

	std::vector<GameObjectAction> mActiveObjActions;
	
};