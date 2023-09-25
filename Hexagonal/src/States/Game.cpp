#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

constexpr float DEF_SPEED = 400.f;
constexpr int SCROLL_L = 1;
constexpr int SCROLL_R = -1;
constexpr int SCROLL_U = 1;
constexpr int SCROLL_D = -1;
constexpr int NO_SCROLL = 0;



auto hexcomp = [](const Hex& hex1, const Hex& hex2) {
	//return !((hex1.get_r() > hex2.get_r()) && (hex1.get_q() > hex2.get_q()) && (hex1.get_s() > hex2.get_s())); 
	int dist1 = (std::abs(hex1.get_q()) + std::abs(hex1.get_q() + hex1.get_r()) + std::abs(hex1.get_r())) / 2;
	int dist2 = (std::abs(hex2.get_q()) + std::abs(hex2.get_q() + hex2.get_r()) + std::abs(hex2.get_r())) / 2;


	//return ((hex1.get_r() > hex2.get_r()) && (dist1 < dist2) && (hex1.get_q() > hex2.get_q()));
	return !((hex1.get_r() > hex2.get_r()));

};

auto hexcomp_r = [](const Hex& hex1, const Hex& hex2) {
	return ((hex1.get_r() < hex2.get_r()));

};

auto hexcomp_q = [](const Hex& hex1, const Hex& hex2) {
	return !((hex1.get_q() > hex2.get_q()));

};

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), ISO_WIDTH(96), ISO_HEIGHT(48), HEX_WIDTH(72), HEX_HEIGHT(46), HEX_OFFSET_X(54), HEX_OFFSET_Y(30) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);


	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");

	m_spriteSheetIso = Globals::spritesheetManager.getAssetPointer("isoTiles");
	m_spriteSheetHex = Globals::spritesheetManager.getAssetPointer("hexTiles");

	m_projection = Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);



	std::fstream fs("res/maps/40x40-01.map");
	if (!fs.is_open())
		return;

	std::string line;
	std::istringstream ss;

	// reading map size
	std::getline(fs, line);
	ss.str(line);

	unsigned int rows = 0;
	unsigned int cols = 0;
	ss >> rows >> cols;

	m_cols = cols;
	m_rows = rows;

	m_tileId = new unsigned int*[m_rows];
	for (int i = 0; i < m_rows; ++i)
		m_tileId[i] = new unsigned int[m_cols];

	for (int r = 0; r < rows; ++r) {

		std::getline(fs, line);
		ss.clear();
		ss.str(line);
		for (int c = 0; c < cols; ++c) {
			ss >> m_tileId[r][c];
		
		}
	}
	fs.close();

	for (int r = 0; r < m_rows; ++r) {		
		for (int c = 0; c < m_cols; ++c) {

			float pointX = c * (float)(ISO_WIDTH) * 0.5f;
			float pointY = r * (float)(ISO_WIDTH) * 0.5f;
			float pointXTrans = (pointX - pointY);
			float pointYTrans = (pointX + pointY) * 0.5f;
			isoTiles.push_back({ Vector2f(pointXTrans, -pointYTrans) + Vector2f(Application::Width * 0.5f - (float)(ISO_WIDTH / 2), Application::Height *  0.5f - (float)(ISO_HEIGHT / 0.5f)),Vector2f(ISO_WIDTH, ISO_HEIGHT), m_tileId[r][c] });
		}
	}

	createBoard(2);

	m_isoMap.createBuffer(isoTiles);

	float h = HEX_WIDTH * 0.25f ;
	float s = HEX_WIDTH * 0.5f;
	float r = HEX_HEIGHT * 0.5f;

	/*for (int cx = 0; cx < 20; ++cx) {
		for (int cy = 0; cy < 20; ++cy) {
			
				int renderX = cx * (h + s);
				int renderY = cy * HEX_HEIGHT;

				if (cx % 2 == 0)
					renderY -= r;

				hexTiles.push_back({ Vector2f(renderX, -renderY) + Vector2f(Application::Width * 0.5f - (float)(HEX_WIDTH / 2), Application::Height *  0.5f - (float)(HEX_HEIGHT / 0.5f)),Vector2f(HEX_WIDTH_2, HEX_HEIGHT_2), 0 });


			
		}
	}*/

	/*for (int cx = 0; cx < 20; ++cx) {
		for (int cy = 0; cy < 20; ++cy) {

			int renderX = cx * HEX_WIDTH_2;
			int renderY = cy * (h + s);

			if (cy % 2 == 0)
				renderX += ( s);

			hexTiles.push_back({ Vector2f(renderX, -renderY) + Vector2f(Application::Width * 0.5f - (float)(HEX_WIDTH / 2), Application::Height *  0.5f - (float)(HEX_HEIGHT / 0.5f)),Vector2f(HEX_WIDTH_2, HEX_HEIGHT_2), 0 });



		}
	}*/

	for (int cx = 0; cx < 20; ++cx) {
		for (int cy = 0; cy < 20; ++cy) {

			int renderX = cx ;
			int renderY = cy;

			float pointXTrans = (renderX - renderY) * HEX_OFFSET_X;
			float pointYTrans = (renderX + renderY) * 0.5f * HEX_OFFSET_Y;

			hexTiles.push_back({ Vector2f(pointXTrans, -pointYTrans) + Vector2f(Application::Width * 0.5f - (float)(HEX_WIDTH / 2), Application::Height *  0.5f - (float)(HEX_HEIGHT / 0.5f)),Vector2f(HEX_WIDTH, HEX_HEIGHT), 1 });

		}
	}

	m_hexMap.createBuffer(hexTiles);

	m_shaderLevel = Globals::shaderManager.getAssetPointer("level");

	setLimits(-1216, 1216, -1642, -430);
	CenterCameraOverCell(19, 19);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A) || keyboard.keyDown(Keyboard::KEY_LEFT)) {
		if (!mMouseScrollX) {
			mDirX = SCROLL_R;
			mKeyScrollX = true;
		}
	} else if (keyboard.keyDown(Keyboard::KEY_D) || keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		if (!mMouseScrollX) {
			mDirX = SCROLL_L;
			mKeyScrollX = true;
		}
	} else if (keyboard.keyDown(Keyboard::KEY_W) || keyboard.keyDown(Keyboard::KEY_UP)) {

		if (!mMouseScrollY) {
			mDirY = SCROLL_U;
			mKeyScrollY = true;
		}
	} else if (keyboard.keyDown(Keyboard::KEY_S) || keyboard.keyDown(Keyboard::KEY_DOWN)) {
		if (!mMouseScrollY) {
			mDirY = SCROLL_D;
			mKeyScrollY = true;
		}
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//m_camera.rotate(dx, dy);

		}

		if (move) {
			//m_camera.move(directrion * 5.0f * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
	updateCamera(m_dt);

}

