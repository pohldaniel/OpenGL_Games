#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_pickConstraint(0), m_player(m_camera) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 5.0f), Vector3f(0.0f, 2.0f, 5.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setOffsetDistance(10.0f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(0.0f);
	applyTransformation(m_trackball);

	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);

	//solidConverter.solidToObj("res/solid/Sword.solid", "res/Sword.obj", "res/sword.mtl", "/textures/Sword.jpg", false);
	//solidConverter.solidToObj("res/solid/Body.solid", "res/Rabbit.obj", "res/rabbit.mtl", "/textures/FurBlackWhite.jpg");

	solidConverter.solidToBuffer("res/solid/Sword.solid", false, vertexBuffer, indexBuffer);
	m_sword.fromBuffer(vertexBuffer, indexBuffer, 5);
	vertexBuffer.clear();vertexBuffer.shrink_to_fit();indexBuffer.clear();indexBuffer.shrink_to_fit();

	solidConverter.solidToBuffer("res/solid/Body.solid", true, vertexBuffer, indexBuffer);
	m_rabbit.fromBuffer(vertexBuffer, indexBuffer, 5);
	vertexBuffer.clear();vertexBuffer.shrink_to_fit();indexBuffer.clear();indexBuffer.shrink_to_fit();

	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

	//mdlConverter.mdlToObj("res/Models/base.mdl", "res/base.obj", "res/base.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/basePlat.mdl", "res/basePlat.obj", "res/basePlat.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/disk.mdl", "res/disk.obj", "res/disk.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/disk1.mdl", "res/disk1.obj", "res/disk1.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/Lift.mdl", "res/lift.obj", "res/lift.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/Lift1.mdl", "res/lift1.obj", "res/lift1.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/LiftButton.mdl", "res/liftButton.obj", "res/liftButton.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/liftExterior.mdl", "res/liftExterior.obj", "res/liftExterior.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/liftExterior1.mdl", "res/liftExterior1.obj", "res/liftExterior1.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCube1.mdl", "res/pCube1.obj", "res/pCube1.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCube2.mdl", "res/pCube2.obj", "res/pCube2.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCube3.mdl", "res/pCube3.obj", "res/pCube3.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCube4.mdl", "res/pCube4.obj", "res/pCube4.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCube5.mdl", "res/pCube5.obj", "res/pCube5.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCube6.mdl", "res/pCube6.obj", "res/pCube6.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCylinder1.mdl", "res/pCylinder1.obj", "res/pCylinder1.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCylinder2.mdl", "res/pCylinder2.obj", "res/pCylinder2.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/pCylinder3.mdl", "res/pCylinder3.obj", "res/pCylinder3.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/ramp.mdl", "res/ramp.obj", "res/ramp.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/ramp2.mdl", "res/ramp2.obj", "res/ramp2.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/ramp3.mdl", "res/ramp3.obj", "res/ramp3.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/upperFloor.mdl", "res/upperFloor.obj", "res/upperFloor.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/upperFloor1.mdl", "res/upperFloor1.obj", "res/upperFloor1.mtl", "/textures/ProtoWhite256.jpg");
	//mdlConverter.mdlToObj("res/Models/Cylinder.mdl", "res/Cylinder.obj", "res/cylinder.mtl", "/textures/ProtoWhite256.jpg");

	mdlConverter.mdlToBuffer("res/Models/disk.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_disk.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/Lift.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_lift.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/LiftButton.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftButton.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/base.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_base.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear();vertexBuffer.shrink_to_fit();indexBuffer.clear();indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/liftExterior.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_liftExterior.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/upperFloor.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_upperFloor.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/ramp.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();
	
	mdlConverter.mdlToBuffer("res/Models/ramp2.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp2.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/ramp3.mdl", 0.01f, vertexBuffer, indexBuffer);
	m_ramp3.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	mdlConverter.mdlToBuffer("res/Models/Cylinder.mdl", 6.0f, vertexBuffer, indexBuffer);
	m_cylinder.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit();

	m_player.init();

	m_player.setPosition(Vector3f(0.0f, 5.0f, 0.0f));
	m_player.resetOrientation();

	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_lift, 1.0f), Physics::BtTransform(btVector3(35.5938f, 0.350185f, 10.4836f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);
	//Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(35.5938f, 0.412104f + 0.15f, 10.4836f)), new btCylinderShape(btVector3(80.0f, 15.0f, 1.0f) * 0.01f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA, btCollisionObject::CF_STATIC_OBJECT);
	//Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(35.5938f, 0.933648f, 10.4836f)), new btCylinderShape(btVector3(90.0f, 80.0f, 1.0f) * 0.01f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(0.0f, -0.05f, 0.0f)), new btConvexHullShape((btScalar*)(&m_base.getPositions()[0]), m_base.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA, btCollisionObject::CF_STATIC_OBJECT);

	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_disk, 1.0f), Physics::BtTransform(btVector3(26.1357f, 7.00645f, -34.7563f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);
	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_disk, 1.0f), Physics::BtTransform(btVector3(4.14317f, 7.00645f, 35.1134f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);

	
	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_liftExterior, 1.0f), Physics::BtTransform(btVector3(35.6211f, 7.66765f, 10.4388f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);
	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_upperFloor, 1.0f), Physics::BtTransform(btVector3(30.16f, 6.98797f, 10.0099f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);
	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_ramp, 1.0f), Physics::BtTransform(btVector3(13.5771f, 6.23965f, 10.9272f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);	
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-22.8933f, 2.63165f, -23.6786f)), new btConvexHullShape((btScalar*)(&m_ramp2.getPositions()[0]), m_ramp2.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA, btCollisionObject::CF_STATIC_OBJECT);
	Physics::AddRigidBody(0.0f, Physics::BtTransform(btVector3(-15.2665f, 1.9782f, -43.135f)), new btConvexHullShape((btScalar*)(&m_ramp3.getPositions()[0]), m_ramp3.getPositions().size(), 3 * sizeof(btScalar)), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA, btCollisionObject::CF_STATIC_OBJECT);

	Globals::physics->addStaticModel(Physics::CreateCollisionShapes(&m_cylinder, 1.0f), Physics::BtTransform(btVector3(-0.294956f, 2.48167f, 28.3161f)), false, btVector3(1.0f, 1.0f, 1.0f), Physics::collisiontypes::FLOOR, Physics::collisiontypes::CHARACTER | Physics::collisiontypes::CAMERA);
	m_pLiftButton.create(new btCylinderShape(btVector3(80.0f, 15.0f, 1.0f) * 0.01f), Physics::BtTransform(btVector3(35.5938f, 0.412104f + 0.15f, 10.4836f)), Physics::GetDynamicsWorld(), Physics::collisiontypes::TRIGGER, Physics::collisiontypes::CHARACTER);

}

