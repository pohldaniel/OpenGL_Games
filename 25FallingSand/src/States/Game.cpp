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
#include "Tiles.hpp"
#include "Shaders.hpp"
#include "OptionsUI.h"
#include "InGameUI.h"
#include "Macros.hpp"

#include <engine/Batchrenderer.h>

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

	m_background1.setLayer(std::vector<BackgroundLayer>{ 
		{ &Globals::textureManager.get("bg_layer_2"), 2, 1.0f, 0.5f, Vector2f(0.333f, 0.666f) },
		{ &Globals::textureManager.get("bg_layer_3"), 2, 0.0f, -0.0666f, Vector2f(0.37f, 0.37f + 0.333f) },
		{ &Globals::textureManager.get("bg_layer_4"), 2, 3.0f, -0.01f, Vector2f(0.333f, 0.666f) },
		{ &Globals::textureManager.get("bg_layer_5"), 2, 0.0f, 0.01333f, Vector2f(0.333f, 0.666f) } });
	m_background1.setSpeed(0.005f);

	m_background2.setLayer(std::vector<BackgroundLayer>{ 
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background2.setSpeed(0.005f);

	init();
		
	m_texture.createEmptyTexture(world->width, world->height, GL_RGBA8, GL_RGBA);
	m_texture.setWrapMode(GL_CLAMP);
	m_texture.setFilter(GL_NEAREST);
	m_pixelbuffer.create(world->height * world->width * 4);


	m_framebuffer.create(Application::Width, Application::Height);
	m_framebuffer.attachTexture2D(AttachmentTex::RGBA);
	m_framebuffer.attachRenderbuffer(AttachmentRB::DEPTH24);
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
	
	updateFrameEarly();
	tick();
	updateFrameLate();

	m_framebuffer.bindWrite();
	glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.0f, 0.0f, 0.0f, 0.0f}.data());
	glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);
	Globals::spritesheetManager.getAssetPointer("objects")->bind(0);
	Batchrenderer::Get().drawBuffer();
	Globals::spritesheetManager.getAssetPointer("objects")->unbind(0);
	m_framebuffer.unbindWrite();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_switch ? m_background1.draw() : m_background2.draw();

	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadVector("u_texRect", Vector4f(zoomX + offsetX, zoomY - offsetY, (1.0f - zoomX) + offsetX, (1.0f - zoomY) - offsetY));
	//shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	m_framebuffer.bindColorTexture();
	Globals::shapeManager.get("quad").drawRaw();

	m_texture.bind();
	Globals::shapeManager.get("quad").drawRaw();

	shader->unuse();
	glDisable(GL_BLEND);


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	MainMenuUI::Draw(this);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void Game::init() {

	bool openDebugUIs = false;
	DebugUI::visible = openDebugUIs;
	DebugCheatsUI::visible = openDebugUIs;
	DebugDrawUI::visible = openDebugUIs;
	Settings::draw_frame_graph = openDebugUIs;
	if (!openDebugUIs) {
		Settings::draw_background = true;
		Settings::draw_background_grid = false;
		Settings::draw_load_zones = false;
		Settings::draw_physics_debug = false;
		Settings::draw_chunk_state = false;
		Settings::draw_debug_stats = false;
		Settings::draw_detailed_material_info = false;
		Settings::draw_temperature_map = false;
	}

	this->gameDir = GameDir("gamedir/");

	ctpl::thread_pool* worldInitThreadPool = new ctpl::thread_pool(1);
	std::future<void> worldInitThread;

	worldInitThread = worldInitThreadPool->push([&](int id) {
		world = new World();
		world->noSaveLoad = true;
		world->init(gameDir.getWorldPath("mainMenu"), (int)ceil(MAX_WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(MAX_HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3);

	});

	srand((unsigned int)Time::millis());

	Materials::init();

	movingTiles = new uint16_t[Materials::nMaterials];

	b2DebugDraw = new b2DebugDraw_impl();
	worldInitThread.get();

	int w = Application::Width;
	int h = Application::Height;

	handleWindowSizeChange(w, h);
	updateDirtyPool = new ctpl::thread_pool(6);

	for (int x = -CHUNK_W * 4; x < world->width + CHUNK_W * 4; x += CHUNK_W) {
		for (int y = -CHUNK_H * 3; y < world->height + CHUNK_H * 8; y += CHUNK_H) {
			world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
		}
	}

	freeCamX = world->width / 2.0f - CHUNK_W / 2;
	freeCamY = world->height / 2.0f - (int)(CHUNK_H * 0.75);
	if (world->player) {
		plPosX = world->player->x;
		plPosY = world->player->y;
	}
	else {
		plPosX = freeCamX;
		plPosY = freeCamY;
	}

	scale = 3;
	ofsX = (int)(-CHUNK_W * 4);
	ofsY = (int)(-CHUNK_H * 2.5);

	ofsX = (ofsX - Application::Width / 2) / 2 * 3 + Application::Width / 2;
	ofsY = (ofsY - Application::Height / 2) / 2 * 3 + Application::Height / 2;


	objectDelete = new bool[world->width * world->height];
}

void Game::handleWindowSizeChange(int newWidth, int newHeight) {

	int prevWidth = Application::Width;
	int prevHeight = Application::Height;

	Application::Width = newWidth;
	Application::Height = newHeight;

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
	pixelsFire = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsFire_ar = &pixelsFire[0];
	pixelsFlow = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsFlow_ar = &pixelsFlow[0];
	pixelsEmission = std::vector<unsigned char>(world->width * world->height * 4, 0);
	pixelsEmission_ar = &pixelsEmission[0];

	accLoadX -= (newWidth - prevWidth) / 2.0f / scale;
	accLoadY -= (newHeight - prevHeight) / 2.0f / scale;

	tickChunkLoading();

	for (int x = 0; x < world->width; x++) {
		for (int y = 0; y < world->height; y++) {
			world->dirty[x + y * world->width] = true;
			world->layer2Dirty[x + y * world->width] = true;
			world->backgroundDirty[x + y * world->width] = true;
		}
	}
}

void Game::updateFrameEarly() {

	world->tickZone = { CHUNK_W, CHUNK_H, world->width - CHUNK_W * 2, world->height - CHUNK_H * 2 };
	if (world->tickZone.x + world->tickZone.w > world->width) {
		world->tickZone.x = world->width - world->tickZone.w;
	}

	if (world->tickZone.y + world->tickZone.h > world->height) {
		world->tickZone.y = world->height - world->tickZone.h;
	}
}

void Game::tick() {

	int lastReadyToMergeSize = (int)world->readyToMerge.size();

	// check chunk loading
	tickChunkLoading();

	if (world->needToTickGeneration) world->tickChunkGeneration();

	if (Settings::tick_world && world->readyToMerge.size() == 0) {
		world->tickChunks();
	}

	// render objects
	memset(objectDelete, false, (size_t)world->width * world->height * sizeof(bool));


	

	for (size_t i = 0; i < world->rigidBodies.size(); i++) {
		RigidBody* cur = world->rigidBodies[i];
		if (cur == nullptr) continue;
		if (cur->surface == nullptr) continue;
		if (!cur->body->IsEnabled()) continue;

		float x = cur->body->GetPosition().x;
		float y = cur->body->GetPosition().y;
		float scaleObjTex = Settings::hd_objects ? Settings::hd_objects_size : 1.5f;

		Batchrenderer::Get().addQuadAA(Vector4f(x + 280.0f, y + 60.0f, static_cast<float>(cur->surface->w) * scaleObjTex, static_cast<float>(cur->surface->h) * scaleObjTex), Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0);
		// displace fluids
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
						break;
					}else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SAND) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						world->dirty[wxd + wyd * world->width] = true;
						break;
					}else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SOUP) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						world->dirty[wxd + wyd * world->width] = true;
						break;
					}
				}
			}
		}
	}


	if (Settings::tick_world && world->readyToMerge.size() == 0) {
		world->tick();
	}

	//if (Controls::DEBUG_TICK->get()) {
	//	world->tick();
	//}

	// update particles, tickObjects, update dirty
	// TODO: this is not entirely thread safe since tickParticles changes World::tiles and World::dirty
	bool hadDirty = false;
	bool hadLayer2Dirty = false;
	bool hadBackgroundDirty = false;
	bool hadFire = false;
	bool hadFlow = false;

	int pitch;
	unsigned char* dpixels_ar = pixels_ar;
	unsigned char* dpixelsFire_ar = pixelsFire_ar;
	unsigned char* dpixelsFlow_ar = pixelsFlow_ar;
	unsigned char* dpixelsEmission_ar = pixelsEmission_ar;

	std::vector<std::future<void>> results = {};

	results.push_back(updateDirtyPool->push([&](int id) {
		void* particlePixels = pixelsParticles_ar;
		memset(particlePixels, 0, (size_t)world->width * world->height * 4);

		world->renderParticles((unsigned char**)&particlePixels);
		world->tickParticles();

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
					dpixels_ar[offset + 0] = 0;  // b
					dpixels_ar[offset + 1] = 0;  // g
					dpixels_ar[offset + 2] = 0;  // r
					dpixels_ar[offset + 3] = 0;  // a		

					dpixelsFire_ar[offset + 0] = 0;  // b
					dpixelsFire_ar[offset + 1] = 0;  // g
					dpixelsFire_ar[offset + 2] = 0;  // r
					dpixelsFire_ar[offset + 3] = 0;  // a

					dpixelsEmission_ar[offset + 0] = 0;  // b
					dpixelsEmission_ar[offset + 1] = 0;  // g
					dpixelsEmission_ar[offset + 2] = 0;  // r
					dpixelsEmission_ar[offset + 3] = 0;  // a

					world->flowY[i] = 0;
					world->flowX[i] = 0;
				}else {

					Uint32 color = world->tiles[i].color;
					Uint32 emit = world->tiles[i].mat->emitColor;

					dpixels_ar[offset + 2] = ((color >> 0) & 0xff);       // b
					dpixels_ar[offset + 1] = ((color >> 8) & 0xff);       // g
					dpixels_ar[offset + 0] = ((color >> 16) & 0xff);      // r
					dpixels_ar[offset + 3] = world->tiles[i].mat->alpha;  // a	
					
				}
			}
		}

	}));

	for (int i = 0; i < results.size(); i++) {
		results[i].get();
	}

	if (hadDirty) memset(world->dirty, false, (size_t)world->width * world->height);
	

	if (hadDirty) {
		m_pixelbuffer.mapData(m_texture, pixels.data());
	}

	if (Settings::tick_box2d && tickTime % 4 == 0) world->updateWorldMesh();
}

