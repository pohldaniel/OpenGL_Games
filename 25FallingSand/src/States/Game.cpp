#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include "MainMenuUI.h"

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
	world->init(gameDir.getWorldPath("mainMenu"), (int)ceil(MAX_WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(MAX_HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, target, NetworkMode::SERVER);

	for (int x = -CHUNK_W * 4; x < world->width + CHUNK_W * 4; x += CHUNK_W) {
		for (int y = -CHUNK_H * 3; y < world->height + CHUNK_H * 8; y += CHUNK_H) {
			world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
		}
	}
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

		GPU_BlitRectX(cur->texture, NULL, tgt, &r, cur->body->GetAngle() * 180 / (float)W_PI, 0, 0, GPU_FLIP_NONE);



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