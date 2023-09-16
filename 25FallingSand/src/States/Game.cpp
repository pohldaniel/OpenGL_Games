#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include "MainMenuUI.h"
#include "DebugUI.h"
#include "Textures.hpp"


Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	
	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	
	m_background1.setLayer(std::vector<BackgroundLayer>{{&Globals::textureManager.get("bg_layer_2"), 2, 1.0f, 0.5f, Vector2f(0.333f, 0.666f)},
                                                        {&Globals::textureManager.get("bg_layer_3"), 2, 0.0f, -0.0666f, Vector2f(0.37f, 0.37f + 0.333f)},
                                                        {&Globals::textureManager.get("bg_layer_4"), 2, 3.0f, -0.01f, Vector2f(0.333f, 0.666f)},
                                                        {&Globals::textureManager.get("bg_layer_5"), 2, 0.0f, 0.01333f, Vector2f(0.333f, 0.666f)} });
	m_background1.setSpeed(0.005f);

	m_background2.setLayer(std::vector<BackgroundLayer>{{&Globals::textureManager.get("forest_1"), 1, 1.0f},
														{&Globals::textureManager.get("forest_2"), 1, 2.0f},
														{&Globals::textureManager.get("forest_3"), 1, 3.0f},
														{&Globals::textureManager.get("forest_4"), 1, 4.0f},
														{&Globals::textureManager.get("forest_5"), 1, 5.0f}});
	m_background2.setSpeed(0.005f);

	MainMenuUI::Setup();
	Materials::init();


	world = new World();
	world->noSaveLoad = true;
	world->init(gameDir.getWorldPath("mainMenu"), (int)ceil(Application::Width / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(Application::Height / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, target, NetworkMode::SERVER);

	for (int x = -CHUNK_W * 4; x < world->width + CHUNK_W * 4; x += CHUNK_W) {
		for (int y = -CHUNK_H * 3; y < world->height + CHUNK_H * 8; y += CHUNK_H) {
			world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
		}
	}

	updateDirtyPool = new ctpl::thread_pool(6);
	

	prevWidth = Application::Width;
	prevHeight = Application::Height;

	newWidth = Application::Width;
	newHeight = Application::Height;

	/*loadingOnColor = 0xFFFFFFFF;
	loadingOffColor = 0x000000FF;

	loadingTexture = GPU_CreateImage(loadingScreenW = (WIDTH / 20), loadingScreenH = (HEIGHT / 20), GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(loadingTexture, GPU_FILTER_NEAREST);

	texture = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);

	worldTexture = GPU_CreateImage(world->width * Settings::hd_objects_size, world->height * Settings::hd_objects_size, GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(worldTexture, GPU_FILTER_NEAREST);

	GPU_LoadTarget(worldTexture);


	lightingTexture = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(lightingTexture, GPU_FILTER_NEAREST);
	GPU_LoadTarget(lightingTexture);

	emissionTexture = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(emissionTexture, GPU_FILTER_NEAREST);

	textureFlow = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureFlow, GPU_FILTER_NEAREST);

	textureFire = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureFire, GPU_FILTER_NEAREST);

	texture2Fire = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(texture2Fire, GPU_FILTER_NEAREST);

	GPU_LoadTarget(texture2Fire);

	textureLayer2 = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureLayer2, GPU_FILTER_NEAREST);

	textureBackground = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureBackground, GPU_FILTER_NEAREST);

	textureObjects = GPU_CreateImage(world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureObjects, GPU_FILTER_NEAREST);

	textureObjectsLQ = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureObjectsLQ, GPU_FILTER_NEAREST);

	textureObjectsBack = GPU_CreateImage(world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureObjectsBack, GPU_FILTER_NEAREST);

	GPU_LoadTarget(textureObjects);
	GPU_LoadTarget(textureObjectsLQ);
	GPU_LoadTarget(textureObjectsBack);
	textureParticles = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(textureParticles, GPU_FILTER_NEAREST);

	textureEntities = GPU_CreateImage(world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_LoadTarget(textureEntities);
	GPU_SetImageFilter(textureEntities, GPU_FILTER_NEAREST);

	textureEntitiesLQ = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_LoadTarget(textureEntitiesLQ);
	GPU_SetImageFilter(textureEntitiesLQ, GPU_FILTER_NEAREST);

	temperatureMap = GPU_CreateImage(world->width, world->height,GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(temperatureMap, GPU_FILTER_NEAREST);

	backgroundImage = GPU_CreateImage(Application::Width, Application::Height,GPU_FormatEnum::GPU_FORMAT_RGBA);
	GPU_SetImageFilter(backgroundImage, GPU_FILTER_NEAREST);
	GPU_LoadTarget(backgroundImage);


	pixels = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixels_ar = &pixels[0];
	pixelsLayer2 = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsLayer2_ar = &pixelsLayer2[0];
	pixelsBackground = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsBackground_ar = &pixelsBackground[0];
	pixelsObjects = std::vector<unsigned char>(world->width * world->height * 4, SDL_ALPHA_TRANSPARENT);
	pixelsObjects_ar = &pixelsObjects[0];
	pixelsTemp = std::vector<unsigned char>(world->width * world->height * 4, SDL_ALPHA_TRANSPARENT);
	pixelsTemp_ar = &pixelsTemp[0];
	pixelsParticles = std::vector<unsigned char>(world->width * world->height * 4, SDL_ALPHA_TRANSPARENT);
	pixelsParticles_ar = &pixelsParticles[0];
	pixelsLoading = std::vector<unsigned char>(loadingTexture->w * loadingTexture->h * 4, SDL_ALPHA_TRANSPARENT);
	pixelsLoading_ar = &pixelsLoading[0];
	pixelsFire = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsFire_ar = &pixelsFire[0];
	pixelsFlow = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsFlow_ar = &pixelsFlow[0];
	pixelsEmission = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsEmission_ar = &pixelsEmission[0];
	loadShaders();

	accLoadX -= (newWidth - prevWidth) / 2.0f / scale;
	accLoadY -= (newHeight - prevHeight) / 2.0f / scale;
	movingTiles = new uint16_t[Materials::nMaterials];
	tickChunkLoading();

	for (int x = 0; x < world->width; x++) {
		for (int y = 0; y < world->height; y++) {
			world->dirty[x + y * world->width] = true;
			world->layer2Dirty[x + y * world->width] = true;
			world->backgroundDirty[x + y * world->width] = true;
		}
	}

	long long lastFPS = Time::millis();
	int frames = 0;
	fps = 0;

	lastTime = Time::millis();
	lastTick = lastTime;
	long long lastTickPhysics = lastTime;

	mspt = 33;
	long msptPhysics = 16;

	scale = 3;
	ofsX = (int)(-CHUNK_W * 4);
	ofsY = (int)(-CHUNK_H * 2.5);

	ofsX = (ofsX - WIDTH / 2) / 2 * 3 + WIDTH / 2;
	ofsY = (ofsY - HEIGHT / 2) / 2 * 3 + HEIGHT / 2;

	for (int i = 0; i < frameTimeNum; i++) {
		frameTime[i] = 0;
	}
	objectDelete = new bool[world->width * world->height];


	fadeInStart = Time::millis();
	fadeInLength = 250;
	fadeInWaitFrames = 5;*/
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
		m_background1.addOffset(-0.001f);		
		m_background2.addOffset(-0.001f);
		m_background1.setSpeed(-0.005f);
		m_background2.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background1.addOffset(0.001f);
		m_background2.addOffset(0.001f);
		m_background1.setSpeed(0.005f);
		m_background2.setSpeed(0.005f);
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

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_switch = !m_switch;
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(directrion * 5.0f * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background1.update(m_dt);
	m_background2.update(m_dt);
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_switch ? m_background1.draw() : m_background2.draw();

	/*for (size_t i = 0; i < world->rigidBodies.size(); i++) {
		RigidBody* cur = world->rigidBodies[i];
		if (cur == nullptr) continue;
		if (cur->surface == nullptr) continue;
		if (!cur->body->IsEnabled()) continue;

		float x = cur->body->GetPosition().x;
		float y = cur->body->GetPosition().y;

		// draw
		GPU_Target* tgt = cur->back ? textureObjectsBack->target : textureObjects->target;
		GPU_Target* tgtLQ = cur->back ? textureObjectsBack->target : textureObjectsLQ->target;
		int scaleObjTex = Settings::hd_objects ? Settings::hd_objects_size : 1;

		GPU_Rect r = { x * scaleObjTex, y * scaleObjTex, (float)cur->surface->w * scaleObjTex, (float)cur->surface->h * scaleObjTex };

		if (cur->texNeedsUpdate) {
			if (cur->texture != nullptr) {
				GPU_FreeImage(cur->texture);
			}
			cur->texture = GPU_CopyImageFromSurface(cur->surface);
			GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
			cur->texNeedsUpdate = false;
		}

		GPU_BlitRectX(cur->texture, NULL, tgt, &r, cur->body->GetAngle() * 180 / (float)PI, 0, 0, GPU_FLIP_NONE);

		float s = sin(cur->body->GetAngle());
		float c = cos(cur->body->GetAngle());

		std::vector<std::pair<int, int>> checkDirs = { { 0, 0 },{ 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 } };

		for (int tx = 0; tx < cur->matWidth; tx++) {
			for (int ty = 0; ty < cur->matHeight; ty++) {
				MaterialInstance rmat = cur->tiles[tx + ty * cur->matWidth];
				if (rmat.mat->id == Materials::GENERIC_AIR.id) continue;

				// rotate point
				int wx = (int)(tx * c - (ty + 1) * s + x);
				int wy = (int)(tx * s + (ty + 1) * c + y);

				for (auto& dir : checkDirs) {
					int wxd = wx + dir.first;
					int wyd = wy + dir.second;

					if (wxd < 0 || wyd < 0 || wxd >= world->width || wyd >= world->height) continue;
					if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::AIR) {
						world->tiles[wxd + wyd * world->width] = rmat;
						world->dirty[wxd + wyd * world->width] = true;
						//objectDelete[wxd + wyd * world->width] = true;
						break;
					}
					else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SAND) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						//objectDelete[wxd + wyd * world->width] = true;
						world->dirty[wxd + wyd * world->width] = true;
						cur->body->SetLinearVelocity({ cur->body->GetLinearVelocity().x * (float)0.99, cur->body->GetLinearVelocity().y * (float)0.99 });
						cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.98);
						break;
					}
					else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SOUP) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						//objectDelete[wxd + wyd * world->width] = true;
						world->dirty[wxd + wyd * world->width] = true;
						cur->body->SetLinearVelocity({ cur->body->GetLinearVelocity().x * (float)0.998, cur->body->GetLinearVelocity().y * (float)0.998 });
						cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.99);
						break;
					}
				}
			}
		}
	}*/


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	MainMenuUI::Draw(this);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	//if (m_drawUi)
	//	renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = true;
		Mouse::instance().detach();
		Keyboard::instance().disable();
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
		//m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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


	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::updateFrameEarly() {


	// handle controls
#pragma region
	if (Controls::DEBUG_UI->get()) {
		DebugUI::visible ^= true;
		DebugDrawUI::visible ^= true;
		DebugCheatsUI::visible ^= true;
	}

	if (Settings::draw_frame_graph) {
		if (Controls::STATS_DISPLAY->get()) {
			Settings::draw_frame_graph = false;
			Settings::draw_debug_stats = false;
			Settings::draw_chunk_state = false;
			Settings::draw_detailed_material_info = false;
		}
	}
	else {
		if (Controls::STATS_DISPLAY->get()) {
			Settings::draw_frame_graph = true;
			Settings::draw_debug_stats = true;

			if (Controls::STATS_DISPLAY_DETAILED->get()) {
				Settings::draw_chunk_state = true;
				Settings::draw_detailed_material_info = true;
			}
		}
	}



	if (Controls::DEBUG_REFRESH->get()) {
		for (int x = 0; x < world->width; x++) {
			for (int y = 0; y < world->height; y++) {
				world->dirty[x + y * world->width] = true;
				world->layer2Dirty[x + y * world->width] = true;
				world->backgroundDirty[x + y * world->width] = true;
			}
		}
	}

	if (Controls::DEBUG_RIGID->get()) {
		for (auto& cur : world->rigidBodies) {
			if (cur->body->IsEnabled()) {
				float s = sin(cur->body->GetAngle());
				float c = cos(cur->body->GetAngle());
				bool upd = false;

				for (int xx = 0; xx < cur->matWidth; xx++) {
					for (int yy = 0; yy < cur->matHeight; yy++) {
						int tx = xx * c - yy * s + cur->body->GetPosition().x;
						int ty = xx * s + yy * c + cur->body->GetPosition().y;

						MaterialInstance tt = cur->tiles[xx + yy * cur->matWidth];
						if (tt.mat->id != Materials::GENERIC_AIR.id) {
							if (world->tiles[tx + ty * world->width].mat->id == Materials::GENERIC_AIR.id) {
								world->tiles[tx + ty * world->width] = tt;
								world->dirty[tx + ty * world->width] = true;
							}
							else if (world->tiles[(tx + 1) + ty * world->width].mat->id == Materials::GENERIC_AIR.id) {
								world->tiles[(tx + 1) + ty * world->width] = tt;
								world->dirty[(tx + 1) + ty * world->width] = true;
							}
							else if (world->tiles[(tx - 1) + ty * world->width].mat->id == Materials::GENERIC_AIR.id) {
								world->tiles[(tx - 1) + ty * world->width] = tt;
								world->dirty[(tx - 1) + ty * world->width] = true;
							}
							else if (world->tiles[tx + (ty + 1) * world->width].mat->id == Materials::GENERIC_AIR.id) {
								world->tiles[tx + (ty + 1) * world->width] = tt;
								world->dirty[tx + (ty + 1) * world->width] = true;
							}
							else if (world->tiles[tx + (ty - 1) * world->width].mat->id == Materials::GENERIC_AIR.id) {
								world->tiles[tx + (ty - 1) * world->width] = tt;
								world->dirty[tx + (ty - 1) * world->width] = true;
							}
							else {
								world->tiles[tx + ty * world->width] = Tiles::createObsidian(tx, ty);
								world->dirty[tx + ty * world->width] = true;
							}
						}
					}
				}

				if (upd) {
					GPU_FreeImage(cur->texture);
					cur->texture = GPU_CopyImageFromSurface(cur->surface);
					GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
					//world->updateRigidBodyHitbox(cur);
					cur->needsUpdate = true;
				}
			}

			world->b2world->DestroyBody(cur->body);
		}
		world->rigidBodies.clear();
	}

	if (Controls::DEBUG_UPDATE_WORLD_MESH->get()) {
		world->updateWorldMesh();
	}

	if (Controls::DEBUG_EXPLODE->get()) {
		int x = (int)((mx - ofsX - camX) / scale);
		int y = (int)((my - ofsY - camY) / scale);
		world->explosion(x, y, 30);
	}

	if (Controls::DEBUG_CARVE->get()) {
		// carve square
#pragma region
		int x = (int)((mx - ofsX - camX) / scale - 16);
		int y = (int)((my - ofsY - camY) / scale - 16);

		SDL_Surface* tex = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, SDL_PIXELFORMAT_ARGB8888);

		int n = 0;
		for (int xx = 0; xx < 32; xx++) {
			for (int yy = 0; yy < 32; yy++) {

				if (world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SOLID) {
					PIXEL(tex, xx, yy) = world->tiles[(x + xx) + (y + yy) * world->width].color;
					world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::NOTHING;
					world->dirty[(x + xx) + (y + yy) * world->width] = true;
					n++;
				}
			}
		}
		if (n > 0) {
			b2PolygonShape s;
			s.SetAsBox(1, 1);
			RigidBody* rb = world->makeRigidBody(b2_dynamicBody, (float)x, (float)y, 0, s, 1, (float)0.3, tex);
			for (int tx = 0; tx < tex->w; tx++) {
				b2Filter bf = {};
				bf.categoryBits = 0x0002;
				bf.maskBits = 0x0001;
				rb->body->GetFixtureList()[0].SetFilterData(bf);
			}
			world->rigidBodies.push_back(rb);
			world->updateRigidBodyHitbox(rb);

			world->updateWorldMesh();
		}
#pragma endregion
	}

	if (Controls::DEBUG_BRUSHSIZE_INC->get()) {
		DebugDrawUI::brushSize = DebugDrawUI::brushSize < 50 ? DebugDrawUI::brushSize + 1 : DebugDrawUI::brushSize;
	}

	if (Controls::DEBUG_BRUSHSIZE_DEC->get()) {
		DebugDrawUI::brushSize = DebugDrawUI::brushSize > 1 ? DebugDrawUI::brushSize - 1 : DebugDrawUI::brushSize;
	}

	if (Controls::DEBUG_TOGGLE_PLAYER->get()) {
		if (world->player) {
			freeCamX = world->player->x + world->player->hw / 2.0f;
			freeCamY = world->player->y - world->player->hh / 2.0f;
			world->entities.erase(std::remove(world->entities.begin(), world->entities.end(), world->player), world->entities.end());
			world->b2world->DestroyBody(world->player->rb->body);
			delete world->player;
			world->player = nullptr;
		}
		else {
			Player* e = new Player();
			e->x = -world->loadZone.x + world->tickZone.x + world->tickZone.w / 2.0f;
			e->y = -world->loadZone.y + world->tickZone.y + world->tickZone.h / 2.0f;
			e->vx = 0;
			e->vy = 0;
			e->hw = 10;
			e->hh = 20;
			b2PolygonShape sh;
			sh.SetAsBox(e->hw / 2.0f + 1, e->hh / 2.0f);
			e->rb = world->makeRigidBody(b2BodyType::b2_kinematicBody, e->x + e->hw / 2.0f - 0.5, e->y + e->hh / 2.0f - 0.5, 0, sh, 1, 1, NULL);
			e->rb->body->SetGravityScale(0);
			e->rb->body->SetLinearDamping(0);
			e->rb->body->SetAngularDamping(0);

			Item* i3 = new Item();
			i3->setFlag(ItemFlags::VACUUM);
			i3->vacuumParticles = {};
			i3->surface = Textures::loadTexture("res/objects/testVacuum.png");
			i3->texture = GPU_CopyImageFromSurface(i3->surface);
			GPU_SetImageFilter(i3->texture, GPU_FILTER_NEAREST);
			i3->pivotX = 6;
			e->setItemInHand(i3, world);

			b2Filter bf = {};
			bf.categoryBits = 0x0001;
			//bf.maskBits = 0x0000;
			e->rb->body->GetFixtureList()[0].SetFilterData(bf);

			world->entities.push_back(e);
			world->player = e;


		}
	}


	if (false) {

	}
	else {
		//audioEngine.SetEventParameter("event:/World/Sand", "Sand", 0);
		if (world->player && world->player->heldItem != NULL && world->player->heldItem->getFlag(ItemFlags::FLUID_CONTAINER)) {
			if (Controls::lmouse && world->player->heldItem->carry.size() > 0) {
				// shoot fluid from container
#pragma region
				int x = (int)(world->player->x + world->player->hw / 2.0f + world->loadZone.x + 10 * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f));
				int y = (int)(world->player->y + world->player->hh / 2.0f + world->loadZone.y + 10 * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f));

				MaterialInstance mat = world->player->heldItem->carry[world->player->heldItem->carry.size() - 1];
				world->player->heldItem->carry.pop_back();
				world->addParticle(new Particle(mat, (float)x, (float)y, (float)(world->player->vx / 2 + (rand() % 10 - 5) / 10.0f + 1.5f * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f)), (float)(world->player->vy / 2 + -(rand() % 5 + 5) / 10.0f + 1.5f * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f)), 0, (float)0.1));

				int i = (int)world->player->heldItem->carry.size();
				i = (int)((i / (float)world->player->heldItem->capacity) * world->player->heldItem->fill.size());
				UInt16Point pt = world->player->heldItem->fill[i];
				PIXEL(world->player->heldItem->surface, pt.x, pt.y) = 0x00;

				world->player->heldItem->texture = GPU_CopyImageFromSurface(world->player->heldItem->surface);
				GPU_SetImageFilter(world->player->heldItem->texture, GPU_FILTER_NEAREST);

				//audioEngine.SetEventParameter("event:/World/Sand", "Sand", 1);
#pragma endregion
			}
			else {
				// pick up fluid into container
#pragma region
				float breakSize = world->player->heldItem->breakSize;

				int x = (int)(world->player->x + world->player->hw / 2.0f + world->loadZone.x + 10 * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f) - breakSize / 2);
				int y = (int)(world->player->y + world->player->hh / 2.0f + world->loadZone.y + 10 * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f) - breakSize / 2);

				int n = 0;
				for (int xx = 0; xx < breakSize; xx++) {
					for (int yy = 0; yy < breakSize; yy++) {
						if (world->player->heldItem->capacity == 0 || (world->player->heldItem->carry.size() < world->player->heldItem->capacity)) {
							float cx = (float)((xx / breakSize) - 0.5);
							float cy = (float)((yy / breakSize) - 0.5);

							if (cx * cx + cy * cy > 0.25f) continue;

							if (world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SAND || world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SOUP) {
								world->player->heldItem->carry.push_back(world->tiles[(x + xx) + (y + yy) * world->width]);

								int i = (int)world->player->heldItem->carry.size() - 1;
								i = (int)((i / (float)world->player->heldItem->capacity) * world->player->heldItem->fill.size());
								UInt16Point pt = world->player->heldItem->fill[i];
								Uint32 c = world->tiles[(x + xx) + (y + yy) * world->width].color;
								PIXEL(world->player->heldItem->surface, pt.x, pt.y) = (world->tiles[(x + xx) + (y + yy) * world->width].mat->alpha << 24) + c;

								world->player->heldItem->texture = GPU_CopyImageFromSurface(world->player->heldItem->surface);
								GPU_SetImageFilter(world->player->heldItem->texture, GPU_FILTER_NEAREST);

								world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::NOTHING;
								world->dirty[(x + xx) + (y + yy) * world->width] = true;
								n++;
							}
						}
					}
				}

				if (n > 0) {
					//audioEngine.PlayEvent("event:/Player/Impact");
				}
