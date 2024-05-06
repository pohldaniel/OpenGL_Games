#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Turso.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include <tracy/Tracy.hpp>


Turso::Turso(StateMachine& machine) : State(machine, States::TURSO) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	float aspect = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	m_view.lookAt(Vector3f(0.0f, 50.0f, 0.0f), Vector3f(0.0f, 50.0f - 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_camera.orthographic(-50.0f * aspect, 50.0f * aspect, -50.0f, 50.0f, -1000.0f, 1000.0f);

	m_camera.lookAt(Vector3f(0.0f, 0.0f, 75.0f), Vector3f(0.0f, 0.0f, 75.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(15.0f);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	bool useThreads = true;

	// Create subsystems that don't depend on the application window / OpenGL context
	workQueue = new WorkQueueTu(useThreads ? 0 : 1);
	profiler = new Profiler();
	log = new Log();
	cache = new ResourceCache();
	cache->AddResourceDir("Data");

	// Create the Graphics subsystem to open the application window and initialize OpenGL
	graphics = new Graphics("Turso3D renderer test", IntVector2(Application::Width, Application::Height));
	graphics->Initialize();

	// Create subsystems that depend on the application window / OpenGL
	//input = new InputTu(graphics->Window());
	renderer = new Renderer(m_frustum, m_camera);
	debugRenderer = new DebugRendererTu();

	renderer->SetupShadowMaps(1024, 2048, FMT_D16);

	// Rendertarget textures
	viewFbo = new FrameBufferTu();
	viewMRTFbo = new FrameBufferTu();
	ssaoFbo = new FrameBufferTu();
	colorBuffer = new TextureTu();
	normalBuffer = new TextureTu();
	depthStencilBuffer = new TextureTu();
	ssaoTexture = new TextureTu();
	occlusionDebugTexture = new TextureTu();

	// Random noise texture for SSAO
	unsigned char noiseData[4 * 4 * 4];
	for (int i = 0; i < 4 * 4; ++i){
		Vector3 noiseVec(Random() * 2.0f - 1.0f, Random() * 2.0f - 1.0f, Random() * 2.0f - 1.0f);
		noiseVec.Normalize();

		noiseData[i * 4 + 0] = (unsigned char)(noiseVec.x * 127.0f + 128.0f);
		noiseData[i * 4 + 1] = (unsigned char)(noiseVec.y * 127.0f + 128.0f);
		noiseData[i * 4 + 2] = (unsigned char)(noiseVec.z * 127.0f + 128.0f);
		noiseData[i * 4 + 3] = 0;
	}

	ImageLevel noiseDataLevel(IntVector2(4, 4), FMT_RGBA8, &noiseData[0]);
	noiseTexture = new TextureTu();
	noiseTexture->Define(TEX_2D, IntVector2(4, 4), FMT_RGBA8, 1, 1, &noiseDataLevel);
	noiseTexture->DefineSampler(FILTER_POINT);

	m_octree = new OctreeTu();

	colorBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_RGBA8);
	colorBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	depthStencilBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_D32);
	depthStencilBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	normalBuffer->Define(TEX_2D, IntVector2(Application::Width, Application::Height), FMT_RGBA8);
	normalBuffer->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	viewFbo->Define(colorBuffer, depthStencilBuffer);

	std::vector<TextureTu*> mrt;
	mrt.push_back(colorBuffer.Get());
	mrt.push_back(normalBuffer.Get());
	viewMRTFbo->Define(mrt, depthStencilBuffer);

	ssaoTexture->Define(TEX_2D, IntVector2(colorBuffer->Width() / 2, colorBuffer->Height() / 2), FMT_R32F, 1, 1);
	ssaoTexture->DefineSampler(FILTER_BILINEAR, ADDRESS_CLAMP, ADDRESS_CLAMP, ADDRESS_CLAMP);
	ssaoFbo->Define(ssaoTexture, nullptr);

	graphics->SetViewport(IntRect(0, 0, Application::Width, Application::Height));


	m_root = new SceneNodeLC();
	ShapeNode* child;
	for (int x = -10; x < 10; x++) {
		for (int y = -10; y < 10; y++) {
			for (int z = -10; z < 10; z++) {
				child = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("cube"));
				child->setPosition(2.2f * x, 2.2f * y, 2.2f * z);
				//Important: guarantee thread safeness
				child->updateSOP();
				m_octree->QueueUpdate(child);
				m_entities.push_back(child);
			}
		}
	}
	child = m_root->addChild<ShapeNode, Shape>(Globals::shapeManager.get("quad_xy"));
	child->setPosition(0.0f, 0.0f, 30.0f);
	child->updateSOP();
	m_octree->QueueUpdate(child);
	m_entities.push_back(child);

	//glFrontFace(GL_CW);
}

Turso::~Turso() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Turso::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void Turso::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
	}
	perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
	m_frustum.updatePlane(perspective, m_camera.getViewMatrix());
	m_frustum.updateVertices(perspective, m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
}

void Turso::render() {
		
	
	renderer->PrepareView(m_octree, shadowMode > 0, useOcclusion, m_dt);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_vp", m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix());
	Globals::textureManager.get("marble").bind(0);

	for (size_t i = 0; i < renderer->rootLevelOctants.size(); ++i) {
		const ThreadOctantResult& result = renderer->octantResults[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			OctantTu* octant = oIt->first;			
			const std::vector<ShapeNode*>& drawables = octant->Drawables();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				ShapeNode* drawable = *dIt;
				shader->loadMatrix("u_model", drawable->getWorldTransformation());
				drawable->getShape().drawRaw();
			}
		}
	}

	shader->unuse();

	if (useOcclusion) {
		renderer->RenderOcclusionQueries();
	}
	

	if (m_drawUi)
		renderUi();
}

void Turso::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void Turso::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Turso::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Turso::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Turso::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Turso::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Turso::renderUi() {
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
	ImGui::Checkbox("Use Occlusion", &useOcclusion);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}