#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <engine/DebugRenderer.h>
#include <States/Menu.h>

#include "OctreeInterface.h"
#include "Application.h"
#include "Globals.h"

static const float OCCLUSION_MARGIN = 0.1f;
//unsigned int occlusionQueryType = GL_SAMPLES_PASSED;
unsigned int occlusionQueryType = GL_ANY_SAMPLES_PASSED;

OctreeInterface::OctreeInterface(StateMachine& machine) : State(machine, States::OCTREEINTERFACE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	float aspect = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	m_view.lookAt(Vector3f(0.0f, 50.0f, 0.0f), Vector3f(0.0f, 50.0f - 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_camera.orthographic(-50.0f * aspect, 50.0f * aspect, -50.0f, 50.0f, -1000.0f, 1000.0f);
	
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 25.0f), Vector3f(0.0f, 0.0f, 25.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(15.0f);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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

	m_frustum.init();
	m_frustum.getDebug() = true;
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
	perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
	m_frustum.updatePlane(perspective, m_camera.getViewMatrix());
	m_frustum.updateVertices(perspective, m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
	updateOctree();
}

void OctreeInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", !m_overview ? m_camera.getPerspectiveMatrix() : m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", !m_overview ? m_camera.getViewMatrix() : m_view);
	
	Globals::textureManager.get("marble").bind(0);
	
	for (size_t i = 0; i < rootLevelOctants.size(); ++i) {
		const ThreadOctantResult& result = octantResults[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			octant->OnRenderAABB();

			const std::vector<ShapeNode*>& drawables = octant->Drawables();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				ShapeNode* drawable = *dIt;
				
				shader->loadMatrix("u_model", drawable->getWorldTransformation());
				drawable->getShape().drawRaw();
				//drawable->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}

	shader->unuse();

	if (m_useOcclusion)
		RenderOcclusionQueries();

	m_frustum.updateVbo(perspective, m_camera.getViewMatrix());

	!m_overview ? m_frustum.drawFrustum(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix(), m_distance) : m_frustum.drawFrustum(m_camera.getOrthographicMatrix(), m_view, m_distance);
	!m_overview ? DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix()) : DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_view);
	
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
		Mouse::instance().detach();
		m_isRunning = false;
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
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
	ImGui::SliderFloat("Fovx", &m_fovx, 0.01f, 180.0f);
	ImGui::SliderFloat("Far", &m_far, 25.0f, 1100.0f);
	ImGui::SliderFloat("Near", &m_near, 0.1f, 200.0f);
	ImGui::SliderFloat("Distance", &m_distance, -100.0f, 100.0f);
	ImGui::Checkbox("Overview", &m_overview);
	ImGui::Checkbox("Use Culling", &m_useCulling);
	if (ImGui::Button("Reset")) {
		m_far = m_camera.getFar();
		m_near = m_camera.getNear();
		m_fovx = m_camera.getFovXDeg();
	}
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
	occlusionQueries.clear();
}

void OctreeInterface::CollectOctants(Octant* octant, ThreadOctantResult& result, unsigned char planeMask) {
	const BoundingBox& octantBox = octant->CullingBox();

	if (planeMask) {
		planeMask = m_useCulling ? m_frustum.isInsideMasked(octantBox, planeMask) : 0x00;
		if (planeMask == 0xff) {
			if (octant->Visibility() != VIS_OUTSIDE_FRUSTUM)
				octant->SetVisibility(VIS_OUTSIDE_FRUSTUM, true);
			return;
		}
	}

	if (m_useOcclusion){
		// If was previously outside frustum, reset to visible-unknown
		if (octant->Visibility() == VIS_OUTSIDE_FRUSTUM)
			octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);

		switch (octant->Visibility()){
			// If octant is occluded, issue query if not pending, and do not process further this frame
			case VIS_OCCLUDED:
				AddOcclusionQuery(octant, result, planeMask);
				return;

			// If octant was occluded previously, but its parent came into view, issue tests along the hierarchy but do not render on this frame
			case VIS_OCCLUDED_UNKNOWN:
				AddOcclusionQuery(octant, result, planeMask);
				if (octant != m_octree->Root() && octant->HasChildren()){
					for (size_t i = 0; i < NUM_OCTANTS; ++i){
						if (octant->Child(i))
							CollectOctants(octant->Child(i), result, planeMask);
					}
				}
				return;

			// If octant has unknown visibility, issue query if not pending, but collect child octants and drawables
			case VIS_VISIBLE_UNKNOWN:
				AddOcclusionQuery(octant, result, planeMask);
				break;

			// If the octant's parent is already visible too, only test the octant if it is a "leaf octant" with drawables
			// Note: visible octants will also add a time-based staggering to reduce queries
			case VIS_VISIBLE:
				Octant* parent = octant->Parent();
				if (octant->Drawables().size() > 0 || (parent && parent->Visibility() != VIS_VISIBLE))
					AddOcclusionQuery(octant, result, planeMask);
				break;
		}
	}else {
		octant->SetVisibility(VIS_VISIBLE_UNKNOWN, false);
	}

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

void OctreeInterface::AddOcclusionQuery(Octant* octant, ThreadOctantResult& result, unsigned char planeMask){
	// No-op if previous query still ongoing. Also If the octant intersects the frustum, verify with SAT test that it actually covers some screen area
	// Otherwise the occlusion test will produce a false negative
	
	//if (octant->CheckNewOcclusionQuery(lastFrameTime) && (!planeMask || m_frustum.isInsideSAT(octant->CullingBox(), m_frustum.m_frustumSATData)))
		//result.occlusionQueries.push_back(octant);
}

void OctreeInterface::CollectOctantsWork(Task* task_, unsigned threadIndex) {

	CollectOctantsTask* task = static_cast<CollectOctantsTask*>(task_);
	Octant* octant = task->startOctant;
	ThreadOctantResult& result = octantResults[task->resultIdx];

	CollectOctants(octant, result);

	numPendingBatchTasks.fetch_add(-1);
}

void OctreeInterface::RenderOcclusionQueries(){


	//if (!boundingBoxShaderProgram)
		//return;

	Matrix4f boxMatrix(Matrix4f::IDENTITY);
	float nearClip = m_camera.getNear();

	// Use camera's motion since last frame to enlarge the bounding boxes. Use multiplied movement speed to account for latency in query results
	Vector3f cameraPosition = m_camera.getPosition();
	Vector3f cameraMove = cameraPosition - previousCameraPosition;
	Vector3f enlargement = (OCCLUSION_MARGIN + 4.0f * cameraMove.length()) * Vector3f::ONE;

	//boundingBoxVertexBuffer->Bind(MASK_POSITION);
	//boundingBoxIndexBuffer->Bind();
	//boundingBoxShaderProgram->Bind();
	//graphics->SetRenderState(BLEND_REPLACE, CULL_BACK, CMP_LESS_EQUAL, false, false);

	glDisable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glDepthFunc(GL_LEQUAL);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);

	for (size_t i = 0; i < NUM_OCTANT_TASKS; ++i){
		for (auto it = octantResults[i].occlusionQueries.begin(); it != octantResults[i].occlusionQueries.end(); ++it){
			Octant* octant = *it;

			const BoundingBox& octantBox = octant->CullingBox();
			BoundingBox box(octantBox.min - enlargement, octantBox.max + enlargement);

			// If bounding box could be clipped by near plane, assume visible without performing query
			if (box.distance(cameraPosition) < 2.0f * nearClip){
				//octant->OnOcclusionQueryResult(true);
				continue;
			}

			Vector3f size = box.getHalfSize();
			Vector3f center = box.getCenter();

			boxMatrix[0][0] = size[0];
			boxMatrix[1][1] = size[1];
			boxMatrix[2][2] = size[2];
			boxMatrix[3][0] = center[0];
			boxMatrix[3][1] = center[1];
			boxMatrix[3][2] = center[2];

			//graphics->SetUniform(boundingBoxShaderProgram, U_WORLDMATRIX, boxMatrix);

			unsigned queryId = BeginOcclusionQuery(octant);
			//graphics->DrawIndexed(PT_TRIANGLE_LIST, 0, NUM_BOX_INDICES);
			EndOcclusionQuery();

			// Store query to octant to make sure we don't re-test it until result arrives
			//octant->OnOcclusionQuery(queryId);
		}
	}

	previousCameraPosition = cameraPosition;
}

unsigned OctreeInterface::BeginOcclusionQuery(void* object){
	GLuint queryId;

	if (freeQueries.size()){
		queryId = freeQueries.back();
		freeQueries.pop_back();
	}else
		glGenQueries(1, &queryId);

	glBeginQuery(occlusionQueryType, queryId);
	pendingQueries.push_back(std::make_pair(queryId, object));

	return queryId;
}

void OctreeInterface::EndOcclusionQuery(){
	glEndQuery(occlusionQueryType);
}