#pragma endregion
			}
		}

		// rigidbody hover
#pragma region

		int x = (int)((mx - ofsX - camX) / scale);
		int y = (int)((my - ofsY - camY) / scale);

		bool swapped = false;
		float hoverDelta = 10.0 * m_dt / 1000.0;

		// this copies the vector
		std::vector<RigidBody*> rbs = world->rigidBodies;
		for (size_t i = 0; i < rbs.size(); i++) {
			RigidBody* cur = rbs[i];

			if (swapped) {
				cur->hover = (float)std::fmax(0, cur->hover - hoverDelta);
				continue;
			}

			bool connect = false;
			if (cur->body->IsEnabled()) {
				float s = sin(-cur->body->GetAngle());
				float c = cos(-cur->body->GetAngle());
				bool upd = false;
				for (float xx = -3; xx <= 3; xx += 0.5) {
					for (float yy = -3; yy <= 3; yy += 0.5) {
						if (abs(xx) + abs(yy) == 6) continue;
						// rotate point

						float tx = x + xx - cur->body->GetPosition().x;
						float ty = y + yy - cur->body->GetPosition().y;

						int ntx = (int)(tx * c - ty * s);
						int nty = (int)(tx * s + ty * c);

						if (ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
							if (((PIXEL(cur->surface, ntx, nty) >> 24) & 0xff) != 0x00) {
								connect = true;
							}
						}
					}
				}
			}

			if (connect) {
				swapped = true;
				cur->hover = (float)std::fmin(1, cur->hover + hoverDelta);
			}
			else {
				cur->hover = (float)std::fmax(0, cur->hover - hoverDelta);
			}
		}
