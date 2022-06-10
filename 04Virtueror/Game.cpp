#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), mIsoMap(new IsoMap()), mGameMap(new GameMap(mIsoMap)) {
	const int rendW = 1600;
	const int rendH = 900;

	Camera::CreateDefaultCamera();
	Camera::CreateDummyCamera();

	auto cam = Camera::GetDefaultCamera();
	cam->SetSize(rendW, rendH);
	m_camController = new CameraMapController(cam);

	m_mapLoader.setMaps(mGameMap, mIsoMap);
	m_mapLoader.loadLevel("res/maps/40x40-01.map");
	const int mapH = mIsoMap->GetHeight();

	mIsoMap->setOrigin(rendW * 0.5, (rendH - mapH) * 0.5);

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
	const int cameraB = p2[1] + tileH + marginCameraV - rendH;

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
	glEnable(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_camController->HandleMouseMotion(event);

	const Camera * cam = m_camController->GetCamera();
	const int worldX = cam->GetScreenToWorldX(event.x);
	const int worldY = cam->GetScreenToWorldY(event.y);
}

void Game::OnKeyDown(Event::KeyboardEvent & event) {
	m_camController->HandleKeyDown();
}

void Game::OnKeyUp(Event::KeyboardEvent & event) {
	m_camController->HandleKeyUp();
}

void Game::CenterCameraOverCell(int row, int col) {
	const Vector2f pos = mIsoMap->GetCellPosition(row, col);

	const int cX = pos[0] + mIsoMap->GetTileWidth() * 0.5f;
	const int cY = pos[1] + mIsoMap->GetTileHeight() * 0.5f;

	m_camController->CenterCameraToPoint(cX, cY);

	const Camera * cam = m_camController->GetCamera();

	m_transform.translate(cam->GetX(), cam->GetY(), 0.0f);
}
