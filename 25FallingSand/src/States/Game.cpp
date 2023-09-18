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
#include "Shaders.hpp"
#include "OptionsUI.h"
#include "InGameUI.h"
#include "Controls.hpp"

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

	m_background1.setLayer(std::vector<BackgroundLayer>{ {&Globals::textureManager.get("bg_layer_2"), 2, 1.0f, 0.5f, Vector2f(0.333f, 0.666f)},
	{ &Globals::textureManager.get("bg_layer_3"), 2, 0.0f, -0.0666f, Vector2f(0.37f, 0.37f + 0.333f) },
	{ &Globals::textureManager.get("bg_layer_4"), 2, 3.0f, -0.01f, Vector2f(0.333f, 0.666f) },
	{ &Globals::textureManager.get("bg_layer_5"), 2, 0.0f, 0.01333f, Vector2f(0.333f, 0.666f) } });
	m_background1.setSpeed(0.005f);

	m_background2.setLayer(std::vector<BackgroundLayer>{ {&Globals::textureManager.get("forest_1"), 1, 1.0f},
	{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
	{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
	{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
	{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background2.setSpeed(0.005f);

	init();
	m_timer.reset();
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
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_switch ? m_background1.draw() : m_background2.draw();
	updateFrameEarly();
	tick();
	updateFrameLate();

	target = realTarget;
	GPU_Clear(target);

	renderLate();
	target = realTarget;

	GPU_ActivateShaderProgram(0, NULL);
	GPU_FlushBlitBuffer();

	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
	//MainMenuUI::Draw(this);
	//ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	target = Application::target;
	realTarget = target;

	worldInitThread = worldInitThreadPool->push([&](int id) {
		world = new World();
		world->noSaveLoad = true;
		world->init(gameDir.getWorldPath("mainMenu"), (int)ceil(MAX_WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(MAX_HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, target);

	});

	unsigned int seed = (unsigned int)Time::millis();
	srand(seed);

	Materials::init();

	movingTiles = new uint16_t[Materials::nMaterials];

	b2DebugDraw = new b2DebugDraw_impl(target);
	worldInitThread.get();

	int w;
	int h;
	SDL_GetWindowSize(Application::SWindow, &w, &h);

	GPU_SetWindowResolution(w, h);
	GPU_ResetProjection(realTarget);

	handleWindowSizeChange(w, h);
	updateDirtyPool = new ctpl::thread_pool(6);

	loadShaders();

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

void Game::handleWindowSizeChange(int newWidth, int newHeight) {

	int prevWidth = Application::Width;
	int prevHeight = Application::Height;

	Application::Width = newWidth;
	Application::Height = newHeight;

	if (loadingTexture) {
		GPU_FreeImage(loadingTexture);
		GPU_FreeImage(texture);
		GPU_FreeImage(worldTexture);
		GPU_FreeImage(lightingTexture);
		GPU_FreeImage(emissionTexture);
		GPU_FreeImage(textureFire);
		GPU_FreeImage(textureFlow);
		GPU_FreeImage(texture2Fire);
		GPU_FreeImage(textureLayer2);
		GPU_FreeImage(textureBackground);
		GPU_FreeImage(textureObjects);
		GPU_FreeImage(textureObjectsLQ);
		GPU_FreeImage(textureObjectsBack);
		GPU_FreeImage(textureParticles);
		GPU_FreeImage(textureEntities);
		GPU_FreeImage(textureEntitiesLQ);
		GPU_FreeImage(temperatureMap);
		GPU_FreeImage(backgroundImage);
	}

	loadingOnColor = 0xFFFFFFFF;
	loadingOffColor = 0x000000FF;

	loadingTexture = GPU_CreateImage(loadingScreenW = (Application::Width / 20), loadingScreenH = (Application::Width / 20), GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(loadingTexture, GPU_FILTER_NEAREST);
	texture = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
	worldTexture = GPU_CreateImage(world->width * Settings::hd_objects_size, world->height * Settings::hd_objects_size, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(worldTexture, GPU_FILTER_NEAREST);
	GPU_LoadTarget(worldTexture);

	lightingTexture = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(lightingTexture, GPU_FILTER_NEAREST);

	GPU_LoadTarget(lightingTexture);

	emissionTexture = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(emissionTexture, GPU_FILTER_NEAREST);

	textureFlow = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureFlow, GPU_FILTER_NEAREST);

	textureFire = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureFire, GPU_FILTER_NEAREST);

	texture2Fire = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(texture2Fire, GPU_FILTER_NEAREST);

	GPU_LoadTarget(texture2Fire);

	textureLayer2 = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureLayer2, GPU_FILTER_NEAREST);

	textureBackground = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureBackground, GPU_FILTER_NEAREST);

	textureObjects = GPU_CreateImage(world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1), GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureObjects, GPU_FILTER_NEAREST);

	textureObjectsLQ = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureObjectsLQ, GPU_FILTER_NEAREST);

	textureObjectsBack = GPU_CreateImage(world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1), GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureObjectsBack, GPU_FILTER_NEAREST);

	GPU_LoadTarget(textureObjects);

	GPU_LoadTarget(textureObjectsLQ);

	GPU_LoadTarget(textureObjectsBack);

	textureParticles = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(textureParticles, GPU_FILTER_NEAREST);

	textureEntities = GPU_CreateImage(world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1), GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_LoadTarget(textureEntities);

	GPU_SetImageFilter(textureEntities, GPU_FILTER_NEAREST);

	textureEntitiesLQ = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_LoadTarget(textureEntitiesLQ);

	GPU_SetImageFilter(textureEntitiesLQ, GPU_FILTER_NEAREST);

	temperatureMap = GPU_CreateImage(world->width, world->height, GPU_FormatEnum::GPU_FORMAT_RGBA);

	GPU_SetImageFilter(temperatureMap, GPU_FILTER_NEAREST);

	backgroundImage = GPU_CreateImage(Application::Width, Application::Height, GPU_FormatEnum::GPU_FORMAT_RGBA);

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

		GPU_BlitRectX(cur->texture, NULL, tgt, &r, cur->body->GetAngle() * 180 / PI, 0, 0, GPU_FLIP_NONE);

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
					}
					else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SAND) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						world->dirty[wxd + wyd * world->width] = true;
						cur->body->SetLinearVelocity({ cur->body->GetLinearVelocity().x * (float)0.99, cur->body->GetLinearVelocity().y * (float)0.99 });
						cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.98);
						break;
					}
					else if (world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SOUP) {
						world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
						world->tiles[wxd + wyd * world->width] = rmat;
						world->dirty[wxd + wyd * world->width] = true;
						cur->body->SetLinearVelocity({ cur->body->GetLinearVelocity().x * (float)0.998, cur->body->GetLinearVelocity().y * (float)0.998 });
						cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.99);
						break;
					}
				}
			}
		}
	}

	// render entities
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



	// entity fluid displacement & make solid
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

	if (Settings::tick_world && world->readyToMerge.size() == 0) {
		world->tick();
	}

	if (Controls::DEBUG_TICK->get()) {
		world->tick();
	}

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


	unsigned char* dpixelsLayer2_ar = pixelsLayer2_ar;
	results.push_back(updateDirtyPool->push([&](int id) {

		for (int i = 0; i < world->width * world->height; i++) {
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


	unsigned char* dpixelsBackground_ar = pixelsBackground_ar;
	results.push_back(updateDirtyPool->push([&](int id) {

		for (int i = 0; i < world->width * world->height; i++) {
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
		const unsigned int offset = i * 4;

		if (objectDelete[i]) {
			world->tiles[i] = Tiles::NOTHING;
		}
	}

	for (int i = 0; i < results.size(); i++) {
		results[i].get();
	}

	GPU_UpdateImageBytes(textureParticles,NULL,&pixelsParticles_ar[0],world->width * 4);

	if (hadDirty)		   memset(world->dirty, false, (size_t)world->width * world->height);
	if (hadLayer2Dirty)	   memset(world->layer2Dirty, false, (size_t)world->width * world->height);
	if (hadBackgroundDirty) memset(world->backgroundDirty, false, (size_t)world->width * world->height);

	if (Settings::tick_temperature && tickTime % 4 == 2) {
		world->tickTemperature();
	}
	if (Settings::draw_temperature_map && tickTime % 4 == 0) {
		renderTemperatureMap(world);
	}


	if (hadDirty) {
		GPU_UpdateImageBytes(texture, NULL, &pixels[0], world->width * 4);
		GPU_UpdateImageBytes(emissionTexture,NULL,&pixelsEmission[0],world->width * 4);
	}

	if (hadLayer2Dirty) {
		GPU_UpdateImageBytes(textureLayer2,NULL,&pixelsLayer2[0],world->width * 4);
	}

	if (hadBackgroundDirty) {
		GPU_UpdateImageBytes(textureBackground,NULL,&pixelsBackground[0],world->width * 4);
	}

	if (hadFlow) {
		GPU_UpdateImageBytes(textureFlow,NULL,&pixelsFlow[0],world->width * 4);
		waterFlowPassShader->dirty = true;
	}

	if (hadFire) {
		GPU_UpdateImageBytes(textureFire,NULL,&pixelsFire[0],world->width * 4);
	}

	if (Settings::draw_temperature_map) {
		GPU_UpdateImageBytes(temperatureMap,NULL,&pixelsTemp[0],world->width * 4);
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



void Game::renderLate() {

	target = backgroundImage->target;
	GPU_Clear(target);

	GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);

	GPU_Rect r1 = GPU_Rect{ (float)(ofsX + camX), (float)(ofsY + camY), (float)(world->width * scale), (float)(world->height * scale) };

	target = realTarget;

	GPU_BlitRect(backgroundImage, NULL, target, NULL);
	GPU_SetBlendMode(texture, GPU_BLEND_NORMAL);

	int lmsx = (int)((mx - ofsX - camX) / scale);
	int lmsy = (int)((my - ofsY - camY) / scale);

	GPU_Clear(worldTexture->target);
	GPU_BlitRect(texture, NULL, worldTexture->target, NULL);

	if (Settings::draw_shaders) newLightingShader->activate();

	// I use this to only rerender the lighting when a parameter changes or N times per second anyway
	// Doing this massively reduces the GPU load of the shader
	bool needToRerenderLighting = false;

	unsigned int elapsedSinceLast = m_timer.getElapsedTimeMilli();
	if (elapsedSinceLast > 100u) {
		needToRerenderLighting = true;
		m_timer.reset();
	}

	if (Settings::draw_shaders && world) {
		float lightTx;
		float lightTy;

		if (world->player) {
			lightTx = (world->loadZone.x + world->player->x + world->player->hw / 2.0f) / (float)world->width;
			lightTy = (world->loadZone.y + world->player->y + world->player->hh / 2.0f) / (float)world->height;
		}
		else {
			lightTx = lmsx / (float)world->width;
			lightTy = lmsy / (float)world->height;
		}

		if (newLightingShader->lastLx != lightTx || newLightingShader->lastLy != lightTy) needToRerenderLighting = true;
		newLightingShader->update(worldTexture, emissionTexture, lightTx, lightTy);
		if (newLightingShader->lastQuality != Settings::lightingQuality) {
			needToRerenderLighting = true;
		}
		newLightingShader->setQuality(Settings::lightingQuality);

		int nBg = 0;
		int range = 64;
		for (int xx = std::max(0, (int)(lightTx * world->width) - range); xx <= std::min((int)(lightTx * world->width) + range, world->width - 1); xx++) {
			for (int yy = std::max(0, (int)(lightTy * world->height) - range); yy <= std::min((int)(lightTy * world->height) + range, world->height - 1); yy++) {
				if (world->background[xx + yy * world->width] != 0x00) {
					nBg++;
				}
			}
		}

		newLightingShader_insideDes = std::min(std::max(0.0f, (float)nBg / ((range * 2) * (range * 2))), 1.0f);
		newLightingShader_insideCur += (newLightingShader_insideDes - newLightingShader_insideCur) / 2.0f * m_dt;

		float ins = newLightingShader_insideCur < 0.05 ? 0.0 : newLightingShader_insideCur;
		if (newLightingShader->lastInside != ins) needToRerenderLighting = true;
		newLightingShader->setInside(ins);
		newLightingShader->setBounds(world->tickZone.x * Settings::hd_objects_size, world->tickZone.y * Settings::hd_objects_size, (world->tickZone.x + world->tickZone.w) * Settings::hd_objects_size, (world->tickZone.y + world->tickZone.h) * Settings::hd_objects_size);

		if (newLightingShader->lastSimpleMode != Settings::simpleLighting) needToRerenderLighting = true;
		newLightingShader->setSimpleMode(Settings::simpleLighting);

		if (newLightingShader->lastEmissionEnabled != Settings::lightingEmission) needToRerenderLighting = true;
		newLightingShader->setEmissionEnabled(Settings::lightingEmission);

		if (newLightingShader->lastDitheringEnabled != Settings::lightingDithering) needToRerenderLighting = true;
		newLightingShader->setDitheringEnabled(Settings::lightingDithering);
	}

	if (Settings::draw_shaders && needToRerenderLighting) {
		GPU_Clear(lightingTexture->target);
		GPU_BlitRect(worldTexture, NULL, lightingTexture->target, NULL);
	}

	if (Settings::draw_shaders) GPU_ActivateShaderProgram(0, NULL);

	GPU_BlitRect(worldTexture, NULL, target, &r1);
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