#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <States/Menu.h>

#include "OctreeInterface.h"
#include "Application.h"
#include "Globals.h"

OctreeInterface::OctreeInterface(StateMachine& machine) : State(machine, States::OCTREEINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 15.0f), Vector3f(0.0f, 0.0f, 15.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	WorkQueue::Init(0);
	workQueue = WorkQueue::Get();

	m_octree = new Octree();
	octantResults = new OctreeInterface::ThreadOctantResult[NUM_OCTANT_TASKS];

	for (size_t i = 0; i < NUM_OCTANTS + 1; ++i) {
		collectOctantsTasks[i] = new OctreeInterface::CollectOctantsTask(this, &OctreeInterface::CollectOctantsWork);
		collectOctantsTasks[i]->resultIdx = i;
	}

	DebugRenderer::Get().setEnable(true);

	m_cube = new ShapeNode(Globals::shapeManager.get("cube"));
	m_cube->setPosition(0.0f, 1.1f, 0.0f);
	m_octree->QueueUpdate(m_cube);
	
	m_cube = new ShapeNode(Globals::shapeManager.get("cube"));
	m_cube->setPosition(0.0f, -1.1f, 0.0f);
	m_octree->QueueUpdate(m_cube);
}

OctreeInterface::~OctreeInterface() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void OctreeInterface::fixedUpdate() {

}

void OctreeInterface::update() {

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

	updateOctree();
}

void OctreeInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	
	Globals::textureManager.get("marble").bind(0);
	
	
	//std::cout << "Size1: " << rootLevelOctants.size() << std::endl;
	for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
		const ThreadOctantResult& result = octantResults[i];

		//std::cout << "Size2: " << result.octants.size() << std::endl;

		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			octant->OnRenderAABB();

			const std::vector<ShapeNode*>& drawables = octant->Drawables();

			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				ShapeNode* drawable = *dIt;

				shader->loadMatrix("u_model", drawable->getWorldTransformation());
				drawable->getShape().drawRaw();
			}
		}
	}

	shader->unuse();

	DebugRenderer::Get().SetView(&m_camera);
	DebugRenderer::Get().drawBuffer();

	if (m_drawUi)
		renderUi();
}

void OctreeInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void OctreeInterface::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void OctreeInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void OctreeInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void OctreeInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
	
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void OctreeInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void OctreeInterface::renderUi() {
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

void OctreeInterface::updateOctree() {
	rootLevelOctants.clear();


	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i)
		octantResults[i].Clear();

	// Process moved / animated objects' octree reinsertions
	m_octree->Update(0);
	m_octree->FinishUpdate();

	// Enable threaded update during geometry / light gathering in case nodes' OnPrepareRender() causes further reinsertion queuing
	m_octree->SetThreadedUpdate(workQueue->NumThreads() > 1);

	// Find the starting points for octree traversal. Include the root if it contains drawables that didn't fit elsewhere
	Octant* rootOctant = m_octree->Root();
	if (rootOctant->Drawables().size()) {
		rootLevelOctants.push_back(rootOctant);
	}

	for (size_t i = 0; i < NUM_OCTANTS; ++i) {
		if (rootOctant->Child(i)) {
			rootLevelOctants.push_back(rootOctant->Child(i));
		}
	}

	numPendingBatchTasks.store((int)rootLevelOctants.size());

	for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
		collectOctantsTasks[i]->startOctant = rootLevelOctants[i];
	}

	workQueue->QueueTasks(rootLevelOctants.size(), reinterpret_cast<Task**>(&collectOctantsTasks[0]));

	while (numPendingBatchTasks.load() > 0)
		workQueue->TryComplete();

	//SortMainBatches();
	workQueue->Complete();
	m_octree->SetThreadedUpdate(false);
}

void OctreeInterface::ThreadOctantResult::Clear() {
	drawableAcc = 0;
	taskOctantIdx = 0;
	batchTaskIdx = 0;
	octants.clear();
}

void OctreeInterface::CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
	const BoundingBox& octantBox = octant->CullingBox();

	if (planeMask) {
		// If not already inside all frustum planes, do frustum test and terminate if completely outside
		//planeMask = m_useCulling ? frustum.IsInsideMasked(octantBox, planeMask) : 0x00;
		planeMask = 0x00;

		if (planeMask == 0xff) {
			// If octant becomes frustum culled, reset its visibility for when it comes back to view, including its children
			if (octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
				octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);

	const std::vector<ShapeNode*>& drawables = octant->Drawables();
	for (auto it = drawables.begin(); it != drawables.end(); ++it) {
		ShapeNode* drawable = *it;
		result.octants.push_back(std::make_pair(octant, planeMask));
		result.drawableAcc += drawables.end() - it;
		break;
	}

	if (octant != m_octree->Root() && octant->HasChildren()) {
		for (size_t i = 0; i < NUM_OCTANTS; ++i) {
			if (octant->Child(i))
				CollectOctants(octant->Child(i), result, planeMask);
		}
	}
}

void OctreeInterface::CollectOctantsWork(Task* task_, unsigned threadIndex) {

	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);
	Octant* octant = task->startOctant;
	ThreadOctantResult& result = octantResults[task->resultIdx];

	CollectOctants(octant, result);

	numPendingBatchTasks.fetch_add(-1);
}