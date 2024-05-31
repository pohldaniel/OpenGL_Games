#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/DebugRenderer.h>
#include <Physics/ShapeDrawer.h>
#include <States/Menu.h>

#include "ConstraintInterface.h"
#include "Application.h"
#include "Globals.h"

#define THETA SIMD_PI/4.f
#define L_1 (2 - tan(THETA))
#define L_2 (1 / cos(THETA))
#define RATIO L_2 / L_1
#define SIMD_PI_2 ((SIMD_PI)*0.5f)
#define SIMD_PI_4 ((SIMD_PI)*0.25f)
#define CUBE_HALF_EXTENTS 1.f

ConstraintInterface::ConstraintInterface(StateMachine& machine) : State(machine, States::DEFAULT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-10.0f, 5.0f, 60.0f), Vector3f(-10.0f, 5.0f, 60.0f) +  Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(m_rotationSpeed);
	m_camera.setMovingSpeed(15.0f);
	m_camera.setOffsetDistance(m_offsetDistance);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	DebugRenderer::Get().setEnable(true);
	ShapeDrawer::Get().init(32768);
	ShapeDrawer::Get().setCamera(m_camera);
	Physics::SetDebugMode(btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits);
	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);
	
	m_defaultContactProcessingThreshold = BT_LARGE_FLOAT;

	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 40);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -56, 0));
	btRigidBody* groundBody;
	groundBody = localCreateRigidBody(0, groundTransform, groundShape);



	btCollisionShape* box1 = new btBoxShape(btVector3(CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS));
	btCollisionShape* box2 = new btBoxShape(btVector3(CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS, CUBE_HALF_EXTENTS));
	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(0, 20, 0));

	float mass = 1.f;

	{
		btRigidBody* bodyA = 0;
		btRigidBody* bodyB = 0;
		{
			btCollisionShape* cylA = new btCylinderShape(btVector3(0.2, 0.25, 0.2));
			btCollisionShape* cylB = new btCylinderShape(btVector3(L_1, 0.025, L_1));
			btCompoundShape* cyl0 = new btCompoundShape();
			cyl0->addChildShape(btTransform::getIdentity(), cylA);
			cyl0->addChildShape(btTransform::getIdentity(), cylB);

			btScalar mass = 6.28;
			btVector3 localInertia;
			cyl0->calculateLocalInertia(mass, localInertia);
			btRigidBody::btRigidBodyConstructionInfo ci(mass, 0, cyl0, localInertia);
			ci.m_startWorldTransform.setOrigin(btVector3(-8, 1, -8));

			btRigidBody* body = new btRigidBody(ci);
			Physics::GetDynamicsWorld()->addRigidBody(body);
			body->setLinearFactor(btVector3(0, 0, 0));
			body->setAngularFactor(btVector3(0, 1, 0));
			bodyA = body;
		}

		{
			btCollisionShape* cylA = new btCylinderShape(btVector3(0.2, 0.26, 0.2));
			btCollisionShape* cylB = new btCylinderShape(btVector3(L_2, 0.025, L_2));
			btCompoundShape* cyl0 = new btCompoundShape();
			cyl0->addChildShape(btTransform::getIdentity(), cylA);
			cyl0->addChildShape(btTransform::getIdentity(), cylB);

			btScalar mass = 6.28;
			btVector3 localInertia;
			cyl0->calculateLocalInertia(mass, localInertia);
			btRigidBody::btRigidBodyConstructionInfo ci(mass, 0, cyl0, localInertia);
			ci.m_startWorldTransform.setOrigin(btVector3(-10, 2, -8));


			btQuaternion orn(btVector3(0, 0, 1), -THETA);
			ci.m_startWorldTransform.setRotation(orn);

			btRigidBody* body = new btRigidBody(ci);
			body->setLinearFactor(btVector3(0, 0, 0));
			btHingeConstraint* hinge = new btHingeConstraint(*body, btVector3(0, 0, 0), btVector3(0, 1, 0), true);
			Physics::GetDynamicsWorld()->addConstraint(hinge);
			bodyB = body;
			body->setAngularVelocity(btVector3(0, 3, 0));

			Physics::GetDynamicsWorld()->addRigidBody(body, Physics::PICKABLE_OBJECT, Physics::MOUSEPICKER);
		}

		btVector3	axisA(0, 1, 0);
		btVector3	axisB(0, 1, 0);
		btQuaternion orn(btVector3(0, 0, 1), -THETA);
		btMatrix3x3 mat(orn);
		axisB = mat.getRow(1);

		btGearConstraint* gear = new btGearConstraint(*bodyA, *bodyB, axisA, axisB, RATIO);
		Physics::GetDynamicsWorld()->addConstraint(gear, true);
	}

	
	{ // create a generic 6DOF constraint with springs 

		btTransform tr;
		tr.setIdentity();
		tr.setOrigin(btVector3(btScalar(-20.), btScalar(16.), btScalar(0.)));
		tr.getBasis().setEulerZYX(0, 0, 0);
		btRigidBody* pBodyA = localCreateRigidBody(0.0, tr, box1);
		pBodyA->setActivationState(DISABLE_DEACTIVATION);

		tr.setIdentity();
		tr.setOrigin(btVector3(btScalar(-10.), btScalar(16.), btScalar(0.)));
		tr.getBasis().setEulerZYX(0, 0, 0);
		btRigidBody* pBodyB = localCreateRigidBodyPick(1.0, tr, box2);
		pBodyB->setActivationState(DISABLE_DEACTIVATION);

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(10.), btScalar(0.), btScalar(0.)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.), btScalar(0.), btScalar(0.)));

		btGeneric6DofSpringConstraint* pGen6DOFSpring = new btGeneric6DofSpringConstraint(*pBodyA, *pBodyB, frameInA, frameInB, true);
		pGen6DOFSpring->setLinearUpperLimit(btVector3(5., 0., 0.));
		pGen6DOFSpring->setLinearLowerLimit(btVector3(-5., 0., 0.));

		pGen6DOFSpring->setAngularLowerLimit(btVector3(0.f, 0.f, -1.5f));
		pGen6DOFSpring->setAngularUpperLimit(btVector3(0.f, 0.f, 1.5f));

		Physics::GetDynamicsWorld()->addConstraint(pGen6DOFSpring, true);
		pGen6DOFSpring->setDbgDrawSize(btScalar(5.f));

		pGen6DOFSpring->enableSpring(0, true);
		pGen6DOFSpring->setStiffness(0, 39.478f);
		pGen6DOFSpring->setDamping(0, 0.5f);
		pGen6DOFSpring->enableSpring(5, true);
		pGen6DOFSpring->setStiffness(5, 39.478f);
		pGen6DOFSpring->setDamping(0, 0.3f);
		pGen6DOFSpring->setEquilibriumPoint();
	}

	{ // create a ConeTwist constraint

		btTransform tr;
		tr.setIdentity();
		tr.setOrigin(btVector3(btScalar(-10.), btScalar(5.), btScalar(0.)));
		tr.getBasis().setEulerZYX(0, 0, 0);
		btRigidBody* pBodyA = localCreateRigidBodyPick(1.0, tr, box2);
		pBodyA->setActivationState(DISABLE_DEACTIVATION);

		tr.setIdentity();
		tr.setOrigin(btVector3(btScalar(-10.), btScalar(-5.), btScalar(0.)));
		tr.getBasis().setEulerZYX(0, 0, 0);
		btRigidBody* pBodyB = localCreateRigidBody(0.0, tr, box1);


		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.getBasis().setEulerZYX(0, 0, SIMD_PI_2);
		frameInA.setOrigin(btVector3(btScalar(0.), btScalar(-5.), btScalar(0.)));
		frameInB = btTransform::getIdentity();
		frameInB.getBasis().setEulerZYX(0, 0, SIMD_PI_2);
		frameInB.setOrigin(btVector3(btScalar(0.), btScalar(5.), btScalar(0.)));

		m_ctc = new btConeTwistConstraint(*pBodyA, *pBodyB, frameInA, frameInB);
		m_ctc->setLimit(btScalar(SIMD_PI_4*0.6f), btScalar(SIMD_PI_4), btScalar(SIMD_PI) * 0.8f, 0.5f);
		Physics::GetDynamicsWorld()->addConstraint(m_ctc, true);
		m_ctc->setDbgDrawSize(btScalar(5.f));
	}


	{ // 6DOF connected to the world, with motor
		btTransform tr;
		tr.setIdentity();
		tr.setOrigin(btVector3(btScalar(10.), btScalar(-15.), btScalar(0.)));
		btRigidBody* pBody = localCreateRigidBodyPick(1.0, tr, box2);
		pBody->setActivationState(DISABLE_DEACTIVATION);
		btTransform frameB;
		frameB.setIdentity();
		btGeneric6DofConstraint* pGen6Dof = new btGeneric6DofConstraint(*pBody, frameB, false);
		Physics::GetDynamicsWorld()->addConstraint(pGen6Dof);
		pGen6Dof->setDbgDrawSize(btScalar(5.f));

		pGen6Dof->setAngularLowerLimit(btVector3(0, 0, 0));
		pGen6Dof->setAngularUpperLimit(btVector3(0, 0, 0));
		pGen6Dof->setLinearLowerLimit(btVector3(-10., 0, 0));
		pGen6Dof->setLinearUpperLimit(btVector3(10., 0, 0));

		pGen6Dof->getTranslationalLimitMotor()->m_enableMotor[0] = true;
		pGen6Dof->getTranslationalLimitMotor()->m_targetVelocity[0] = 5.0f;
		pGen6Dof->getTranslationalLimitMotor()->m_maxMotorForce[0] = 2.5f;
	}
}

