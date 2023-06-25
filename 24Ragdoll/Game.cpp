#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), 
m_dynamicsWorld(Physics::GetDynamicsWorld()),
m_pickConstraint(0),
m_modifierKeys(0),
m_ShootBoxInitialSpeed(40.f),
m_stepping(true),
m_singleStep(false),
m_idle(false) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	GLfloat light_ambient[] = { btScalar(0.2), btScalar(0.2), btScalar(0.2), btScalar(1.0) };
	GLfloat light_diffuse[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0) };
	GLfloat light_specular[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0) };
	GLfloat light_position0[] = { btScalar(1.0), btScalar(10.0), btScalar(1.0), btScalar(0.0) };
	GLfloat light_position1[] = { btScalar(-1.0), btScalar(-10.0), btScalar(-1.0), btScalar(0.0) };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(btScalar(0.7f), btScalar(0.7f), btScalar(0.7f), btScalar(0.0f));
	m_shapeDrawer = new GL_ShapeDrawer();
	m_shapeDrawer->enableTexture(true);

	btCollisionShape* groundShape = new btBoxShape(btVector3(200.0f, 10.0f, 200.0f));
	m_collisionShapes.push_back(groundShape);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0.0f, -10.0f, 0.0f));

	btCollisionObject* fixedGround = new btCollisionObject();
	fixedGround->setCollisionShape(groundShape);
	fixedGround->setWorldTransform(groundTransform);
	Physics::GetDynamicsWorld()->addCollisionObject(fixedGround, Physics::FLOOR, Physics::PICKABLE_OBJECT);
	

	// Spawn one ragdoll
	btVector3 startOffset(1.0f, 0.5f, 0.0f);
	spawnRagdoll(startOffset);
	startOffset.setValue(-1.0f, 0.5f, 0.0f);
	spawnRagdoll(startOffset);

	//std::vector<btCollisionShape*> platformShape = Physics::CreateStaticCollisionShapes(&Globals::shapeManager.get("platform"), 1.0f);
	//btRigidBody* body = Globals::physics->addStaticModel(platformShape, Physics::BtTransform(), false, btVector3(1.0f, 1.0f, 1.0f), Physics::FLOOR, Physics::PICKABLE_OBJECT);

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
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getPerspectiveMatrix()[0][0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getViewMatrix()[0][0]);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderme();
	m_mousePicker.drawPicker(m_camera);
	
	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_mousePicker.updatePosition(event.x, event.y, m_camera);

	if (m_pickConstraint) {
		
		btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickConstraint);
		if (pickCon) {
			const MousePickCallback& callback = m_mousePicker.getCallback();
			
			btVector3 newRayTo = callback.m_target;
			btVector3 rayFrom = callback.m_origin;
			btVector3 oldPivotInB = pickCon->getPivotInB();
			btVector3 newPivotB;
				
			
			btVector3 dir = newRayTo - rayFrom;
			dir.normalize();
			dir *= m_mousePicker.getPickingDistance();

			newPivotB = rayFrom + dir;
				
			pickCon->setPivotB(newPivotB);
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

		if (m_mousePicker.click(event.x, event.y, m_camera)) {
			const MousePickCallback& callback = m_mousePicker.getCallback();
			pickObject(callback.m_hitPointWorld, callback.m_collisionObject);
		}
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}else if (event.button == 1u) {
		removePickingConstraint();
	}
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = true;
		Mouse::instance().detach();
		Keyboard::instance().disable();
	}
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	
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

void Game::spawnRagdoll(const btVector3& startOffset) {
	RagDoll* ragDoll = new RagDoll(Physics::GetDynamicsWorld(), startOffset);
	m_ragdolls.push_back(ragDoll);
}

