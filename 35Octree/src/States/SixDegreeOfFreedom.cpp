#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/DebugRenderer.h>
#include <engine/BuiltInShader.h>
#include <States/Menu.h>
#include <Utils/BinaryIO.h>
#include <Physics/ShapeDrawer.h>

#include "SixDegreeOfFreedom.h"
#include "Application.h"
#include "Globals.h"

SixDegreeOfFreedom::SixDegreeOfFreedom(StateMachine& machine) : State(machine, States::DEFAULT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(80.0f, 2.0f, 40.0f) - Vector3f(0.0f, 0.0f, m_offsetDistance), Vector3f(80.0f, 2.0f, 40.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(m_rotationSpeed);
	m_camera.setMovingSpeed(15.0f);
	m_camera.setOffsetDistance(m_offsetDistance);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	glGenBuffers(1, &BuiltInShader::materialUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::materialUbo);
	glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, BuiltInShader::materialUbo, 0, 56);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &BuiltInShader::viewUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::viewUbo);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, BuiltInShader::viewUbo, 0, 64);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	auto shader = Globals::shaderManager.getAssetPointer("texture_view");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("material");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->unuse();
	
	Material::AddTexture("res/textures/ProtoWhite256.jpg");
	Material::GetTextures().back().setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Material::GetTextures().back().setWrapMode(GL_REPEAT);
	Material::AddTexture("res/textures/ProtoYellow256.jpg");
	Material::GetTextures().back().setFilter(GL_LINEAR_MIPMAP_LINEAR);
	Material::GetTextures().back().setWrapMode(GL_REPEAT);

	Material::AddMaterial();
	Material::GetMaterials().back().setDiffuse({0.7384f, 0.40064f, 0.44984f, 1.0f});
	Material::GetMaterials().back().setSpecular({ 0.5f, 0.5f, 0.5f, 1.0f });
	Material::GetMaterials().back().setShininess(20.0f);
	Material::GetMaterials().back().updateMaterialUbo(BuiltInShader::materialUbo);

	DebugRenderer::Get().setEnable(true);
	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);
	Physics::SetDebugMode(btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits);
	WorkQueue::Init(0);
	m_octree = new Octree(m_camera, m_frustum, m_dt);
	m_octree->setUseCulling(m_useCulling);
	m_octree->setUseOcclusionCulling(m_useOcclusion);

	createShapes();
	createScene();
	createPhysics();
}

SixDegreeOfFreedom::~SixDegreeOfFreedom() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Material::CleanupTextures();
	Material::CleanupMaterials();
	delete m_splinePath;
	delete m_octree;
	delete m_root;

	Physics::SetDebugMode(0u);
	Globals::physics->removeAllCollisionObjects();
	ShapeDrawer::Get().shutdown();
	glEnable(GL_BLEND);

	Texture::Unbind(0u);
}

void SixDegreeOfFreedom::fixedUpdate() {	
	updateSplinePath(m_fdt);
	m_kinematicBox->getMotionState()->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(m_splinePath->GetControlledNode()->getWorldPosition()), Physics::QuaternionFrom(m_splinePath->GetControlledNode()->getWorldOrientation())));	
	m_hoverBike->FixedUpdate(m_fdt);

	Globals::physics->stepSimulation(m_fdt);

	m_shipEntity->setPosition(Physics::VectorFrom(m_shipBody->getWorldTransform().getOrigin()));
	m_shipEntity->setOrientation(Physics::QuaternionFrom(m_shipBody->getWorldTransform().getRotation()));	
	
	
	//m_camera.setDirection(m_hoverBike->getDirection());

	Vector3f pos = m_splinePath->GetControlledNode()->getWorldPosition();
	//Vector3f pos = m_hoverBike->getWorldPosition();
	pos[1] += 1.5f;
	m_camera.Camera::setTarget(pos);


}