void Game::render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();

	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_projection * m_camera.getViewMatrix());
	Globals::spritesheetManager.getAssetPointer("isoTiles")->bind(0);

	for (int r = 0; r < 40; ++r) {
		for (int c = 0; c < 40; ++c) {

			float pointX = c * (float)(ISO_WIDTH) * 0.5f;
			float pointY = r * (float)(ISO_WIDTH) * 0.5f;
			float pointXTrans = (pointX - pointY) + Application::Width * 0.5f - (float)(ISO_WIDTH / 2);
			float pointYTrans = (pointX + pointY) * 0.5f - (Application::Height *  0.5f - (float)(ISO_HEIGHT / 0.5f));

			Batchrenderer::Get().addQuadAA(Vector4f(pointXTrans, -pointYTrans, ISO_WIDTH, ISO_HEIGHT), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), m_tileId[r][c]);
		}
	}	
	Batchrenderer::Get().drawBufferRaw();
	Globals::spritesheetManager.getAssetPointer("isoTiles")->unbind(0);
	shader->unuse();
	glDisable(GL_BLEND);

	

		/*glEnable(GL_BLEND);
	glUseProgram(m_shaderLevel->m_program);
	m_shaderLevel->loadMatrix("u_transform", m_projection * m_camera.getViewMatrix());
	Globals::spritesheetManager.getAssetPointer("isoTiles")->bind();
	m_isoMap.drawRaw();
	Globals::spritesheetManager.getAssetPointer("isoTiles")->unbind();
	glUseProgram(0);
	glDisable(GL_BLEND);*/

	/*glEnable(GL_BLEND);
	glUseProgram(m_shaderLevel->m_program);
	m_shaderLevel->loadMatrix("u_transform", m_projection * m_camera.getViewMatrix());
	Globals::spritesheetManager.getAssetPointer("hexTiles")->bind();
	m_hexMap.drawRaw();
	Globals::spritesheetManager.getAssetPointer("hexTiles")->unbind();
	glUseProgram(0);
	glDisable(GL_BLEND);

	if (m_drawUi)
		renderUi();*/

}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {
	mDirX = NO_SCROLL;
	mDirY = NO_SCROLL;
	mKeyScrollX = false;
	mKeyScrollY = false;
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Game::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::updateCamera(float delta) {

	const float movX = mDirX * mSpeed * delta;

	if (mDirX < 0) {
		const int newX = static_cast<int>(movX - 0.5f) + m_camera.getPositionX();

		if (newX < mLimitL) {
			m_camera.setPositionX(mLimitL);
		}else {
			m_camera.moveX(movX);
		}
	}
	else if (mDirX > 0) {
		const int newX = static_cast<int>(movX + 0.5f) + m_camera.getPositionX();

		if (newX > mLimitR) {
			m_camera.setPositionX(mLimitR);
		}else {
			m_camera.moveX(movX);
		}
	}

	// VERTICAL
	const float movY = mDirY * mSpeed * delta;

	if (mDirY < 0) {
		const int newY = static_cast<int>(movY - 0.5f) + m_camera.getPositionY();

		if (newY < mLimitT) {
			m_camera.setPositionY(mLimitT);
		}else {
			m_camera.moveY(movY);
		}
	}
	else if (mDirY > 0) {
		const int newY = static_cast<int>(movY + 0.5f) + m_camera.getPositionY();

		if (newY > mLimitB) {
			m_camera.setPositionY(mLimitB);
		}else {
			m_camera.moveY(movY);
		}
	}
}

void Game::setLimits(int l, int r, int t, int b) {
	mLimitL = l;
	mLimitR = r;
	mLimitT = t;
	mLimitB = b;
}

void Game::CenterCameraOverCell(int row, int col) {
	const Vector2f pos = GetCellPosition(row, col);
	const int cX = pos[0] + ISO_WIDTH * 0.5f;
	const int cY = pos[1] + ISO_HEIGHT * 0.5f;
	CenterCameraToPoint(cX, cY);
}

void Game::CenterCameraToPoint(int x, int y) {
	const int hw = Application::Width / 2;
	const int hh = Application::Height / 2;
	const int cameraX0 = x - hw;
	const int cameraY0 = y - hh;

	// clamp X
	if (cameraX0 < mLimitL)
		x = mLimitL + hw;
	else if (cameraX0 > mLimitR)
		x = mLimitR + hw;

	// clamp Y
	if (cameraY0 < mLimitT)
		y = hh - mLimitT;
	else if (cameraY0 > mLimitB)
		y = hh - mLimitB;

	CenterToPoint(x, y);
}

void Game::CenterToPoint(int x, int y) {
	float mXf = x - Application::Width / 2;
	float mYf = y - Application::Height / 2;

	int mXd = std::roundf(mXf);
	int mYd = std::roundf(mYf);

	m_camera.setPosition(mXf, mYf, 0.0f);
}

Vector2f Game::GetCellPosition(unsigned int r, unsigned int c) const {
	const unsigned int ind = r * m_cols + c;
	return GetCellPosition(ind);
}

Vector2f Game::GetCellPosition(unsigned int index) const {

	if (index < isoTiles.size()) {
		return isoTiles[index].position;
	}else {
		const Vector2f p(-1, -1);
		return p;
	}
}

void Game::createBoard(int nr){
	//clear the list and the map
	hexlist.clear();
	heights.clear();

	int dist = 4;

	switch (nr)
	{
	case 0: //Rectangle 1
		//sBoardName = "Rectangle 1";
		for (int q = -3; q <= 3; q++)
		{ // flat top
			int q_offset = floor((q - 1) / 2.0); // or q>>1
			for (int r = -5 - q_offset; r <= 4 - q_offset; r++)
			{
				if (Hex::hex_distance(Hex(q, r, -q - r), Hex(0, 0, 0)) != 1)
				{
					hexlist.push_back(Hex(q, r, -q - r));
					heights[Hex(q, r, -q - r)] = std::abs(r) % 9 + 1;
				}
			}
		}

		break;

	case 1: //Rectangle 2
			//sBoardName = "Rectangle 2";
		for (int q = -3; q <= 3; q++)
		{ // flat top
			int q_offset = floor((q + 1) / 2.0);
			for (int r = -4 - q_offset; r <= 4 - q_offset; r++)
			{
				if (Hex::hex_distance(Hex(q, r, -q - r), Hex(0, 0, 0)) != 1)
				{
					hexlist.push_back(Hex(q, r, -q - r));
					heights[Hex(q, r, -q - r)] = std::abs(r) % 9 + 1;
				}
			}
		}
		for (int q = -3; q <= 3; q += 2)
		{ // flat top
			int r = 4 - floor((q) / 2.0);
			hexlist.push_back(Hex(q, r, -q - r));
			heights[Hex(q, r, -q - r)] = std::abs(r) % 9 + 1;
			//for (int r = -4 - q_offset; r <= 4 - q_offset; r+=2)
		}

		break;

	case 2: //Hexagon
			//sBoardName = "Hexagon";
		dist = 4;
		for (int q = -dist; q <= dist; q++)
		{
			int r1 = std::max(-dist, -q - dist);
			int r2 = std::min(dist, -q + dist);
			for (int r = r1; r <= r2; r++)
			{
				hexlist.push_back(Hex(q, r, -q - r));
				heights[Hex(q, r, -q - r)] = (Hex::hex_distance(Hex(q, r, -q - r), Hex(0, 0, 0)) + 4) % 9 + 1;
			}
		}
		break;

	case 3: //Parallelograms 1
			//sBoardName = "Parallelogram 1";
		dist = 3;
		for (int q = -dist; q <= dist; q++)
		{
			for (int r = -3; r <= 3; r++)
			{
				if (!((r == 0) && (q == 0)))
				{
					hexlist.push_back(Hex(q, r, -q - r));
					heights[Hex(q, r, -q - r)] = (std::abs(r)) % 9 + 1;
				}
			}
		}
		break;

	case 4: //Parallelograms 2
			//sBoardName = "Parallelogram 2";
		dist = 2;
		for (int r = -dist; r <= dist; r++)
		{
			for (int s = -2; s <= 2; s++)
			{
				if (!((r == 0) && (s == 0)))
				{
					hexlist.push_back(Hex(-r - s, r, s));
					heights[Hex(-r - s, r, s)] = (std::abs(r)) % 9 + 1;
				}
			}
		}
		break;

	case 5: // Parallelograms 3
			//sBoardName = "Parallelogram 3";
		dist = 2;
		for (int s = -dist; s <= dist; s++)
		{
			for (int q = -3; q <= 3; q++)
			{
				if (!((s == 0) && (q == 0)))
				{
					hexlist.push_back(Hex(q, -q - s, s));
					heights[Hex(q, -q - s, s)] = (std::abs(q)) % 9 + 1;
				}
			}
		}
		break;

	case 6: // Triangle 1
			//sBoardName = "Triangle 1";
		dist = 6;
		for (int q = 0; q <= dist; q++)
		{
			for (int r = dist - q; r <= dist; r++)
			{
				hexlist.push_back(Hex(q - 2, r - dist + 1, -q + 2 - r + dist - 1));
				heights[Hex(q - 2, r - dist + 1, -q + 2 - r + dist - 1)] = (std::abs(q)) % 9 + 1;
			}
		}
		break;

	case 7: // Triangle 2
			//sBoardName = "Triangle 2";
		dist = 6;
		for (int q = 0; q <= dist; q++)
		{
			for (int r = 0; r <= dist - q; r++)
			{
				hexlist.push_back(Hex(q - 2, (r - dist / 2 + 1), -(q - 2) + (-r + dist / 2 - 1)));
				heights[Hex(q - 2, (r - dist / 2 + 1), -(q - 2) + (-r + dist / 2 - 1))] = (std::abs(q)) % 9 + 1;
			}
		}
		break;

	default: break;
	}

	// center tile 0,0 to screen center
	//vOriginScreen.x = Application::Width / 2 - HEX_WIDTH / 2;
	//vOriginScreen.y = Application::Height / 2 - HEX_HEIGHT / 2;

	// sort the list to draw from top to bottom
	hexlist.sort(hexcomp_q);
	hexlist.sort(hexcomp_r);

}