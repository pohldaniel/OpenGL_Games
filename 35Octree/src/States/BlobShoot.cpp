#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/BuiltInShader.h>
#include <Physics/ShapeDrawer.h>

#include "BlobShoot.h"
#include "Application.h"
#include "Globals.h"

BlobShoot::BlobShoot(StateMachine& machine) : State(machine, States::BLOBSHOOT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 30.0f), Vector3f(0.0f, 0.0f, 30.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(50.0f);
	m_camera.setOffsetDistance(m_offsetDistance);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	m_background.resize(Application::Width, Application::Height);
	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_sceneBuffer.create(Application::Width, Application::Height);
	m_sceneBuffer.attachTexture2D(AttachmentTex::RGBA);
	m_sceneBuffer.attachTexture2D(AttachmentTex::RED32F);
	m_sceneBuffer.attachTexture2D(AttachmentTex::DEPTH24);

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);

	addBox(Vector3f(0, -BOX_SIZE / 2, 0), Vector3f(BOX_SIZE, TINY_SIZE, BOX_SIZE));
	addBox(Vector3f(0, +BOX_SIZE / 2, 0), Vector3f(BOX_SIZE, TINY_SIZE, BOX_SIZE));
	addBox(Vector3f(BOX_SIZE / 2, 0, 0), Vector3f(TINY_SIZE, BOX_SIZE, BOX_SIZE));
	addBox(Vector3f(-BOX_SIZE / 2, 0, 0), Vector3f(TINY_SIZE, BOX_SIZE, BOX_SIZE));
	addBox(Vector3f(0, 0, -BOX_SIZE / 2), Vector3f(BOX_SIZE, BOX_SIZE, TINY_SIZE));
	addBox(Vector3f(0, 0, +BOX_SIZE / 2), Vector3f(BOX_SIZE, BOX_SIZE, TINY_SIZE));

	addCharacter(Vector3f(0.0f, 0.0f, 0.0f), Vector2f(0.5f, 1.5f));

	glGenBuffers(1, &BuiltInShader::sphereUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::sphereUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SphereStruct) * MAX_SPHERE_COUNT, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 4, BuiltInShader::sphereUbo, 0, sizeof(SphereStruct) * MAX_SPHERE_COUNT);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

BlobShoot::~BlobShoot() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void BlobShoot::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);

}

void BlobShoot::update() {
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

	if (direction.lengthSq() > 0.0f)
		Vector3f::Normalize(direction);

	direction = m_camera.getViewSpaceDirection(direction);
	m_kinematicController->setWalkDirection(Physics::VectorFrom(direction * 0.4f));

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}
	}

	m_camera.Camera::setTarget(getPosition());
	m_camera.moveRelative(Vector3f(0.0f, 1.5f, 0.0f));

	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_LEFT) && m_spheres.size() < MAX_SPHERE_COUNT) {
		btRigidBody* sphere = addSphere(m_camera.getPosition() + Vector3f(0.0f, 3.0f, 0.0f), 6.0f, 5.0, Physics::collisiontypes::SPHERE, Physics::collisiontypes::FLOOR | Physics::collisiontypes::SPHERE);
		btVector3 direction = Physics::VectorFrom(m_camera.getViewDirection() * SPHERE_RADIUS * SPHERE_RADIUS * 20.0f);
		sphere->applyCentralImpulse(direction);
	}

	m_background.update(m_dt);

	for (int i = 0; i < m_bodies.size(); i++) {
		btRigidBody* sphereBody = m_bodies[i];
		SphereStruct& sphere = m_spheres[i];

		sphere.position = getPosition(sphereBody);
		sphere.orientaion = getOrientation(sphereBody);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::sphereUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SphereStruct) * m_spheres.size(), m_spheres.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void BlobShoot::render() {

	m_sceneBuffer.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_background.draw();
	//glClearTexImage(m_sceneBuffer.getColorTexture(1), 0, GL_RED, GL_FLOAT, maxDistance);
	glClearBufferfv(GL_COLOR, 1, maxDistance);
	auto shader = Globals::shaderManager.getAssetPointer("scene");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::Translate(2.0f, 0.0f, 0.0f));
	shader->loadFloat("u_near", m_camera.getNear());
	shader->loadFloat("u_far", m_camera.getFar());

	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::textureManager.get("marble").bind();
	Globals::shapeManager.get("cube").drawRaw();

	//shader->loadVector("u_color", Vector4f(0.25098039215686274f, 0.25098039215686274f, 0.25098039215686274f, 1.0f));
	//shader->loadMatrix("u_model", Matrix4f::Translate(0.0f, -30.0f, 0.0f));
	//Globals::textureManager.get("null").bind();
	//Globals::shapeManager.get("floor").drawRaw();

	shader->unuse();

	

	if (m_debugCollision) {
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	m_sceneBuffer.unbind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	shader = Globals::shaderManager.getAssetPointer("ray_march_new");
	shader->use();
	shader->loadVector("u_campos", m_camera.getPosition());
	shader->loadVector("u_viewdir", m_camera.getViewDirection());
	shader->loadVector("u_camright", m_camera.getCamX());
	shader->loadVector("u_camup", m_camera.getCamY());
	shader->loadFloat("u_scaleFactor", m_camera.getScaleFactor());
	shader->loadFloat("u_aspectRatio", m_camera.getAspect());
	shader->loadUnsignedInt("u_sphereCount", static_cast<unsigned int>(m_spheres.size()));

	shader->loadInt("u_screen_texture", 0);
	shader->loadInt("u_depth_texture", 1);

	m_sceneBuffer.bindColorTexture(0u, 0u);
	m_sceneBuffer.bindColorTexture(1u, 1u);

	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();

	glEnable(GL_DEPTH_TEST);
	if (m_drawUi)
		renderUi();
}