void SixDegreeOfFreedom::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debugPhysic = !m_debugPhysic;
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

	m_mousePicker.updatePosition(mouse.xPos(), mouse.yPos(), m_camera);
	m_octree->updateFrameNumber();
	m_frustum.updatePlane(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.updateVertices(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
	m_frustum.m_frustumSATData.calculate(m_frustum);
	m_octree->updateOctree();
}

void SixDegreeOfFreedom::render() {
	BuiltInShader::UpdateViewUbo(m_camera);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < m_octree->getRootLevelOctants().size(); ++i) {
		const Octree::ThreadOctantResult& result = m_octree->getOctantResults()[i];
		for (auto oIt = result.octants.begin(); oIt != result.octants.end(); ++oIt) {
			Octant* octant = oIt->first;
			if (m_debugTree)
				octant->OnRenderAABB(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

			const std::vector<OctreeNode*>& drawables = octant->getOctreeNodes();
			for (auto dIt = drawables.begin(); dIt != drawables.end(); ++dIt) {
				OctreeNode* drawable = *dIt;
				drawable->drawRaw();
				if (m_drawDebug)
					drawable->OnRenderOBB(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
			}
		}
	}
	if (m_debugTree || m_drawDebug) {
		m_splinePath->OnRenderDebug(Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
		DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
		DebugRenderer::Get().drawBuffer();
	}

	m_mousePicker.drawPicker(m_camera);
	if (m_debugPhysic) {
		ShapeDrawer::Get().setProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_CULL_FACE);		
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);
		Physics::DebugDrawWorld();
	}

	if (m_drawUi)
		renderUi();
}

void SixDegreeOfFreedom::OnMouseMotion(Event::MouseMoveEvent& event) {
	//m_mousePicker.updatePosition(event.x, event.y, m_camera);

	if (m_pickConstraint) {

		if (m_pickConstraint->getConstraintType() == D6_CONSTRAINT_TYPE) {
			btGeneric6DofConstraint* pickCon = static_cast<btGeneric6DofConstraint*>(m_pickConstraint);
			if (pickCon) {
				const MousePickCallback& callback = m_mousePicker.getCallback();

				btVector3 newRayTo = callback.m_target;
				btVector3 rayFrom = callback.m_origin;
				btVector3 oldPivotInB = pickCon->getFrameOffsetA().getOrigin();

				btVector3 dir = newRayTo - rayFrom;
				dir.normalize();
				dir *= m_mousePicker.getPickingDistance();

				pickCon->getFrameOffsetA().setOrigin(rayFrom + dir);
			}

		}else {
			btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickConstraint);
			if (pickCon) {
				const MousePickCallback& callback = m_mousePicker.getCallback();

				btVector3 newRayTo = callback.m_target;
				btVector3 rayFrom = callback.m_origin;
				btVector3 oldPivotInB = pickCon->getPivotInB();

				btVector3 dir = newRayTo - rayFrom;
				dir.normalize();
				dir *= m_mousePicker.getPickingDistance();

				pickCon->setPivotB(rayFrom + dir);
			}
		}
	}
}

void SixDegreeOfFreedom::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_offsetDistance += 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	if (event.direction == 0u) {
		m_offsetDistance -= 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}
}

void SixDegreeOfFreedom::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	//m_mousePicker.updatePosition(event.x, event.y, m_camera);
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow(), true, true);
		//Mouse::instance().attach(Application::GetWindow());
	}else if (event.button == 1u) {

		if (m_mousePicker.click(event.x, event.y, m_camera)) {
			m_mousePicker.setHasPicked(true);
			const MousePickCallback& callback = m_mousePicker.getCallback();
			pickObject(callback.m_hitPointWorld, callback.m_collisionObject);
		}
	}
}

void SixDegreeOfFreedom::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_mousePicker.setHasPicked(false);
		removePickingConstraint();
	}
	//Mouse::instance().detach();
	Mouse::instance().attach(Application::GetWindow(), false, false);
}

void SixDegreeOfFreedom::OnKeyDown(Event::KeyboardEvent& event) {
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

void SixDegreeOfFreedom::OnKeyUp(Event::KeyboardEvent& event) {

}

void SixDegreeOfFreedom::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("texture_view");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("material");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->unuse();
}