void Game::pickObject(const btVector3& pickPos, const btCollisionObject* hitObj) {

	btRigidBody* body = (btRigidBody*)btRigidBody::upcast(hitObj);
	if (body) {

		if (!(body->isStaticObject() || body->isKinematicObject())) {
			pickedBody = body;
			pickedBody->setActivationState(DISABLE_DEACTIVATION);
			btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
			if ((m_modifierKeys& BT_ACTIVE_SHIFT) != 0) {
				btTransform tr;
				tr.setIdentity();
				tr.setOrigin(localPivot);
				btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body, tr, false);
				dof6->setLinearLowerLimit(btVector3(0, 0, 0));
				dof6->setLinearUpperLimit(btVector3(0, 0, 0));
				dof6->setAngularLowerLimit(btVector3(0, 0, 0));
				dof6->setAngularUpperLimit(btVector3(0, 0, 0));

				m_dynamicsWorld->addConstraint(dof6, true);
				m_pickConstraint = dof6;

				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8, 0);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8, 1);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8, 2);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8, 3);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8, 4);
				dof6->setParam(BT_CONSTRAINT_STOP_CFM, 0.8, 5);

				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1, 0);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1, 1);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1, 2);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1, 3);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1, 4);
				dof6->setParam(BT_CONSTRAINT_STOP_ERP, 0.1, 5);
			}else {
				btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
				m_dynamicsWorld->addConstraint(p2p, true);
				m_pickConstraint = p2p;
				p2p->m_setting.m_impulseClamp = mousePickClamping;
				p2p->m_setting.m_tau = 0.001f;
			}
		}
	}

}

void Game::removePickingConstraint() {
	if (m_pickConstraint && m_dynamicsWorld) {
		m_dynamicsWorld->removeConstraint(m_pickConstraint);
		delete m_pickConstraint;

		m_pickConstraint = 0;
		pickedBody->forceActivationState(ACTIVE_TAG);
		pickedBody->setDeactivationTime(0.f);
		pickedBody = 0;
	}
}

void Game::renderscene(int pass) {
	btScalar	m[16];
	btMatrix3x3	rot; rot.setIdentity();
	const int	numObjects = m_dynamicsWorld->getNumCollisionObjects();
	btVector3 wireColor(1, 0, 0);
	for (int i = 0; i<numObjects; i++)
	{
		btCollisionObject*	colObj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody*		body = btRigidBody::upcast(colObj);
		if (body&&body->getMotionState())
		{
			btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
			myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
			rot = myMotionState->m_graphicsWorldTrans.getBasis();
		}
		else
		{
			colObj->getWorldTransform().getOpenGLMatrix(m);
			rot = colObj->getWorldTransform().getBasis();
		}
		btVector3 wireColor(1.f, 1.0f, 0.5f); //wants deactivation
		if (i & 1) wireColor = btVector3(0.f, 0.0f, 1.f);
		///color differently for active, sleeping, wantsdeactivation states
		if (colObj->getActivationState() == 1) //active
		{
			if (i & 1)
			{
				wireColor += btVector3(1.f, 0.f, 0.f);
			}
			else
			{
				wireColor += btVector3(.5f, 0.f, 0.f);
			}
		}
		if (colObj->getActivationState() == 2) //ISLAND_SLEEPING
		{
			if (i & 1)
			{
				wireColor += btVector3(0.f, 1.f, 0.f);
			}
			else
			{
				wireColor += btVector3(0.f, 0.5f, 0.f);
			}
		}

		btVector3 aabbMin(0, 0, 0), aabbMax(0, 0, 0);
		//m_dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);

		aabbMin -= btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
		aabbMax += btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
				
		switch (pass) {
			case	0:	m_shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax); break;
			case	2:	m_shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor*btScalar(0.3), 0, aabbMin, aabbMax); break;
		}
		
	}
}

void Game::renderme() {

	if (m_dynamicsWorld){
		
		glDisable(GL_CULL_FACE);
		renderscene(0);

		int	xOffset = 10;
		int yStart = 20;
		int yIncr = 20;

		glDisable(GL_LIGHTING);
		glColor3f(0, 0, 0);
	}
}