Game::~Game() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {
	m_player.getCharacterController()->preStep();
	Globals::physics->stepSimulation(m_fdt);
	m_player.getCharacterController()->postStep();
	m_player.fixedUpdate(m_fdt);

	m_liftButtonTriggerResult.buttonOffset = 0.0f;
	Physics::GetDynamicsWorld()->contactPairTest(m_player.getContactObject(), m_pLiftButton.getCollisionObject(), m_liftButtonTriggerResult);

	buttonOffset = m_liftButtonTriggerResult.buttonOffset;
}

void Game::update() {

	m_player.update(m_dt);

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_4)) {
		m_debugPhysic = !m_debugPhysic;
	}

	/*Vector3f direction = Vector3f();

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
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);

		}

		if (move) {
			m_camera.move(direction * 5.0f * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());*/

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debugPhysic = !m_debugPhysic;
	}


}

void Game::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_debugPhysic) {
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());

	}else {

		Globals::textureManager.get("proto").bind(0);

		auto shader = Globals::shaderManager.getAssetPointer("mdl");
		shader->use();
		shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
		shader->loadMatrix("u_view", m_camera.getViewMatrix());

		shader->loadMatrix("u_model", Matrix4f::Translate(35.5938f, 0.350185f, 10.4836f));
		m_lift.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(35.5938f, 0.412104f - buttonOffset, 10.4836f));
		m_liftButton.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::IDENTITY);
		m_base.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(35.6211f, 7.66765f, 10.4388f));
		m_liftExterior.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(30.16f, 6.98797f, 10.0099f));
		m_upperFloor.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(13.5771f, 6.23965f, 10.9272f));
		m_ramp.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(-22.8933f, 2.63165f, -23.6786f));
		m_ramp2.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(-15.2665f, 1.9782f, -43.135f));
		m_ramp3.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(26.1357f, 7.00645f, -34.7563f));
		m_disk.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(4.14317f, 7.00645f, 35.1134f));
		m_disk.drawRaw();

		shader->loadMatrix("u_model", Matrix4f::Translate(-0.294956f, 2.48167f, 28.3161f));
		m_cylinder.drawRaw();

		shader->unuse();
		m_player.draw(m_camera);
	}



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

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow());
		Keyboard::instance().enable();
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {

	if (event.keyCode == VK_SPACE) {
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
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);

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

btScalar LiftButtonTriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	buttonOffset = 0.15f;
	return 0;
}