void SixDegreeOfFreedom::renderUi() {
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
	ImGui::Checkbox("Draw Debug", &m_drawDebug);
	if (ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_octree->setUseCulling(m_useCulling);
	}

	if (ImGui::Checkbox("Use Occlusion", &m_useOcclusion)) {
		m_octree->setUseOcclusionCulling(m_useOcclusion);
	}
	ImGui::Checkbox("Debug Tree", &m_debugTree);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SixDegreeOfFreedom::createShapes() {
	Utils::MdlIO mdlConverter;
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	mdlConverter.mdlToBuffer("res/models/box.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_boxShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_boxShape.createBoundingBox();
	m_boxShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/Cylinder.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_cylinderShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_cylinderShape.createBoundingBox();
	m_cylinderShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/ship.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_shipShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_shipShape.createBoundingBox();
	m_shipShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/models/hoverbike.mdl", 1.0f, vertexBuffer, indexBuffer);
	m_hoverbikeShape.fromBuffer(vertexBuffer, indexBuffer, 8);
	m_hoverbikeShape.createBoundingBox();
	m_hoverbikeShape.markForDelete();
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
}

void SixDegreeOfFreedom::createScene() {
	m_root = new SceneNodeLC();
	ShapeEntity* shapeEntity;

	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_boxShape, m_camera);
	shapeEntity->setPosition(0.0f, 0.0f, 0.0f);
	shapeEntity->setScale(500.0f, 1.0f, 500.0f);
	shapeEntity->setTextureIndex(0);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);
	
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_boxShape, m_camera);
	shapeEntity->setPosition(0.0f, 4.99f, 250.0f);
	shapeEntity->setScale(500.0f, 50.0f, 1.0f);
	shapeEntity->setTextureIndex(0);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);
	
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_boxShape, m_camera);
	shapeEntity->setPosition(0.0f, 4.99f, -250.0f);
	shapeEntity->setScale(500.0f, 50.0f, 1.0f);
	shapeEntity->OnOctreeSet(m_octree);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->setTextureIndex(0);

	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_boxShape, m_camera);
	shapeEntity->setPosition(250.0f, 4.99f, 0.0f);
	shapeEntity->setScale(1.0f, 50.0f, 500.0f);
	shapeEntity->setTextureIndex(0);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);
	
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_boxShape, m_camera);
	shapeEntity->setPosition(-250.0f, 4.99f, 0.0f);
	shapeEntity->setScale(1.0f, 50.0f, 500.0f);
	shapeEntity->setTextureIndex(0);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);
	
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_cylinderShape, m_camera);
	shapeEntity->setPosition(-52.0564f, -20.7696f, -80.7397f);
	shapeEntity->setScale(50.0f, 30.0f, 50.0f);
	shapeEntity->setOrientation(90.0f, 0.0f, 0.0f);
	shapeEntity->setTextureIndex(1);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);
	
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_boxShape, m_camera);
	shapeEntity->setPosition(-20.1763f, -7.45501f, 3.0711f);
	shapeEntity->setScale(20.0f, 30.0f, 20.0f);
	shapeEntity->setOrientation(0.0f, 0.0f, 0.642788f, 0.766044f);
	shapeEntity->setTextureIndex(1);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);

	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_cylinderShape, m_camera);
	shapeEntity->setPosition(131.649f, 8.14253f, 22.008f);
	shapeEntity->setScale(20.0f, 30.0f, 20.0f);
	shapeEntity->setTextureIndex(1);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);

	shapeEntity = m_root->addChild<ShapeEntity, Shape>(m_cylinderShape, m_camera);
	shapeEntity->setPosition(-166.368f, 8.14253f, 22.008f);
	shapeEntity->setScale(20.0f, 30.0f, 20.0f);
	shapeEntity->setTextureIndex(1);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("texture_view"));
	shapeEntity->OnOctreeSet(m_octree);

	m_shipEntity = m_root->addChild<ShapeEntity, Shape>(m_shipShape, m_camera);
	m_shipEntity->setPosition(-15.8898f, 1.49614f, -22.0555f);
	m_shipEntity->setName("ship");
	m_shipEntity->setMaterialIndex(0);
	m_shipEntity->setShader(Globals::shaderManager.getAssetPointer("material"));
	m_shipEntity->OnOctreeSet(m_octree);

	m_hoverBike = m_root->addChild<HoverBike, Shape>(m_hoverbikeShape, m_camera);
	m_hoverBike->SceneNodeLC::setPosition(80.0f, 2.0f, 40.0f);
	m_hoverBike->setMaterialIndex(0);
	m_hoverBike->setShader(Globals::shaderManager.getAssetPointer("material"));
	m_hoverBike->OnOctreeSet(m_octree);

	//ShapeNode* shapeNode;
	shapeEntity = m_root->addChild<ShapeEntity, Shape>(Globals::shapeManager.get("cube"), m_camera);
	shapeEntity->setPosition(85.3823f, 8.0f, 18.6991f);
	shapeEntity->setName("box");
	shapeEntity->setMaterialIndex(0);
	shapeEntity->setDisabled(true);
	shapeEntity->setShader(Globals::shaderManager.getAssetPointer("material"));
	shapeEntity->OnOctreeSet(m_octree);

	SceneNodeLC* sceneNode;
	m_splinePath = new SplinePath();
	Vector3f offset = Vector3f(98.7807f, 8.42104f, 26.7421f);

	//sceneNode = m_root->addChild<SceneNodeLC>();
	//sceneNode->setPosition(Vector3f(-199.976f, 0.0f, 46.0545f) + offset);
	//m_splinePath->AddControlPoint(sceneNode, 0);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-15.8898f, 0.0f, -22.0555f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 1);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-5.88043f, 0.0f, 86.5049f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 2);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-21.2097f, 0.0f, 120.285f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 3);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-90.2015f, 0.0f, 142.828f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 4);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-157.816f, 0.0f, 99.5992f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 5);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-199.976f, 0.0f, 46.0545f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 6);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-199.976f, 0.0f, - 71.1696f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 7);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-156.093f, 2.0274f, -106.743f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 8);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-49.0508f, 0.0f, -106.743f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 9);

	sceneNode = m_root->addChild<SceneNodeLC>();
	sceneNode->setPosition(Vector3f(-15.8898f, 0.0f, -22.0555f) + offset);
	m_splinePath->AddControlPoint(sceneNode, 10);

	m_splinePath->SetControlledNode(m_root->findChild<ShapeEntity>("box"));
	m_splinePath->SetInterpolationMode(CATMULL_ROM_FULL_CURVE);
	m_splinePath->SetSpeed(30.0f);
}