#pragma endregion

		// update world->tickZone
#pragma region
		world->tickZone = { CHUNK_W, CHUNK_H, world->width - CHUNK_W * 2, world->height - CHUNK_H * 2 };
		if (world->tickZone.x + world->tickZone.w > world->width) {
			world->tickZone.x = world->width - world->tickZone.w;
		}

		if (world->tickZone.y + world->tickZone.h > world->height) {
			world->tickZone.y = world->height - world->tickZone.h;
		}
#pragma endregion

	}
}

void Game::tickChunkLoading() {
	
	// if need to load chunks
	if ((abs(accLoadX) > CHUNK_W / 2 || abs(accLoadY) > CHUNK_H / 2)) {
		while (world->toLoad.size() > 0) {
			// tick chunkloading
			world->frame();
		}

		//iterate
#pragma region

		for (int i = 0; i < world->width * world->height; i++) {
			const unsigned int offset = i * 4;

#define UCH_SET_PIXEL(pix_ar, ofs, c_r, c_g, c_b, c_a) \
				pix_ar[ofs + 0] = c_b;\
				pix_ar[ofs + 1] = c_g;\
				pix_ar[ofs + 2] = c_r;\
				pix_ar[ofs + 3] = c_a;

			if (world->dirty[i]) {
				if (world->tiles[i].mat->physicsType == PhysicsType::AIR) {
					UCH_SET_PIXEL(pixels_ar, offset, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
				}
				else {
					Uint32 color = world->tiles[i].color;
					Uint32 emit = world->tiles[i].mat->emitColor;
					UCH_SET_PIXEL(pixels_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, world->tiles[i].mat->alpha);
					UCH_SET_PIXEL(pixelsEmission_ar, offset, (emit >> 0) & 0xff, (emit >> 8) & 0xff, (emit >> 16) & 0xff, (emit >> 24) & 0xff);
				}
			}

			if (world->layer2Dirty[i]) {
				if (world->layer2[i].mat->physicsType == PhysicsType::AIR) {
					if (Settings::draw_background_grid) {
						Uint32 color = ((i) % 2) == 0 ? 0x888888 : 0x444444;
						UCH_SET_PIXEL(pixelsLayer2_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, SDL_ALPHA_OPAQUE);
					}
					else {
						UCH_SET_PIXEL(pixelsLayer2_ar, offset, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
					}
					continue;
				}
				Uint32 color = world->layer2[i].color;
				UCH_SET_PIXEL(pixelsLayer2_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, world->layer2[i].mat->alpha);
			}

			if (world->backgroundDirty[i]) {
				Uint32 color = world->background[i];
				UCH_SET_PIXEL(pixelsBackground_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, (color >> 24) & 0xff);
			}
#undef UCH_SET_PIXEL
		}
#pragma endregion

		memset(world->dirty, false, (size_t)world->width * world->height);
		memset(world->layer2Dirty, false, (size_t)world->width * world->height);
		memset(world->backgroundDirty, false, (size_t)world->width * world->height);


		while ((abs(accLoadX) > CHUNK_W / 2 || abs(accLoadY) > CHUNK_H / 2)) {
			int subX = std::fmax(std::fmin(accLoadX, CHUNK_W / 2), -CHUNK_W / 2);
			if (abs(subX) < CHUNK_W / 2) subX = 0;
			int subY = std::fmax(std::fmin(accLoadY, CHUNK_H / 2), -CHUNK_H / 2);
			if (abs(subY) < CHUNK_H / 2) subY = 0;

			world->loadZone.x += subX;
			world->loadZone.y += subY;

			int delta = 4 * (subX + subY * world->width);

			std::vector<std::future<void>> results = {};
			if (delta > 0) {
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixels_ar[0]), &(pixels_ar[world->width * world->height * 4]) - delta, &(pixels_ar[world->width * world->height * 4]));
					//rotate(pixels.begin(), pixels.end() - delta, pixels.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsLayer2_ar[0]), &(pixelsLayer2_ar[world->width * world->height * 4]) - delta, &(pixelsLayer2_ar[world->width * world->height * 4]));
					//rotate(pixelsLayer2.begin(), pixelsLayer2.end() - delta, pixelsLayer2.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsBackground_ar[0]), &(pixelsBackground_ar[world->width * world->height * 4]) - delta, &(pixelsBackground_ar[world->width * world->height * 4]));
					//rotate(pixelsBackground.begin(), pixelsBackground.end() - delta, pixelsBackground.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsFire_ar[0]), &(pixelsFire_ar[world->width * world->height * 4]) - delta, &(pixelsFire_ar[world->width * world->height * 4]));
					//rotate(pixelsFire_ar.begin(), pixelsFire_ar.end() - delta, pixelsFire_ar.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsFlow_ar[0]), &(pixelsFlow_ar[world->width * world->height * 4]) - delta, &(pixelsFlow_ar[world->width * world->height * 4]));
					//rotate(pixelsFlow_ar.begin(), pixelsFlow_ar.end() - delta, pixelsFlow_ar.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsEmission_ar[0]), &(pixelsEmission_ar[world->width * world->height * 4]) - delta, &(pixelsEmission_ar[world->width * world->height * 4]));
					//rotate(pixelsEmission_ar.begin(), pixelsEmission_ar.end() - delta, pixelsEmission_ar.end());
				}));
			}
			else if (delta < 0) {
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixels_ar[0]), &(pixels_ar[0]) - delta, &(pixels_ar[world->width * world->height * 4]));
					//rotate(pixels.begin(), pixels.begin() - delta, pixels.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsLayer2_ar[0]), &(pixelsLayer2_ar[0]) - delta, &(pixelsLayer2_ar[world->width * world->height * 4]));
					//rotate(pixelsLayer2.begin(), pixelsLayer2.begin() - delta, pixelsLayer2.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsBackground_ar[0]), &(pixelsBackground_ar[0]) - delta, &(pixelsBackground_ar[world->width * world->height * 4]));
					//rotate(pixelsBackground.begin(), pixelsBackground.begin() - delta, pixelsBackground.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsFire_ar[0]), &(pixelsFire_ar[0]) - delta, &(pixelsFire_ar[world->width * world->height * 4]));
					//rotate(pixelsFire_ar.begin(), pixelsFire_ar.begin() - delta, pixelsFire_ar.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsFlow_ar[0]), &(pixelsFlow_ar[0]) - delta, &(pixelsFlow_ar[world->width * world->height * 4]));
					//rotate(pixelsFlow_ar.begin(), pixelsFlow_ar.begin() - delta, pixelsFlow_ar.end());
				}));
				results.push_back(updateDirtyPool->push([&](int id) {
					std::rotate(&(pixelsEmission_ar[0]), &(pixelsEmission_ar[0]) - delta, &(pixelsEmission_ar[world->width * world->height * 4]));
					//rotate(pixelsEmission_ar.begin(), pixelsEmission_ar.begin() - delta, pixelsEmission_ar.end());
				}));
			}

			for (auto& v : results) {
				v.get();
			}


			#define CLEARPIXEL(pixels, ofs) \
			pixels[ofs + 0] = pixels[ofs + 1] = pixels[ofs + 2] = 0xff; \
			pixels[ofs + 3] = SDL_ALPHA_TRANSPARENT;

			for (int x = 0; x < abs(subX); x++) {
				for (int y = 0; y < world->height; y++) {
					const unsigned int offset = (world->width * 4 * y) + x * 4;
					if (offset < world->width * world->height * 4) {
						CLEARPIXEL(pixels_ar, offset);
						CLEARPIXEL(pixelsLayer2_ar, offset);
						CLEARPIXEL(pixelsObjects_ar, offset);
						CLEARPIXEL(pixelsBackground_ar, offset);
						CLEARPIXEL(pixelsFire_ar, offset);
						CLEARPIXEL(pixelsFlow_ar, offset);
						CLEARPIXEL(pixelsEmission_ar, offset);
					}
				}
			}

			for (int y = 0; y < abs(subY); y++) {
				for (int x = 0; x < world->width; x++) {
					const unsigned int offset = (world->width * 4 * y) + x * 4;
					if (offset < world->width * world->height * 4) {
						CLEARPIXEL(pixels_ar, offset);
						CLEARPIXEL(pixelsLayer2_ar, offset);
						CLEARPIXEL(pixelsObjects_ar, offset);
						CLEARPIXEL(pixelsBackground_ar, offset);
						CLEARPIXEL(pixelsFire_ar, offset);
						CLEARPIXEL(pixelsFlow_ar, offset);
						CLEARPIXEL(pixelsEmission_ar, offset);
					}
				}
			}