void BlobShoot::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void BlobShoot::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void BlobShoot::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void BlobShoot::OnMouseWheel(Event::MouseWheelEvent& event) {
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

void BlobShoot::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void BlobShoot::OnKeyUp(Event::KeyboardEvent& event) {

}

void BlobShoot::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_sceneBuffer.resize(Application::Width, Application::Height);
}

void BlobShoot::renderUi() {
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
	ImGui::Checkbox("Debug Collision", &m_debugCollision);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

btRigidBody* BlobShoot::addSphere(const Vector3f& pos, float rad, float mass, int collisionFilterGroup, int collisionFilterMask) {
	m_bodies.push_back(Physics::AddRigidBody(mass, Physics::BtTransform(Physics::VectorFrom(pos)), new btSphereShape(rad * 0.25), collisionFilterGroup, collisionFilterMask, btCollisionObject::CF_DYNAMIC_OBJECT));
	m_bodies.back()->setRestitution(1.0f);
	m_spheres.push_back({ pos , 0.0f, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, rad });
	return m_bodies.back();
}

btCollisionObject* BlobShoot::addBox(const Vector3f& pos, const Vector3f& size) {
	btCollisionObject* box = Physics::AddStaticObject(Physics::BtTransform(Physics::VectorFrom(pos)), new btBoxShape(Physics::VectorFrom(size * 0.5f)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::SPHERE);
	box->setRestitution(0.5f);
	box->setFriction(10.0f); //may clamped maybe not?
	return box;
}

void BlobShoot::addCharacter(const Vector3f& pos, const Vector2f& size) {
	
	m_ghostPairCallback = new btGhostPairCallback();
	Physics::GetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);


	btConvexShape* capsule = new btCapsuleShape(size[0], size[1]);

	m_pairCachingGhostObject = new btPairCachingGhostObject();
	m_pairCachingGhostObject->setCollisionShape(capsule);
	m_pairCachingGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	
	m_kinematicController = new btKinematicCharacterController(m_pairCachingGhostObject, static_cast<btConvexShape*>(m_pairCachingGhostObject->getCollisionShape()), 0.35f, btVector3(0.0f, 1.0f, 0.0f));


	m_pairCachingGhostObject->setWorldTransform(Physics::BtTransform(Physics::VectorFrom(pos)));


	Physics::GetDynamicsWorld()->addCollisionObject(m_pairCachingGhostObject, Physics::collisiontypes::CHARACTER, Physics::collisiontypes::FLOOR);
	Physics::GetDynamicsWorld()->addAction(m_kinematicController);
}

const Vector3f BlobShoot::getPosition() {
	btTransform t = m_pairCachingGhostObject->getWorldTransform();
	return Physics::VectorFrom(t.getOrigin()) - m_colShapeOffset;
}

const Vector3f BlobShoot::getPosition(btRigidBody* body) {
	btTransform t = body->getWorldTransform();
	return Physics::VectorFrom(t.getOrigin()) - m_colShapeOffset;
}

const Quaternion BlobShoot::getOrientation(btRigidBody* body) {
	btTransform t = body->getWorldTransform();
	return Physics::QuaternionFrom(t.getRotation());
}