void SixDegreeOfFreedom::createPhysics() {
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(0.0f, 0.0f, 0.0f)), new btBoxShape(btVector3(500.0f, 1.0f, 500.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(0.0f, 4.99f, 250.0f)), Physics::CreateCollisionShape(&m_boxShape, btVector3(500.0f, 50.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(0.0f, 4.99f, -250.0f)), Physics::CreateCollisionShape(&m_boxShape, btVector3(500.0f, 50.0f, 1.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(250.0f, 4.99f, 0.0f)), Physics::CreateCollisionShape(&m_boxShape, btVector3(1.0f, 50.0f, 500.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-250.0f, 4.99f, 0.0f)), Physics::CreateCollisionShape(&m_boxShape, btVector3(1.0f, 50.0f, 500.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);

	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(131.649f, 8.14253f, 22.008f)), Physics::CreateCollisionShape(&m_cylinderShape, btVector3(20.0f, 30.0f, 20.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-166.368f, 8.14253f, 22.008f)), Physics::CreateCollisionShape(&m_cylinderShape, btVector3(20.0f, 30.0f, 20.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-52.0564f, -20.7696f, -80.7397f), btQuaternion(0.0f, 90.0f * PI_ON_180, 0.0f)), Physics::CreateCollisionShape(&m_cylinderShape, btVector3(50.0f, 30.0f, 50.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-20.1763f, -7.45501f, 3.0711f), btQuaternion(0.0f, 0.0f, 0.642788f, 0.766044f)), Physics::CreateCollisionShape(&m_boxShape, btVector3(20.0f, 30.0f, 20.0f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::PICKABLE_OBJECT, btCollisionObject::CF_STATIC_OBJECT);
	
	m_kinematicBox = Physics::AddKinematicRigidBody(Physics::BtTransform(btVector3(85.3823f, 8.0f, 18.6991f)), new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));
	m_shipBody = Physics::AddRigidBody(1.0f, Physics::BtTransform(btVector3(85.3823f, 8.0f, 18.6991f)), Physics::CreateConvexHullShape(&m_shipShape), Physics::PICKABLE_OBJECT, Physics::collisiontypes::FLOOR | Physics::MOUSEPICKER);
	//m_shipBody = Physics::AddRigidBody(1.0f, Physics::BtTransform(btVector3(85.3823f, 8.0f, 18.6991f)), new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)), Physics::PICKABLE_OBJECT, Physics::collisiontypes::FLOOR | Physics::MOUSEPICKER);
	m_shipBody->setDamping(0.7f, m_shipBody->getAngularDamping());
	m_shipBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	m_shipEntity->setPosition(Physics::VectorFrom(m_shipBody->getWorldTransform().getOrigin()));
	m_shipEntity->setOrientation(Physics::QuaternionFrom(m_shipBody->getWorldTransform().getRotation()));

	btGeneric6DofSpring2Constraint* pGen6DOFSpring = new btGeneric6DofSpring2Constraint(*m_kinematicBox, *m_shipBody, Physics::BtTransform(), Physics::BtTransform());
	pGen6DOFSpring->setLinearLowerLimit(btVector3(-8.0f, -4.0f, -0.2f));
	pGen6DOFSpring->setLinearUpperLimit(btVector3(8.0f, 4.0f, 0.2f));
	pGen6DOFSpring->setAngularLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
	pGen6DOFSpring->setAngularUpperLimit(btVector3(0.0f, 0.0f, 0.0f));

	pGen6DOFSpring->enableSpring(D6_LINEAR_X, true);
	pGen6DOFSpring->setStiffness(D6_LINEAR_X, 0.75f);
	pGen6DOFSpring->setDamping(D6_LINEAR_X, 1.0f);

	pGen6DOFSpring->enableSpring(D6_LINEAR_Z, true);
	pGen6DOFSpring->setStiffness(D6_LINEAR_Z, 0.75f);
	pGen6DOFSpring->setDamping(D6_LINEAR_Z, 1.0f);

	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 0);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 1);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_ERP, 0.8f, 5);

	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 0);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 1);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_ERP, 0.2f, 5);

	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 0);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 1);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_CFM, 0.8f, 5);

	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 0);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 1);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 2);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 3);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 4);
	pGen6DOFSpring->setParam(BT_CONSTRAINT_STOP_CFM, 0.2f, 5);

	pGen6DOFSpring->setDbgDrawSize(btScalar(5.f));
	Physics::GetDynamicsWorld()->addConstraint(pGen6DOFSpring, true);
	

}