ConstraintInterface::~ConstraintInterface() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	Physics::SetDebugMode(0u);
}

void ConstraintInterface::fixedUpdate() {

	m_time += 0.03f;
	if (m_testConeTwistMotor){
		btScalar t = 1.25f * m_time;
		btVector3 axis(0, sin(t), cos(t));
		axis.normalize();
		btQuaternion q1(axis, 0.75f*SIMD_PI);
		btQuaternion q2(btVector3(1, 0, 0), -1.49f * btSin(1.5f * m_time));
		q1 = q1 * q2;
		m_ctc->enableMotor(true);
		m_ctc->setMotorTargetInConstraintSpace(q1);
	}

	Globals::physics->stepSimulation(m_fdt);
}

void ConstraintInterface::update() {
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

}

void ConstraintInterface::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DebugRenderer::Get().SetProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());	
	DebugRenderer::Get().drawBuffer();
	

	if (m_debugPhysic) {
		ShapeDrawer::Get().setProjectionView(m_camera.getPerspectiveMatrix(), m_camera.getViewMatrix());

		glDisable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());
		//glPolygonMode(GL_FRONT_AND_BACK, StateMachine::GetEnableWireframe() ? GL_LINE : GL_FILL);
		glEnable(GL_CULL_FACE);
		m_mousePicker.drawPicker(m_camera);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);
		Physics::DebugDrawWorld();
	}

	

	if (m_drawUi)
		renderUi();
}

void ConstraintInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
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

void ConstraintInterface::OnMouseWheel(Event::MouseWheelEvent& event) {
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

void ConstraintInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_mousePicker.updatePosition(event.x, event.y, m_camera);
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}else if (event.button == 1u) {

		if (m_mousePicker.click(event.x, event.y, m_camera)) {
			m_mousePicker.setHasPicked(true);
			const MousePickCallback& callback = m_mousePicker.getCallback();
			pickObject(callback.m_hitPointWorld, callback.m_collisionObject);
		}
	}
}

void ConstraintInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_mousePicker.setHasPicked(false);	
		removePickingConstraint();
	}
	Mouse::instance().detach();
}

void ConstraintInterface::OnKeyDown(Event::KeyboardEvent& event) {
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

void ConstraintInterface::OnKeyUp(Event::KeyboardEvent& event) {

}

void ConstraintInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void ConstraintInterface::renderUi() {
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
	//ImGui::Checkbox("Cone Twist", &m_testConeTwistMotor);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

btRigidBody* ConstraintInterface::localCreateRigidBodyPick(float mass, const btTransform& startTransform, btCollisionShape* shape){
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
	body->setWorldTransform(startTransform);
#endif//

	Physics::GetDynamicsWorld()->addRigidBody(body, Physics::PICKABLE_OBJECT, Physics::MOUSEPICKER);
	return body;
}

btRigidBody* ConstraintInterface::localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape) {
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
	body->setWorldTransform(startTransform);
#endif//

	Physics::GetDynamicsWorld()->addRigidBody(body);
	return body;
}

void ConstraintInterface::pickObject(const btVector3& pickPos, const btCollisionObject* hitObj) {
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

void ConstraintInterface::removePickingConstraint() {
	if (m_pickConstraint && Physics::GetDynamicsWorld()) {
		Physics::GetDynamicsWorld()->removeConstraint(m_pickConstraint);
		delete m_pickConstraint;

		m_pickConstraint = 0;
		pickedBody->forceActivationState(ACTIVE_TAG);
		pickedBody->setDeactivationTime(0.0f);
		pickedBody = 0;
	}
}