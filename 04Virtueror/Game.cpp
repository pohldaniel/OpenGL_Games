#include "Game.h"
#include "Unit.h"
#include "ObjectData.h"
#include "Pathfinder.h"
#include "ObjectPath.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), mIsoMap(new IsoMap()), mGameMap(new GameMap(mIsoMap)), mPathfinder(new Pathfinder()) {
	m_isometricMouse = new IsometricMouse();
	
	const int rendW = 1600;
	const int rendH = 900;

	Camera::CreateDefaultCamera();
	Camera::CreateDummyCamera();

	auto cam = Camera::GetDefaultCamera();
	cam->SetSize(rendW, rendH);
	m_camController = new CameraMapController(cam);

	m_mapLoader.setMaps(mGameMap, mIsoMap);
	m_mapLoader.loadLevel("res/maps/40x40-01.map");
	
	const Vector2f p0 = mIsoMap->GetCellPosition(0, 0);
	const Vector2f p1 = mIsoMap->GetCellPosition(mIsoMap->GetNumRows() - 1, 0);
	const Vector2f p2 = mIsoMap->GetCellPosition(mIsoMap->GetNumRows() - 1, mIsoMap->GetNumCols() - 1);
	const Vector2f p3 = mIsoMap->GetCellPosition(0, mIsoMap->GetNumCols() - 1);
	const int tileW = TILE_WIDTH;
	const int tileH = TILE_HEIGHT;
	const int marginCameraH = tileW;
	const int marginCameraV = tileH * 2;

	const int cameraL = p1[0] - marginCameraH;
	const int cameraR = p3[0] + tileW + marginCameraH - rendW;
	const int cameraT = p0[1] - marginCameraV;
	const int cameraB = -p2[1] + marginCameraV;

	m_camController->SetLimits(cameraL, cameraR, cameraT, cameraB);
	CenterCameraOverCell(19, 19);

	// init pathfinder
	mPathfinder->SetMap(mGameMap, mGameMap->GetNumRows(), mGameMap->GetNumCols());
}

Game::~Game() {
}

void Game::fixedUpdate() {
}

void Game::update() {
	m_camController->Update(m_dt);
	mGameMap->update(m_dt);
}

void Game::render(unsigned int &frameBuffer) {
	const Camera * cam = m_camController->GetCamera();
	m_transform.translate(cam->GetX(), cam->GetY(), 0.0f);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	mIsoMap->render(m_transform);
	mIsoMap->GetLayer(3)->Render(m_transform);
	//m_isometricMouse->Render(mouseX, mouseY);
	glEnable(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_camController->HandleMouseMotion(event);
}

void Game::OnKeyDown(Event::KeyboardEvent & event) {
	m_camController->HandleKeyDown();

}

void Game::OnKeyUp(Event::KeyboardEvent & event) {
	m_camController->HandleKeyUp();
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	HandleSelectionClick(event);
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	//HandleSelectionClick(event);
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		HandleActionClick(event);
	}
}

void Game::CenterCameraOverCell(int row, int col) {
	const Vector2f pos = mIsoMap->GetCellPosition(row, col);

	const int cX = pos[0] + mIsoMap->GetTileWidth() * 0.5f;
	const int cY = pos[1] + mIsoMap->GetTileHeight() * 0.5f;

	m_camController->CenterCameraToPoint(cX, cY);
	
	const Camera * cam = m_camController->GetCamera();	

	m_transform.translate(cam->GetX(), cam->GetY(), 0.0f);
}