void SixDegreeOfFreedom::updateSplinePath(float timeStep){
	if (m_splinePath){
		m_splinePath->Move(timeStep);

		// Looped path, reset to continue
		if (m_splinePath->IsFinished()){
			m_splinePath->Reset();
		}

		// Orient the control node in the path direction
		if (m_splinePath->GetControlledNode()){
			Vector3f curPos = m_splinePath->GetPosition();
			float traveled = m_splinePath->GetTraveled() + 0.02f;
			if (traveled > 1.0f){
				traveled -= 1.0f;
			}
			Vector3f aheadPos = m_splinePath->GetPoint(traveled);
			m_direction = aheadPos - curPos;
			m_direction[1] = 0.0f;
			Vector3f::Normalize(m_direction);
			m_splinePath->GetControlledNode()->setOrientation(Quaternion(m_direction));
			m_splinePath->GetControlledNode()->setOrientation(Quaternion(Vector3f::FORWARD, m_direction));
			m_camera.setDirection(m_direction);
		}
	}
}

void SixDegreeOfFreedom::pickObject(const btVector3& pickPos, const btCollisionObject* hitObj) {
	Keyboard &keyboard = Keyboard::instance();

	btRigidBody* body = (btRigidBody*)btRigidBody::upcast(hitObj);
	if (body) {

		if (!(body->isStaticObject() || body->isKinematicObject())) {
			pickedBody = body;
			pickedBody->setActivationState(DISABLE_DEACTIVATION);
			btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
			if (keyboard.keyDown(Keyboard::KEY_RSHIFT) || keyboard.keyDown(Keyboard::KEY_LSHIFT)) {
				btTransform tr;
				tr.setIdentity();
				tr.setOrigin(localPivot);
				btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body, tr, false);
				dof6->setLinearLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
				dof6->setLinearUpperLimit(btVector3(0.0f, 0.0f, 0.0f));
				dof6->setAngularLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
				dof6->setAngularUpperLimit(btVector3(0.0f, 0.0f, 0.0f));

				Physics::GetDynamicsWorld()->addConstraint(dof6, true);
				m_pickConstraint = dof6;

				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 0);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 1);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 2);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 3);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 4);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8f, 5);

				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 0);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 1);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 2);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 3);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 4);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1f, 5);
			}
			else {
				btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
				Physics::GetDynamicsWorld()->addConstraint(p2p, true);
				m_pickConstraint = p2p;
				p2p->m_setting.m_impulseClamp = mousePickClamping;
				p2p->m_setting.m_tau = 0.001f;
			}
		}
	}

}

void SixDegreeOfFreedom::removePickingConstraint() {
	if (m_pickConstraint && Physics::GetDynamicsWorld()) {
		Physics::GetDynamicsWorld()->removeConstraint(m_pickConstraint);
		delete m_pickConstraint;

		m_pickConstraint = 0;
		pickedBody->forceActivationState(ACTIVE_TAG);
		pickedBody->setDeactivationTime(0.0f);
		pickedBody = 0;
	}
}