#undef CLEARPIXEL

			accLoadX -= subX;
			accLoadY -= subY;

			ofsX -= subX * scale;
			ofsY -= subY * scale;
		}

		world->tickChunks();
		world->updateWorldMesh();
		world->dirty[0] = true;
		world->layer2Dirty[0] = true;
		world->backgroundDirty[0] = true;

	}
	else {
		world->frame();
	}
}

void Game::tick() {
	int lastReadyToMergeSize = (int)world->readyToMerge.size();

	// check chunk loading
	tickChunkLoading();

	if (world->needToTickGeneration) world->tickChunkGeneration();

	// clear objects
	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
	GPU_Clear(textureObjects->target);

	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
	GPU_Clear(textureObjectsLQ->target);

	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
	GPU_Clear(textureObjectsBack->target);

	if (Settings::tick_world && world->readyToMerge.size() == 0) {
		world->tickChunks();
	}

	// render objects
#pragma region

	memset(objectDelete, false, (size_t)world->width * world->height * sizeof(bool));


	GPU_SetBlendMode(textureObjects, GPU_BLEND_NORMAL);
	GPU_SetBlendMode(textureObjectsLQ, GPU_BLEND_NORMAL);
	GPU_SetBlendMode(textureObjectsBack, GPU_BLEND_NORMAL);

	for (size_t i = 0; i < world->rigidBodies.size(); i++) {
		RigidBody* cur = world->rigidBodies[i];
		if (cur == nullptr) continue;
		if (cur->surface == nullptr) continue;
		if (!cur->body->IsEnabled()) continue;

		float x = cur->body->GetPosition().x;
		float y = cur->body->GetPosition().y;

		// draw
#pragma region
		GPU_Target* tgt = cur->back ? textureObjectsBack->target : textureObjects->target;
		GPU_Target* tgtLQ = cur->back ? textureObjectsBack->target : textureObjectsLQ->target;
		int scaleObjTex = Settings::hd_objects ? Settings::hd_objects_size : 1;

		GPU_Rect r = { x * scaleObjTex, y * scaleObjTex, (float)cur->surface->w * scaleObjTex, (float)cur->surface->h * scaleObjTex };

		if (cur->texNeedsUpdate) {
			if (cur->texture != nullptr) {
				GPU_FreeImage(cur->texture);
			}
			cur->texture = GPU_CopyImageFromSurface(cur->surface);
			GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
			cur->texNeedsUpdate = false;
		}

		GPU_BlitRectX(cur->texture, NULL, tgt, &r, cur->body->GetAngle() * 180 / (float)PI, 0, 0, GPU_FLIP_NONE);
#pragma endregion

		// draw outline
		/*#pragma region
		Uint8 outlineAlpha = (Uint8)(cur->hover * 255);
		if(outlineAlpha > 0) {
		SDL_Color col = {0xff, 0xff, 0x80, outlineAlpha};
		GPU_SetShapeBlendMode(GPU_BLEND_NORMAL_FACTOR_ALPHA); // SDL_BLENDMODE_BLEND
		for(auto& l : cur->outline) {
		b2Vec2* vec = new b2Vec2[l.GetNumPoints()];
		for(int j = 0; j < l.GetNumPoints(); j++) {
		vec[j] = {(float)l.GetPoint(j).x / scale, (float)l.GetPoint(j).y / scale};
		}
		Drawing::drawPolygon(tgtLQ, col, vec, (int)x, (int)y, scale, l.GetNumPoints(), cur->body->GetAngle(), 0, 0);
		delete[] vec;
		}
		GPU_SetShapeBlendMode(GPU_BLEND_NORMAL); // SDL_BLENDMODE_NONE
		}
		#pragma endregion*/

		// displace fluids
#pragma region
		float s = sin(cur->body->GetAngle());
		float c = cos(cur->body->GetAngle());

		std::vector<std::pair<int, int>> checkDirs = { { 0, 0 },{ 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 } };

		for (int tx = 0; tx < cur->matWidth; tx++) {
			for (int ty = 0; ty < cur->matHeight; ty++) {
				MaterialInstance rmat = cur->tiles[tx + ty * cur->matWidth];
				if (rmat.mat->id == Materials::GENERIC_AIR.id) continue;

				// rotate point
				int wx = (int)(tx * c - (ty + 1) * s + x);
				int wy = (int)(tx * s + (ty + 1) * c + y);

				for (auto& dir : checkDirs) {
					int wxd = wx + dir.first;
					int wyd = wy + dir.second;

					if (wxd < 0 || wyd < 0 || wxd >= world->width || wyd >= world->height) continue;
					if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::AIR) {
						world->tiles[wxd + wyd * world->width] = rmat;
						world->dirty[wxd + wyd * world->width] = true;
						//objectDelete[wxd + wyd * world->width] = true;
						break;
					}
					else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SAND) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						//objectDelete[wxd + wyd * world->width] = true;
						world->dirty[wxd + wyd * world->width] = true;
						cur->body->SetLinearVelocity({ cur->body->GetLinearVelocity().x * (float)0.99, cur->body->GetLinearVelocity().y * (float)0.99 });
						cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.98);
						break;
					}
					else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SOUP) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						//objectDelete[wxd + wyd * world->width] = true;
						world->dirty[wxd + wyd * world->width] = true;
						cur->body->SetLinearVelocity({ cur->body->GetLinearVelocity().x * (float)0.998, cur->body->GetLinearVelocity().y * (float)0.998 });
						cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.99);
						break;
					}
				}
			}
		}