void Game::tickChunkLoading() {

	// if need to load chunks
	if ((abs(accLoadX) > CHUNK_W / 2 || abs(accLoadY) > CHUNK_H / 2)) {
		while (world->toLoad.size() > 0) {
			// tick chunkloading
			world->frame();
		}

		for (int i = 0; i < world->width * world->height; i++) {
			const unsigned int offset = i * 4;

			if (world->dirty[i]) {
				if (world->tiles[i].mat->physicsType == PhysicsType::AIR) {
					UCH_SET_PIXEL(pixels_ar, offset, 128, 0, 0, 0);
				}
				else {
					Uint32 color = world->tiles[i].color;
					UCH_SET_PIXEL(pixels_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, world->tiles[i].mat->alpha);
					
				}
			}
		}

		memset(world->dirty, false, (size_t)world->width * world->height);
	
		while ((abs(accLoadX) > CHUNK_W / 2 || abs(accLoadY) > CHUNK_H / 2)) {
			int subX = std::fmax(std::fmin(accLoadX, CHUNK_W / 2), -CHUNK_W / 2);
			if (abs(subX) < CHUNK_W / 2) subX = 0;
			int subY = std::fmax(std::fmin(accLoadY, CHUNK_H / 2), -CHUNK_H / 2);
			if (abs(subY) < CHUNK_H / 2) subY = 0;

			world->loadZone.x += subX;
			world->loadZone.y += subY;

			int delta = 4 * (subX + subY * world->width);

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

void Game::updateFrameLate() {

	int nofsX;
	int nofsY;

	plPosX = (float)(plPosX + (freeCamX - plPosX) / 50.0f);
	plPosY = (float)(plPosY + (freeCamY - plPosY) / 50.0f);

	nofsX = (int)(-(plPosX + 0 + world->loadZone.x) * scale + Application::Width / 2.0f);
	nofsY = (int)(-(plPosY + 0 + world->loadZone.y) * scale + Application::Height / 2.0f);

	accLoadX += (nofsX - ofsX) / (float)scale;
	accLoadY += (nofsY - ofsY) / (float)scale;
	ofsX += (nofsX - ofsX);
	ofsY += (nofsY - ofsY);

	camX = (float)(camX + (desCamX - camX) * m_dt * 4.0f);
	camY = (float)(camY + (desCamY - camY) * m_dt * 4.0f);
}

void Game::renderTemperatureMap(World* world) {

	for (int x = 0; x < world->width; x++) {
		for (int y = 0; y < world->height; y++) {
			auto t = world->tiles[x + y * world->width];
			int32_t temp = t.temperature;
			Uint32 color = (Uint8)((temp + 1024) / 2048.0f * 255);

			const unsigned int offset = (world->width * 4 * y) + x * 4;
			pixelsTemp_ar[offset + 0] = color;        // b
			pixelsTemp_ar[offset + 1] = color;        // g
			pixelsTemp_ar[offset + 2] = color;        // r
			pixelsTemp_ar[offset + 3] = 0xf0;    // a
		}
	}

}

int Game::getAimSurface(int dist) {
	int dcx = this->mx - Application::Width / 2;
	int dcy = this->my - Application::Height / 2;

	float len = sqrtf(dcx * dcx + dcy * dcy);
	float udx = dcx / len;
	float udy = dcy / len;

	int mmx = Application::Width / 2.0f + udx * dist;
	int mmy = Application::Height / 2.0f + udy * dist;

	int wcx = (int)((Application::Width / 2.0f - ofsX - camX) / scale);
	int wcy = (int)((Application::Height / 2.0f - ofsY - camY) / scale);

	int wmx = (int)((mmx - ofsX - camX) / scale);
	int wmy = (int)((mmy - ofsY - camY) / scale);

	int startInd = -1;
	world->forLine(wcx, wcy, wmx, wmy, [&](int ind) {
		if (world->tiles[ind].mat->physicsType == PhysicsType::SOLID || world->tiles[ind].mat->physicsType == PhysicsType::SAND || world->tiles[ind].mat->physicsType == PhysicsType::SOUP) {
			startInd = ind;
			return true;
		}
		return false;
	});

	return startInd;
}

int Game::getAimSolidSurface(int dist) {
	int dcx = this->mx - Application::Width / 2;
	int dcy = this->my - Application::Height / 2;

	float len = sqrtf(dcx * dcx + dcy * dcy);
	float udx = dcx / len;
	float udy = dcy / len;

	int mmx = Application::Width / 2.0f + udx * dist;
	int mmy = Application::Height / 2.0f + udy * dist;

	int wcx = (int)((Application::Width / 2.0f - ofsX - camX) / scale);
	int wcy = (int)((Application::Height / 2.0f - ofsY - camY) / scale);

	int wmx = (int)((mmx - ofsX - camX) / scale);
	int wmy = (int)((mmy - ofsY - camY) / scale);

	int startInd = -1;
	world->forLine(wcx, wcy, wmx, wmy, [&](int ind) {
		if (world->tiles[ind].mat->physicsType == PhysicsType::SOLID) {
			startInd = ind;
			return true;
		}
		return false;
	});

	return startInd;
}