#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"



Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_pickConstraint(0), Object(new Urho3D::Context()) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 5.0f), Vector3f(0.0f, 2.0f, 5.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(0.0f);
	applyTransformation(m_trackball);

	//solidConverter.solidToObj("res/solid/Sword.solid", "res/Sword.obj", "res/sword.mtl", "/textures/Sword.jpg", false);
	//solidConverter.solidToObj("res/solid/Body.solid", "res/Rabbit.obj", "res/rabbit.mtl", "/textures/FurBlackWhite.jpg");

	solidConverter.solidToBuffer("res/solid/Sword.solid", false, vertexBuffer, indexBuffer);
	m_sword.fromBuffer(vertexBuffer, indexBuffer, 5);

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();

	indexBuffer.clear();
	indexBuffer.shrink_to_fit();

	solidConverter.solidToBuffer("res/solid/Body.solid", true, vertexBuffer, indexBuffer);
	m_rabbit.fromBuffer(vertexBuffer, indexBuffer, 5);

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	context_->RegisterSubsystem(new Urho3D::FileSystem(context_));
	context_->RegisterSubsystem(new Urho3D::ResourceCache(context_));
	RegisterSceneLibrary(context_);

	scene_ = new Urho3D::Scene(context_);
	node = new Urho3D::Node(context_);

	Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
	Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();
	Urho3D::XMLFile *xmlLevel = cache->GetResource<Urho3D::XMLFile>("res/playGroundTest.xml");

	scene_->LoadXML(xmlLevel->GetRoot());
	Urho3D::Node* movingPlatNode = scene_->GetChild("movingPlatformDisk1", true);

	Urho3D::Vector3 pos = movingPlatNode->GetWorldPosition();
	std::cout << pos.x_ << "  " << pos.y_ << "  " << pos.z_ << std::endl;
}

Game::~Game() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
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
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
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
}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix() * Matrix4f::Scale(0.001f, 0.001f, 0.001f));
	Globals::textureManager.get("sword").bind(0);

	m_sword.drawRaw();
	
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix() * Matrix4f::Rotate(Vector3f(1.0f, 0.0f, 0.0f), 180.0f) * Matrix4f::Scale(0.01f, 0.01f, 0.01f));
	Globals::textureManager.get("fur").bind(0);

	m_rabbit.drawRaw();
	shader->unuse();

	m_mousePicker.drawPicker(m_camera);

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);

	m_mousePicker.updatePosition(event.x, event.y, m_camera);

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

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_mousePicker.updatePosition(event.x, event.y, m_camera);

	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}else if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);

		if (m_mousePicker.click(event.x, event.y, m_camera)) {
			m_mousePicker.setHasPicked(true);
			const MousePickCallback& callback = m_mousePicker.getCallback();
			pickObject(callback.m_hitPointWorld, callback.m_collisionObject);
		}
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}else if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);

		m_mousePicker.setHasPicked(false);
		removePickingConstraint();
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
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
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

void Game::pickObject(const btVector3& pickPos, const btCollisionObject* hitObj) {
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

void Game::removePickingConstraint() {
	if (m_pickConstraint && Physics::GetDynamicsWorld()) {
		Physics::GetDynamicsWorld()->removeConstraint(m_pickConstraint);
		delete m_pickConstraint;

		m_pickConstraint = 0;
		pickedBody->forceActivationState(ACTIVE_TAG);
		pickedBody->setDeactivationTime(0.0f);
		pickedBody = 0;
	}
}

Urho3D::StringHash Game::GetType() const { 
	return GetTypeInfoStatic()->GetType(); 
}

const Urho3D::String& Game::GetTypeName() const { 
	return GetTypeInfoStatic()->GetTypeName(); 
}

const Urho3D::TypeInfo* Game::GetTypeInfo() const { 
	return GetTypeInfoStatic(); 
}