#pragma endregion
	}
#pragma endregion

	// render entities
#pragma region
	if (lastReadyToMergeSize == 0) {
		world->tickEntities(textureEntities->target);

		if (world->player) {
			if (world->player->holdHammer) {
				int x = (int)((mx - ofsX - camX) / scale);
				int y = (int)((my - ofsY - camY) / scale);
				GPU_Line(textureEntitiesLQ->target, x, y, world->player->hammerX, world->player->hammerY, { 0xff, 0xff, 0x00, 0xff });
			}
		}
	}
	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL); // SDL_BLENDMODE_NONE
#pragma endregion

	// entity fluid displacement & make solid
#pragma region
	for (size_t i = 0; i < world->entities.size(); i++) {
		Entity cur = *world->entities[i];

		for (int tx = 0; tx < cur.hw; tx++) {
			for (int ty = 0; ty < cur.hh; ty++) {

				int wx = (int)(tx + cur.x + world->loadZone.x);
				int wy = (int)(ty + cur.y + world->loadZone.y);
				if (wx < 0 || wy < 0 || wx >= world->width || wy >= world->height) continue;
				if (world->tiles[wx + wy * world->width].mat->physicsType == PhysicsType::AIR) {
					world->tiles[wx + wy * world->width] = Tiles::OBJECT;
					objectDelete[wx + wy * world->width] = true;
				}
				else if (world->tiles[wx + wy * world->width].mat->physicsType == PhysicsType::SAND || world->tiles[wx + wy * world->width].mat->physicsType == PhysicsType::SOUP) {
					world->addParticle(new Particle(world->tiles[wx + wy * world->width], (float)(wx + rand() % 3 - 1 - cur.vx), (float)(wy - abs(cur.vy)), (float)(-cur.vx / 4 + (rand() % 10 - 5) / 5.0f), (float)(-cur.vy / 4 + -(rand() % 5 + 5) / 5.0f), 0, (float)0.1));
					world->tiles[wx + wy * world->width] = Tiles::OBJECT;
					objectDelete[wx + wy * world->width] = true;
					world->dirty[wx + wy * world->width] = true;
				}
			}
		}
	}