void Game::HandleSelectionClick(Event::MouseButtonEvent& event) {
	float cursorPosNDCX = (2.0f * event.x) / (float)WIDTH - 1.0f;
	float cursorPosNDCY = 1.0f - (2.0f * event.y) / (float)HEIGHT;
	Vector4f m_cursorPosEye = Vector4f(cursorPosNDCX, cursorPosNDCY, -1.0f, 1.0f) ^ Globals::invProjection;

	const Camera * cam = m_camController->GetCamera();
	const int worldX = cam->GetScreenToWorldX(m_cursorPosEye[0]);
	const int worldY = cam->GetScreenToWorldY(m_cursorPosEye[1]);

	const Cell2D clickCell = mIsoMap->CellFromScreenPoint(worldX, worldY);
	
	// clicked outside the map -> clear current selection
	if (!mIsoMap->IsCellInside(clickCell)){
		ClearSelection();
		return;
	}

	const GameMapCell & clickGameCell = mGameMap->GetCell(clickCell.row, clickCell.col);
	GameObject * clickObj = clickGameCell.objTop ? clickGameCell.objTop : clickGameCell.objBottom;
	const bool isClickObjOwn = clickObj != nullptr;
	
	// clicked non-own or no object -> nothing to do
	if (!isClickObjOwn) {
		ClearSelection();
		return;
	}
	

	/*GameObject * currSel = player->GetSelectedObject();*/

	// clicked selected object -> deselect it
	if (clickObj != selectedObj){
		ClearSelection();

	}

	// normal selection -> clear current selection and select clicked object
	//ClearSelection();
	SelectObject(clickObj);

	if (clickObj->GetObjectType() == OBJ_BASE) {
		const auto type = UnitType::UNIT_1;
		SetupNewUnit(type, clickObj);
	}
}

void Game::HandleActionClick(Event::MouseButtonEvent& event){
	//Player * player = GetGame()->GetLocalPlayer();

	// no object selected -> nothing to do
	//if (!player->HasSelectedObject())
		//return;

	
	float cursorPosNDCX = (2.0f * event.x) / (float)WIDTH - 1.0f;
	float cursorPosNDCY = 1.0f - (2.0f * event.y) / (float)HEIGHT;
	Vector4f m_cursorPosEye = Vector4f(cursorPosNDCX, cursorPosNDCY, -1.0f, 1.0f) ^ Globals::invProjection;

	const Camera * cam = m_camController->GetCamera();
	const int worldX = cam->GetScreenToWorldX(m_cursorPosEye[0]);
	const int worldY = cam->GetScreenToWorldY(m_cursorPosEye[1]);

	const Cell2D clickCell = mIsoMap->CellFromScreenPoint(worldX, worldY);

	// clicked outside the map -> nothing to do
	if (!mIsoMap->IsCellInside(clickCell))
		return;

	GameObject * selObj = selectedObj;
	const Cell2D selCell(selObj->GetRow0(), selObj->GetCol0());

	// check if there's a lower object when top is empty
	const GameMapCell & clickGameCell = mGameMap->GetCell(clickCell.row, clickCell.col);
	GameObject * clickObj = clickGameCell.objTop ? clickGameCell.objTop : clickGameCell.objBottom;

	// selected object is a unit
	if (selObj->GetObjectType() == OBJ_UNIT){
		Unit * selUnit = static_cast<Unit *>(selObj);

		const GameObjectActionId action = selUnit->GetActiveAction();

		const bool diffClick = selCell != clickCell;

		// try to move only if clicked on a different cell
		if (diffClick)
			HandleUnitMoveOnMouseUp(selUnit, clickCell);
	}
}

void Game::ClearSelection() {
	if (nullptr == selectedObj)
		return;

	selectedObj->SetSelected(false);
	selectedObj = nullptr;
}

void Game::SelectObject(GameObject * obj) {
	obj->SetSelected(true);
	selectedObj = obj;
}

bool Game::SetupNewUnit(UnitType type, GameObject * gen) {

	const ObjectData & data = ObjectData::NullObj;

	// check if create is possible
	//if (!mGameMap->CanCreateUnit(data, gen, player))
		//return false;

	Cell2D cell = mGameMap->GetNewUnitDestination(gen);
	

	if (-1 == cell.row || -1 == cell.col){
		std::cerr << "GameMap::GetNewUnitDestination FAILED" << std::endl;
		return false;
	}

	// start create
	mGameMap->StartCreateUnit(data, gen, cell);

	gen->SetActiveAction(GameObjectActionId::IDLE);
	gen->SetCurrentAction(GameObjectActionId::BUILD_UNIT);

	// store active action
	mActiveObjActions.emplace_back(gen, GameObjectActionId::BUILD_UNIT, cell);

	gen->SetActiveAction(GameObjectActionId::IDLE);
	gen->SetCurrentAction(GameObjectActionId::BUILD_UNIT);

	// disable actions panel
	//mPanelObjActions->SetActionsEnabled(false);

	gen->SetCurrentAction(GameObjectActionId::IDLE);
	mGameMap->CreateUnit(data, gen, cell);
	SetObjectActionCompleted(gen);

	return true;
}

