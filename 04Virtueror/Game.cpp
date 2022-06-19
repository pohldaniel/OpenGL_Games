#include "Game.h"
#include "Unit.h"
#include "ObjectData.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), mIsoMap(new IsoMap()), mGameMap(new GameMap(mIsoMap)) {
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
}

Game::~Game() {
}

void Game::fixedUpdate() {
}

void Game::update() {
	m_camController->Update(m_dt);
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
	Vector4f m_cursorPosEye = Globals::invProjection * Vector4f(cursorPosNDCX, cursorPosNDCY, -1.0f, 1.0f);

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

	const auto type = UnitType::UNIT_1;
	SetupNewUnit(type, clickObj);
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