#pragma endregion

	if (Settings::tick_world && world->readyToMerge.size() == 0) {
		world->tick();
	}

	if (Controls::DEBUG_TICK->get()) {
		world->tick();
	}

	// player movement
	tickPlayer();

	// update particles, tickObjects, update dirty
	// TODO: this is not entirely thread safe since tickParticles changes World::tiles and World::dirty
#pragma region

	bool hadDirty = false;
	bool hadLayer2Dirty = false;
	bool hadBackgroundDirty = false;
	bool hadFire = false;
	bool hadFlow = false;

	int pitch;
	//void* vdpixels_ar = texture->data;
	//unsigned char* dpixels_ar = (unsigned char*)vdpixels_ar;
	unsigned char* dpixels_ar = pixels_ar;
	unsigned char* dpixelsFire_ar = pixelsFire_ar;
	unsigned char* dpixelsFlow_ar = pixelsFlow_ar;
	unsigned char* dpixelsEmission_ar = pixelsEmission_ar;

	std::vector<std::future<void>> results = {};

	results.push_back(updateDirtyPool->push([&](int id) {
		//SDL_SetRenderTarget(renderer, textureParticles);
		void* particlePixels = pixelsParticles_ar;
		memset(particlePixels, 0, (size_t)world->width * world->height * 4);
		world->renderParticles((unsigned char**)&particlePixels);
		world->tickParticles();
		//SDL_SetRenderTarget(renderer, NULL);
	}));

	if (world->readyToMerge.size() == 0) {
		results.push_back(updateDirtyPool->push([&](int id) {
			world->tickObjectBounds();
		}));
	}

	for (int i = 0; i < results.size(); i++) {
		results[i].get();
	}

	for (size_t i = 0; i < world->rigidBodies.size(); i++) {
		RigidBody* cur = world->rigidBodies[i];
		if (cur == nullptr) continue;
		if (cur->surface == nullptr) continue;
		if (!cur->body->IsEnabled()) continue;

		float x = cur->body->GetPosition().x;
		float y = cur->body->GetPosition().y;

		float s = sin(cur->body->GetAngle());
		float c = cos(cur->body->GetAngle());

		std::vector<std::pair<int, int>> checkDirs = { { 0, 0 },{ 1, 0 },{ -1, 0 },{ 0, 1 },{ 0, -1 } };
		for (int tx = 0; tx < cur->matWidth; tx++) {
			for (int ty = 0; ty < cur->matHeight; ty++) {
				MaterialInstance rmat = cur->tiles[tx + ty * cur->matWidth];
				if (rmat.mat->id == Materials::GENERIC_AIR.id) continue;

				// rotate point
				int wx = (int)(tx * c - (ty + 1) * s + x);
				int wy = (int)(tx * s + (ty + 1) * c + y);

				bool found = false;
				for (auto& dir : checkDirs) {
					int wxd = wx + dir.first;
					int wyd = wy + dir.second;

					if (wxd < 0 || wyd < 0 || wxd >= world->width || wyd >= world->height) continue;
					if (world->tiles[wxd + wyd * world->width] == rmat) {
						cur->tiles[tx + ty * cur->matWidth] = world->tiles[wxd + wyd * world->width];
						world->tiles[wxd + wyd * world->width] = Tiles::NOTHING;
						world->dirty[wxd + wyd * world->width] = true;
						found = true;

						//for(int dxx = -1; dxx <= 1; dxx++) {
						//    for(int dyy = -1; dyy <= 1; dyy++) {
						//        if(world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].mat->physicsType == PhysicsType::SAND || world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].mat->physicsType == PhysicsType::SOUP) {
						//            uint32_t color = world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].color;

						//            unsigned int offset = ((wxd + dxx) + (wyd + dyy) * world->width) * 4;

						//            dpixels_ar[offset + 2] = ((color >> 0) & 0xff);        // b
						//            dpixels_ar[offset + 1] = ((color >> 8) & 0xff);        // g
						//            dpixels_ar[offset + 0] = ((color >> 16) & 0xff);        // r
						//            dpixels_ar[offset + 3] = world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].mat->alpha;    // a
						//        }
						//    }
						//}

						//if(!Settings::draw_load_zones) {
						//    unsigned int offset = (wxd + wyd * world->width) * 4;
						//    dpixels_ar[offset + 2] = 0;        // b
						//    dpixels_ar[offset + 1] = 0;        // g
						//    dpixels_ar[offset + 0] = 0xff;        // r
						//    dpixels_ar[offset + 3] = 0xff;    // a
						//}


						break;
					}
				}

				if (!found) {
					if (world->tiles[wx + wy * world->width].mat->id == Materials::GENERIC_AIR.id) {
						cur->tiles[tx + ty * cur->matWidth] = Tiles::NOTHING;
					}
				}
			}
		}

		for (int tx = 0; tx < cur->surface->w; tx++) {
			for (int ty = 0; ty < cur->surface->h; ty++) {
				MaterialInstance mat = cur->tiles[tx + ty * cur->surface->w];
				if (mat.mat->id == Materials::GENERIC_AIR.id) {
					PIXEL(cur->surface, tx, ty) = 0x00000000;
				}
				else {
					PIXEL(cur->surface, tx, ty) = (mat.mat->alpha << 24) + (mat.color & 0x00ffffff);
				}
			}
		}

		GPU_FreeImage(cur->texture);
		cur->texture = GPU_CopyImageFromSurface(cur->surface);
		GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);

		cur->needsUpdate = true;
	}

	if (world->readyToMerge.size() == 0) {
		if (Settings::tick_box2d) world->tickObjects();
	}

	if (tickTime % 10 == 0) world->tickObjectsMesh();

	for (int i = 0; i < Materials::nMaterials; i++) movingTiles[i] = 0;

	results.clear();
	results.push_back(updateDirtyPool->push([&](int id) {
		for (int i = 0; i < world->width * world->height; i++) {
			const unsigned int offset = i * 4;

			if (world->dirty[i]) {
				hadDirty = true;
				movingTiles[world->tiles[i].mat->id]++;
				if (world->tiles[i].mat->physicsType == PhysicsType::AIR) {
					dpixels_ar[offset + 0] = 0;        // b
					dpixels_ar[offset + 1] = 0;        // g
					dpixels_ar[offset + 2] = 0;        // r
					dpixels_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a		

					dpixelsFire_ar[offset + 0] = 0;        // b
					dpixelsFire_ar[offset + 1] = 0;        // g
					dpixelsFire_ar[offset + 2] = 0;        // r
					dpixelsFire_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a

					dpixelsEmission_ar[offset + 0] = 0;        // b
					dpixelsEmission_ar[offset + 1] = 0;        // g
					dpixelsEmission_ar[offset + 2] = 0;        // r
					dpixelsEmission_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a

					world->flowY[i] = 0;
					world->flowX[i] = 0;
				}
				else {
					Uint32 color = world->tiles[i].color;
					Uint32 emit = world->tiles[i].mat->emitColor;
					//float br = world->light[i];
					dpixels_ar[offset + 2] = ((color >> 0) & 0xff);        // b
					dpixels_ar[offset + 1] = ((color >> 8) & 0xff);        // g
					dpixels_ar[offset + 0] = ((color >> 16) & 0xff);        // r
					dpixels_ar[offset + 3] = world->tiles[i].mat->alpha;    // a

					dpixelsEmission_ar[offset + 2] = ((emit >> 0) & 0xff);        // b
					dpixelsEmission_ar[offset + 1] = ((emit >> 8) & 0xff);        // g
					dpixelsEmission_ar[offset + 0] = ((emit >> 16) & 0xff);        // r
					dpixelsEmission_ar[offset + 3] = ((emit >> 24) & 0xff);    // a

					if (world->tiles[i].mat->id == Materials::FIRE.id) {
						dpixelsFire_ar[offset + 2] = ((color >> 0) & 0xff);        // b
						dpixelsFire_ar[offset + 1] = ((color >> 8) & 0xff);        // g
						dpixelsFire_ar[offset + 0] = ((color >> 16) & 0xff);        // r
						dpixelsFire_ar[offset + 3] = world->tiles[i].mat->alpha;    // a
						hadFire = true;
					}
					if (world->tiles[i].mat->physicsType == PhysicsType::SOUP) {

						float newFlowX = world->prevFlowX[i] + (world->flowX[i] - world->prevFlowX[i]) * 0.25;
						float newFlowY = world->prevFlowY[i] + (world->flowY[i] - world->prevFlowY[i]) * 0.25;
						if (newFlowY < 0) newFlowY *= 0.5;

						dpixelsFlow_ar[offset + 2] = 0; // b
						dpixelsFlow_ar[offset + 1] = std::min(std::max(newFlowY * (3.0 / world->tiles[i].mat->iterations + 0.5) / 4.0 + 0.5, 0.0), 1.0) * 255; // g
						dpixelsFlow_ar[offset + 0] = std::min(std::max(newFlowX * (3.0 / world->tiles[i].mat->iterations + 0.5) / 4.0 + 0.5, 0.0), 1.0) * 255; // r
						dpixelsFlow_ar[offset + 3] = 0xff; // a
						hadFlow = true;
						world->prevFlowX[i] = newFlowX;
						world->prevFlowY[i] = newFlowY;
						world->flowY[i] = 0;
						world->flowX[i] = 0;
					}
					else {
						world->flowY[i] = 0;
						world->flowX[i] = 0;
					}
				}
			}
		}
	}));

	//void* vdpixelsLayer2_ar = textureLayer2->data;
	//unsigned char* dpixelsLayer2_ar = (unsigned char*)vdpixelsLayer2_ar;
	unsigned char* dpixelsLayer2_ar = pixelsLayer2_ar;
	results.push_back(updateDirtyPool->push([&](int id) {
		for (int i = 0; i < world->width * world->height; i++) {
			//const unsigned int i = x + y * world->width;
			const unsigned int offset = i * 4;
			if (world->layer2Dirty[i]) {
				hadLayer2Dirty = true;
				if (world->layer2[i].mat->physicsType == PhysicsType::AIR) {
					if (Settings::draw_background_grid) {
						Uint32 color = ((i) % 2) == 0 ? 0x888888 : 0x444444;
						dpixelsLayer2_ar[offset + 2] = (color >> 0) & 0xff;        // b
						dpixelsLayer2_ar[offset + 1] = (color >> 8) & 0xff;        // g
						dpixelsLayer2_ar[offset + 0] = (color >> 16) & 0xff;       // r
						dpixelsLayer2_ar[offset + 3] = SDL_ALPHA_OPAQUE;			 // a
						continue;
					}
					else {
						dpixelsLayer2_ar[offset + 0] = 0;        // b
						dpixelsLayer2_ar[offset + 1] = 0;        // g
						dpixelsLayer2_ar[offset + 2] = 0;        // r
						dpixelsLayer2_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a
						continue;
					}
				}
				Uint32 color = world->layer2[i].color;
				dpixelsLayer2_ar[offset + 2] = (color >> 0) & 0xff;        // b
				dpixelsLayer2_ar[offset + 1] = (color >> 8) & 0xff;        // g
				dpixelsLayer2_ar[offset + 0] = (color >> 16) & 0xff;        // r
				dpixelsLayer2_ar[offset + 3] = world->layer2[i].mat->alpha;    // a
			}
		}
	}));

	//void* vdpixelsBackground_ar = textureBackground->data;
	//unsigned char* dpixelsBackground_ar = (unsigned char*)vdpixelsBackground_ar;
	unsigned char* dpixelsBackground_ar = pixelsBackground_ar;
	results.push_back(updateDirtyPool->push([&](int id) {
		for (int i = 0; i < world->width * world->height; i++) {
			//const unsigned int i = x + y * world->width;
			const unsigned int offset = i * 4;

			if (world->backgroundDirty[i]) {
				hadBackgroundDirty = true;
				Uint32 color = world->background[i];
				dpixelsBackground_ar[offset + 2] = (color >> 0) & 0xff;        // b
				dpixelsBackground_ar[offset + 1] = (color >> 8) & 0xff;        // g
				dpixelsBackground_ar[offset + 0] = (color >> 16) & 0xff;       // r
				dpixelsBackground_ar[offset + 3] = (color >> 24) & 0xff;       // a
			}

		}
	}));

	for (int i = 0; i < world->width * world->height; i++) {
		//const unsigned int i = x + y * world->width;
		const unsigned int offset = i * 4;

		if (objectDelete[i]) {
			world->tiles[i] = Tiles::NOTHING;
		}
	}

	for (int i = 0; i < results.size(); i++) {
		results[i].get();
	}

	//updateMaterialSounds();

	GPU_UpdateImageBytes(
		textureParticles,
		NULL,
		&pixelsParticles_ar[0],
		world->width * 4
	);

	if (hadDirty)		   memset(world->dirty, false, (size_t)world->width * world->height);
	if (hadLayer2Dirty)	   memset(world->layer2Dirty, false, (size_t)world->width * world->height);
	if (hadBackgroundDirty) memset(world->backgroundDirty, false, (size_t)world->width * world->height);