void Game::SetObjectActionCompleted(GameObject * obj) {

	auto it = mActiveObjActions.begin();

	// search selected object in active actions
	while (it != mActiveObjActions.end()){
		if (it->obj == obj){
			// remove pending action
			mActiveObjActions.erase(it);

			// re-enable actions panel
			//mPanelObjActions->SetActionsEnabled(true);

			// reset object's active action to default
			obj->SetActiveActionToDefault();
			// reset current action to idle
			obj->SetCurrentAction(IDLE);

			return;
		}

		++it;
	}
}

void Game::HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell){
	
	const Cell2D selCell(unit->GetRow0(), unit->GetCol0());

	const bool clickWalkable = mGameMap->IsCellWalkable(clickCell.row, clickCell.col);

	// destination is walkable -> try to generate a path and move
	if (clickWalkable){
		SetupUnitMove(unit, selCell, clickCell);
		return;
	}
	
	//Player * player = GetGame()->GetLocalPlayer();

	const GameMapCell & clickGameCell = mGameMap->GetCell(clickCell.row, clickCell.col);
	const GameObject * clickObj = clickGameCell.objTop;
	const bool clickVisited = clickObj && clickObj->IsVisited();

	// destination never visited (hence not visible as well) -> try to move close
	if (!clickVisited){
		Cell2D target = mGameMap->GetCloseMoveTarget(selCell, clickCell);

		// failed to find a suitable target
		if (-1 == target.row || -1 == target.col)
			return;

		SetupUnitMove(unit, selCell, target);
		return;
	}

	// check if destination obj is visible
	const bool clickVisible = clickObj && clickObj->IsVisible();

	// visited, but not visible object -> exit
	if (!clickVisible)
		return;

	// visible, but it can't be conquered -> exit
	if (!clickObj->CanBeConquered())
		return;

	// object is adjacent -> try to interact
	if (mGameMap->AreObjectsAdjacent(unit, clickObj))
		SetupStructureConquest(unit, selCell, clickCell);
	// object is far -> move close and then try to interact
	else{
		Cell2D target = mGameMap->GetAdjacentMoveTarget(selCell, clickObj);

		// failed to find a suitable target
		if (-1 == target.row || -1 == target.col)
			return;

		SetupUnitMove(unit, selCell, target, [this, unit, clickCell]
		{
			const Cell2D currCell(unit->GetRow0(), unit->GetCol0());

			SetupStructureConquest(unit, currCell, clickCell);
		});
	}
}

void Game::SetupUnitMove(Unit * unit, const Cell2D & start, const Cell2D & end, const std::function<void()> & onCompleted){

	const auto path = mPathfinder->MakePath(start.row, start.col, end.row, end.col, Pathfinder::ALL_OPTIONS);

	// empty path -> exit
	if (path.empty())
		return;

	auto op = new ObjectPath(unit, mIsoMap, mGameMap, this);
	op->SetPathCells(path);
	op->SetOnCompleted(onCompleted);

	const bool res = mGameMap->MoveUnit(op);

	// movement failed
	if (!res)
		return;

	ClearCellOverlays();

	//mPanelObjActions->SetActionsEnabled(false);

	// store active action
	mActiveObjActions.emplace_back(unit, GameObjectActionId::MOVE);

	unit->SetActiveAction(GameObjectActionId::IDLE);
	unit->SetCurrentAction(GameObjectActionId::MOVE);
}

void Game::ClearCellOverlays(){
	/*IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS2);
	layer->ClearObjects();

	layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);
	layer->ClearObjects();

	layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4);
	layer->ClearObjects();

	// delete move indicator
	delete mMoveInd;
	mMoveInd = nullptr;*/
}

bool Game::SetupStructureConquest(Unit * unit, const Cell2D & start, const Cell2D & end) {
	return false;
}