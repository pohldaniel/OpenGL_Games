#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/DebugRenderer.h>
#include <engine/octree/WorkQueue.h>
#include <Physics/ShapeDrawer.h>
#include <States/Menu.h>

#include "KCCInterface.h"
#include "Application.h"
#include "Globals.h"

KCCInterface::KCCInterface(StateMachine& machine) : State(machine, States::KCC) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	float aspect = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	m_view.lookAt(Vector3f(0.0f, 50.0f, 0.0f), Vector3f(0.0f, 50.0f - 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_camera.orthographic(-60.0f * aspect, 60.0f * aspect, -60.0f, 60.0f, -1000.0f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(15.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	
	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);
	createShapes();
	createPhysics();

	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum);

	DebugRenderer::Get().setEnable(true);
	m_root = new SceneNodeLC();
	ShapeNode* child;

	child = m_root->addChild<ShapeNode, Shape>(m_baseShape);
	child->setPosition(0.0f, 0.0f, 0.0f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_upperFloorShape);
	child->setPosition(30.16f, 6.98797f, 10.0099f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_rampShape);
	child->setPosition(13.5771f, 6.23965f, 10.9272f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_ramp2Shape);
	child->setPosition(-22.8933f, 2.63165f, -23.6786f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_ramp3Shape);
	child->setPosition(-15.2665f, 1.9782f, -43.135f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_liftShape);
	child->setPosition(35.5938f, 0.350185f, 10.4836f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_liftExteriorShape);
	child->setPosition(35.6211f, 7.66765f, 10.4388f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_liftButtonShape);
	child->setPosition(Vector3f(35.5938f, 0.350185f, 10.4836f) + Vector3f(0.0f, 0.0619186f, 0.0f));
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	child = m_root->addChild<ShapeNode, Shape>(m_diskShape);
	child->setPosition(26.1357f, 7.00645f, -34.7563f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	m_movingPlatform = new MovingPlatform();
	m_movingPlatform->initialize(child, m_kinematicPlatform1, child->getWorldPosition() + Vector3f(0.0f, 0.0f, 20.0f));

	child = m_root->addChild<ShapeNode, Shape>(m_cylinderShape);
	child->setPosition(-0.294956f, 3.46579f, 28.3161f);
	child->updateSOP();
	child->OnOctreeSet(m_octree);
	m_entities.push_back(child);

	m_frustum.init();
	m_frustum.getDebug() = true;
}

KCCInterface::~KCCInterface() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	Globals::physics->removeAllCollisionObjects();
	ShapeDrawer::Get().shutdown();
}

void KCCInterface::fixedUpdate() {
	m_movingPlatform->fixedUpdate(m_fdt);
}

void KCCInterface::update() {
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

	m_octree->updateFrameNumber();
	perspective.perspective(m_fovx, (float)Application::Width / (float)Application::Height, m_near, m_far);
	m_frustum.updatePlane(perspective, m_camera.getViewMatrix());
	m_frustum.updateVertices(perspective, m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);

	m_octree->updateOctree(m_dt);
}

void KCCInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", !m_overview ? m_camera.getPerspectiveMatrix() : m_camera.getOrthographicMatrix());
	shader->loadMatrix("u_view", !m_overview ? m_camera.getViewMatrix() : m_view);
	Globals::textureManager.get("proto").bind();

	for (size_t i = 0; i < m_octree->rootLevelOctants.size(); ++i) {
		const Octree::ThreadOctantResult& result = m_octree->octantResults[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			if (m_debugTree)
				octant->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

			const std::vector<OctreeNode*>& drawables = octant->Drawables();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				OctreeNode* drawable = *dIt;
				shader->loadMatrix("u_model", drawable->getWorldTransformation());
				drawable->drawRaw();
				if (m_debugTree)
					drawable->OnRenderAABB(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
	}

	if (m_useOcclusion)
		m_octree->RenderOcclusionQueries();

	if (m_overview) {
		m_frustum.updateVbo(perspective, m_camera.getViewMatrix());
		m_frustum.drawFrustum(m_camera.getOrthographicMatrix(), m_view, m_distance, Vector4f(0.0f, 0.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	}

	!m_overview ? DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix()) : DebugRenderer::Get().SetProjectionView(m_camera.getOrthographicMatrix(), m_view);

	if (m_debugTree)
		DebugRenderer::Get().drawBuffer();

	if (m_debugPhysic) {
		!m_overview ? ShapeDrawer::Get().setProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix()) : ShapeDrawer::Get().setProjectionView(m_camera.getOrthographicMatrix(), m_view);

		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	if (m_drawUi)
		renderUi();
}

void KCCInterface::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void KCCInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void KCCInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void KCCInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void KCCInterface::OnKeyDown(Event::KeyboardEvent& event) {
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

void KCCInterface::OnKeyUp(Event::KeyboardEvent& event) {

}

void KCCInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void KCCInterface::renderUi() {
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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::Checkbox("Overview", &m_overview);
	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_octree->m_useCulling = m_useCulling;
	}

	if (ImGui::Checkbox("Use Occlusion", &m_useOcclusion)) {
		m_octree->m_useOcclusion = m_useOcclusion;
	}
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::SliderFloat("Fovx", &m_fovx, 0.01f, 180.0f);
	ImGui::SliderFloat("Far", &m_far, 25.0f, 1100.0f);
	ImGui::SliderFloat("Near", &m_near, 0.1f, 200.0f);
	ImGui::SliderFloat("Distance", &m_distance, -100.0f, 100.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void KCCInterface::createShapes() {
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	mdlConverter.mdlToBuffer("res/models/base.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_baseShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_baseShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/upperFloor.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_upperFloorShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_upperFloorShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/ramp.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_rampShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_rampShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/ramp2.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp2Shape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_ramp2Shape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/ramp3.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp3Shape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_ramp3Shape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/Lift.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_liftShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/liftExterior.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftExteriorShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_liftExteriorShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/disk.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_diskShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_diskShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/Cylinder.mdl", { 6.0f, 0.7f, 6.0f }, vertexBuffer, indexBuffer);
	m_cylinderShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cylinderShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/LiftButton.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftButtonShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_liftButtonShape.createBoundingBox();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
}

void KCCInterface::createPhysics() {
	Physics::AddStaticRigidBody(Physics::BtTransform(btVector3(0.0f, -0.05f, 0.0f)), new btConvexHullShape((btScalar*)(&m_baseShape.getPositions()[0]), m_baseShape.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	Physics::AddStaticRigidBody(Physics::BtTransform(btVector3(30.16f, 6.98797f, 10.0099f)), Physics::CreateCollisionShape(&m_upperFloorShape, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	
	Physics::AddStaticRigidBody(Physics::BtTransform(btVector3(13.5771f, 6.23965f, 10.9272f)), Physics::CreateCollisionShape(&m_rampShape, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	Physics::AddStaticRigidBody(Physics::BtTransform(btVector3(-22.8933f, 2.63165f, -23.6786f)), new btConvexHullShape((btScalar*)(&m_ramp2Shape.getPositions()[0]), m_ramp2Shape.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	Physics::AddStaticRigidBody(Physics::BtTransform(btVector3(-15.2665f, 1.9782f, -43.135f)), new btConvexHullShape((btScalar*)(&m_ramp3Shape.getPositions()[0]), m_ramp3Shape.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);

	Physics::AddStaticObject(Physics::BtTransform(btVector3(35.6211f, 7.66765f, 10.4388f)), Physics::CreateCollisionShape(&m_liftExteriorShape, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);

	m_kinematicLift = Physics::AddKinematicObject(Physics::BtTransform(btVector3(35.5938f, 0.350185f, 10.4836f)), Physics::CreateCollisionShape(&m_liftShape, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	m_liftButtonTrigger = Physics::AddKinematicTrigger(Physics::BtTransform(btVector3(35.5938f, 0.412104f, 10.4836f)), new btCylinderShape(btVector3(50.0f, 50.0f, 40.0f) * 0.01f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	m_kinematicPlatform1 = Physics::AddKinematicObject(Physics::BtTransform(btVector3(26.1357f, 7.00645f, -34.7563f)), Physics::CreateCollisionShape(&m_diskShape, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
	m_kinematicPlatform2 = Physics::AddKinematicObject(Physics::BtTransform(btVector3(-0.294956f, 3.46579f, 28.3161f)), Physics::CreateCollisionShape(&m_cylinderShape, btVector3(1.0f, 1.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::RAY | Physics::collisiontypes::CAMERA);
}