#pragma endregion

	if (Settings::tick_temperature && tickTime % 4 == 2) {
		world->tickTemperature();
	}
	if (Settings::draw_temperature_map && tickTime % 4 == 0) {
		//renderTemperatureMap(world);
	}

	if (hadDirty) {
		GPU_UpdateImageBytes(texture, NULL, &pixels[0], world->width * 4);

		GPU_UpdateImageBytes(
			emissionTexture,
			NULL,
			&pixelsEmission[0],
			world->width * 4
		);
	}

	if (hadLayer2Dirty) {
		GPU_UpdateImageBytes(
			textureLayer2,
			NULL,
			&pixelsLayer2[0],
			world->width * 4
		);
	}

	if (hadBackgroundDirty) {
		GPU_UpdateImageBytes(
			textureBackground,
			NULL,
			&pixelsBackground[0],
			world->width * 4
		);
	}

	if (hadFlow) {
		GPU_UpdateImageBytes(
			textureFlow,
			NULL,
			&pixelsFlow[0],
			world->width * 4
		);

		waterFlowPassShader->dirty = true;
	}

	if (hadFire) {
		GPU_UpdateImageBytes(
			textureFire,
			NULL,
			&pixelsFire[0],
			world->width * 4
		);
	}

	if (Settings::draw_temperature_map) {
		GPU_UpdateImageBytes(
			temperatureMap,
			NULL,
			&pixelsTemp[0],
			world->width * 4
		);
	}

	if (Settings::tick_box2d && tickTime % 4 == 0) world->updateWorldMesh();

}

void Game::tickPlayer() {

}

void Game::loadShaders() {

	if (waterShader) delete waterShader;
	if (waterFlowPassShader) delete waterFlowPassShader;
	if (newLightingShader) delete newLightingShader;
	if (fireShader) delete fireShader;
	if (fire2Shader) delete fire2Shader;

	waterShader = new WaterShader();
	waterFlowPassShader = new WaterFlowPassShader();
	newLightingShader = new NewLightingShader();
	fireShader = new FireShader();
	fire2Shader = new Fire2Shader();
}
