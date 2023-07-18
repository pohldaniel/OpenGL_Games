#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Urho3D::Vector<Urho3D::String> arguments;

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
	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();

	indexBuffer.clear();
	indexBuffer.shrink_to_fit();


	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	Urho3D::VariantMap engineParameters_ = Urho3D::Engine::ParseParameters(arguments);

	engine = new Urho3D::Engine(context_);
	engine->Initialize(engineParameters_);
	/*context_->RegisterSubsystem(new Urho3D::FileSystem(context_));
	context_->RegisterSubsystem(new Urho3D::ResourceCache(context_));
	RegisterSceneLibrary(context_);
	context_->RegisterSubsystem(new Urho3D::Graphics(context_));
	context_->RegisterSubsystem(new Urho3D::Renderer(context_));
	context_->RegisterSubsystem(new Urho3D::Time(context_));*/

	Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

	renderer = GetSubsystem<Urho3D::Renderer>();
	graphics = GetSubsystem<Urho3D::Graphics>();

	scene_ = new Urho3D::Scene(context_);
	node = new Urho3D::Node(context_);
	Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();
	Urho3D::XMLFile *xmlLevel = cache->GetResource<Urho3D::XMLFile>("res/playGroundTest.xml");

	scene_->LoadXML(xmlLevel->GetRoot());
	//Urho3D::Node* movingPlatNode = scene_->GetChild("movingPlatformDisk1", true);

	//Urho3D::Vector3 pos = movingPlatNode->GetWorldPosition();

	//Urho3D::StringHash strHash = Urho3D::StringHash("StaticModel");
	//Urho3D::SharedPtr<Urho3D::StaticModel> staticModel = Urho3D::DynamicCast<Urho3D::StaticModel>(context_->CreateObject(strHash));


	//Urho3D::Model* model = cache->GetResource<Urho3D::Model>("res/Models/disk.mdl");
	//const Urho3D::BoundingBox& bb = model->GetBoundingBox();
	

	Urho3D::Model* model = new Urho3D::Model(context_);
	Urho3D::SharedPtr<Urho3D::File> file(new Urho3D::File(context_, "res/Models/disk.mdl"));
	model->Load(*(file.Get()));
	geometry = model->GetGeometry(0, 0);

	//mldConverter.mdlToObj("res/Models/base.mdl", "res/base.obj", "res/base.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/basePlat.mdl", "res/basePlat.obj", "res/basePlat.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/disk.mdl", "res/disk.obj", "res/disk.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/disk1.mdl", "res/disk1.obj", "res/disk1.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/Lift.mdl", "res/lift.obj", "res/lift.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/Lift1.mdl", "res/lift1.obj", "res/lift1.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/LiftButton.mdl", "res/liftButton.obj", "res/liftButton.mtl", "/textures/ProtoWhite256.jpg");

	//mldConverter.mdlToObj("res/Models/liftExterior.mdl", "res/liftExterior.obj", "res/liftExterior.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/liftExterior1.mdl", "res/liftExterior1.obj", "res/liftExterior1.mtl", "/textures/ProtoWhite256.jpg");

	//mldConverter.mdlToObj("res/Models/pCube1.mdl", "res/pCube1.obj", "res/pCube1.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCube2.mdl", "res/pCube2.obj", "res/pCube2.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCube3.mdl", "res/pCube3.obj", "res/pCube3.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCube4.mdl", "res/pCube4.obj", "res/pCube4.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCube5.mdl", "res/pCube5.obj", "res/pCube5.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCube6.mdl", "res/pCube6.obj", "res/pCube6.mtl", "/textures/ProtoWhite256.jpg");

	//mldConverter.mdlToObj("res/Models/pCylinder1.mdl", "res/pCylinder1.obj", "res/pCylinder1.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCylinder2.mdl", "res/pCylinder2.obj", "res/pCylinder2.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/pCylinder3.mdl", "res/pCylinder3.obj", "res/pCylinder3.mtl", "/textures/ProtoWhite256.jpg");

	//mldConverter.mdlToObj("res/Models/ramp.mdl", "res/ramp.obj", "res/ramp.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/ramp2.mdl", "res/ramp2.obj", "res/ramp2.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/ramp3.mdl", "res/ramp3.obj", "res/ramp3.mtl", "/textures/ProtoWhite256.jpg");

	//mldConverter.mdlToObj("res/Models/upperFloor.mdl", "res/upperFloor.obj", "res/upperFloor.mtl", "/textures/ProtoWhite256.jpg");
	//mldConverter.mdlToObj("res/Models/upperFloor1.mdl", "res/upperFloor1.obj", "res/upperFloor1.mtl", "/textures/ProtoWhite256.jpg");

	
	mldConverter.mdlToBuffer("res/Models/disk.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_disk.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/Lift.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_lift.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/LiftButton.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftButton.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/base.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_base.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/liftExterior.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftExterior.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/upperFloor.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_upperFloor.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/ramp.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
	
	mldConverter.mdlToBuffer("res/Models/ramp2.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp2.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/ramp3.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp3.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mldConverter.mdlToBuffer("res/Models/Cylinder.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_cylinder.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
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

	Globals::textureManager.get("proto").bind(0);

	shader = Globals::shaderManager.getAssetPointer("mdl");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	
	shader->loadMatrix("u_model", Matrix4f::Translate(35.5938f, 0.350185f, 10.4836f));
	m_lift.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(35.5938f, 0.412104f, 10.4836f));
	m_liftButton.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	m_base.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(35.6211f, 7.66765f, 10.4388f));
	m_liftExterior.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(30.16f, 6.98797f, 10.0099f));
	m_upperFloor.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(13.5771f, 6.23965f, 10.9272));
	m_ramp.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(-22.8933f, 2.63165f, -23.6786));
	m_ramp2.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(-15.2665f, 1.9782f, -43.135));
	m_ramp3.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(26.1357f, 7.00645f, -34.7563f));
	m_disk.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(-0.294956f, 2.48167f, 28.3161f));
	m_cylinder.drawRaw();

	shader->loadMatrix("u_model", Matrix4f::Translate(4.14317f, 7.00645f, 35.1134f));
	m_disk.drawRaw();

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