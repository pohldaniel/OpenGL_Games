#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "VehicleInterface.h"
#include "Application.h"
#include "Globals.h"

VehicleInterface::VehicleInterface(StateMachine& machine) : State(machine, CurrentState::RAGDOLLINTERFACE),
m_pickConstraint(0) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	ShapeDrawer::Get().init(8192);
	ShapeDrawer::Get().setCamera(m_camera);
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	/*btCollisionShape* groundShape = new btBoxShape(btVector3(200.0f, 10.0f, 200.0f));
	m_collisionShapes.push_back(groundShape);
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0.0f, -10.0f, 0.0f));

	btCollisionObject* fixedGround = new btCollisionObject();
	fixedGround->setCollisionShape(groundShape);
	fixedGround->setWorldTransform(groundTransform);
	Physics::GetDynamicsWorld()->addCollisionObject(fixedGround, Physics::FLOOR, Physics::PICKABLE_OBJECT);*/

	btTransform tr;
	tr.setIdentity();
	
	int i;

	const float TRIANGLE_SIZE = 20.f;

	//create a triangle-mesh ground
	int vertStride = sizeof(btVector3);
	int indexStride = 3 * sizeof(int);

	const int NUM_VERTS_X = 20;
	const int NUM_VERTS_Y = 20;
	const int totalVerts = NUM_VERTS_X * NUM_VERTS_Y;

	const int totalTriangles = 2 * (NUM_VERTS_X - 1)*(NUM_VERTS_Y - 1);

	m_vertices = new btVector3[totalVerts];
	int* gIndices = new int[totalTriangles * 3];



	for (i = 0; i<NUM_VERTS_X; i++) {
		for (int j = 0; j<NUM_VERTS_Y; j++) {
			float wl = .2f;
			//height set to zero, but can also use curved landscape, just uncomment out the code
			float height = 20.f*sinf(float(i)*wl)*cosf(float(j)*wl);

			m_vertices[i + j*NUM_VERTS_X].setValue( (i - NUM_VERTS_X*0.5f)*TRIANGLE_SIZE, height, (j - NUM_VERTS_Y*0.5f)*TRIANGLE_SIZE);

			
		}
	}

	int index = 0;
	for (i = 0; i<NUM_VERTS_X - 1; i++){
		for (int j = 0; j<NUM_VERTS_Y - 1; j++){
			gIndices[index++] = j*NUM_VERTS_X + i;
			gIndices[index++] = j*NUM_VERTS_X + i + 1;
			gIndices[index++] = (j + 1)*NUM_VERTS_X + i + 1;

			gIndices[index++] = j*NUM_VERTS_X + i;
			gIndices[index++] = (j + 1)*NUM_VERTS_X + i + 1;
			gIndices[index++] = (j + 1)*NUM_VERTS_X + i;
		}
	}

	m_indexVertexArrays = new btTriangleIndexVertexArray(totalTriangles, gIndices, indexStride, totalVerts, (btScalar*)&m_vertices[0].x(), vertStride);

	bool useQuantizedAabbCompression = true;
	btCollisionShape* groundShape = new btBvhTriangleMeshShape(m_indexVertexArrays, useQuantizedAabbCompression);

	tr.setOrigin(btVector3(0, -4.5f, 0));

	m_collisionShapes.push_back(groundShape);

	//create ground object
	localCreateRigidBody(0, tr, groundShape);
	tr.setOrigin(btVector3(0, 0, 0));//-64.5f,0));

	btCollisionShape* chassisShape = new btBoxShape(btVector3(1.f, 0.5f, 2.f));
	m_collisionShapes.push_back(chassisShape);

	btCompoundShape* compound = new btCompoundShape();
	m_collisionShapes.push_back(compound);
	btTransform localTrans;
	localTrans.setIdentity();
	//localTrans effectively shifts the center of mass with respect to the chassis
	localTrans.setOrigin(btVector3(0, 1, 0));

	compound->addChildShape(localTrans, chassisShape);

	tr.setOrigin(btVector3(0, 0.f, 0));

	m_carChassis = localCreateRigidBody(800, tr, compound);//chassisShape);
														   //m_carChassis->setDamping(0.2,0.2);

	m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

	//clientResetScene();

	/// create vehicle
	{

		m_vehicleRayCaster = new btDefaultVehicleRaycaster(Physics::GetDynamicsWorld());
		m_vehicle = new btRaycastVehicle(m_tuning, m_carChassis, m_vehicleRayCaster);

		///never deactivate the vehicle
		m_carChassis->setActivationState(DISABLE_DEACTIVATION);

		Physics::GetDynamicsWorld()->addVehicle(m_vehicle);

		float connectionHeight = 1.2f;


		bool isFrontWheel = true;


		m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

		btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - (0.3*wheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - wheelRadius);
		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
		connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3*wheelWidth), connectionHeight, 2 * CUBE_HALF_EXTENTS - wheelRadius);
		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
		connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + (0.3*wheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + wheelRadius);
		isFrontWheel = false;
		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
		connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - (0.3*wheelWidth), connectionHeight, -2 * CUBE_HALF_EXTENTS + wheelRadius);
		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
		for (int i = 0; i<m_vehicle->getNumWheels(); i++) {
			btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
			wheel.m_suspensionStiffness = suspensionStiffness;
			wheel.m_wheelsDampingRelaxation = suspensionDamping;
			wheel.m_wheelsDampingCompression = suspensionCompression;
			wheel.m_frictionSlip = wheelFriction;
			wheel.m_rollInfluence = rollInfluence;
		}
	}
}

VehicleInterface::~VehicleInterface() {
	Globals::physics->removeAllCollisionObjects();
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void VehicleInterface::fixedUpdate() {
	Globals::physics->stepSimulation(m_fdt);
}

void VehicleInterface::update() {
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

void VehicleInterface::render() {

	btScalar m[16];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ShapeDrawer::Get().drawDynmicsWorld(Physics::GetDynamicsWorld());

	for (int i = 0; i<m_vehicle->getNumWheels(); i++){
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i, true);
		//draw wheels (cylinders)
		m_vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(m);
		ShapeDrawer::Get().drawShape(m, m_wheelShape);
	}

	m_mousePicker.drawPicker(m_camera);

	if (m_drawUi)
		renderUi();
}

void VehicleInterface::OnMouseMotion(Event::MouseMoveEvent& event) {
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

void VehicleInterface::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void VehicleInterface::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_mousePicker.updatePosition(event.x, event.y, m_camera);

	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
	else if (event.button == 1u) {

		if (m_mousePicker.click(event.x, event.y, m_camera)) {
			const MousePickCallback& callback = m_mousePicker.getCallback();
			pickObject(callback.m_hitPointWorld, callback.m_collisionObject);
		}
	}
}

void VehicleInterface::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().detach();
	}
	else if (event.button == 1u) {
		removePickingConstraint();
	}
}

void VehicleInterface::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = true;
		Mouse::instance().detach();
		Keyboard::instance().disable();
	}
}

void VehicleInterface::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

}


void VehicleInterface::renderUi() {
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


void VehicleInterface::pickObject(const btVector3& pickPos, const btCollisionObject* hitObj) {
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
				dof6->setLinearLowerLimit(btVector3(0, 0, 0));
				dof6->setLinearUpperLimit(btVector3(0, 0, 0));
				dof6->setAngularLowerLimit(btVector3(0, 0, 0));
				dof6->setAngularUpperLimit(btVector3(0, 0, 0));

				Physics::GetDynamicsWorld()->addConstraint(dof6, true);
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

void VehicleInterface::removePickingConstraint() {
	if (m_pickConstraint && Physics::GetDynamicsWorld()) {
		Physics::GetDynamicsWorld()->removeConstraint(m_pickConstraint);
		delete m_pickConstraint;

		m_pickConstraint = 0;
		pickedBody->forceActivationState(ACTIVE_TAG);
		pickedBody->setDeactivationTime(0.f);
		pickedBody = 0;
	}
}

btRigidBody* VehicleInterface::localCreateRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape)
{
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

void VehicleInterface::clientResetScene()
{
	gVehicleSteering = 0.f;
	m_carChassis->setCenterOfMassTransform(btTransform::getIdentity());
	m_carChassis->setLinearVelocity(btVector3(0, 0, 0));
	m_carChassis->setAngularVelocity(btVector3(0, 0, 0));
	Physics::GetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_carChassis->getBroadphaseHandle(), Physics::GetDynamicsWorld()->getDispatcher());
	if (m_vehicle)
	{
		m_vehicle->resetSuspension();
		for (int i = 0; i<m_vehicle->getNumWheels(); i++)
		{
			//synchronize the wheels with the (interpolated) chassis worldtransform
			m_vehicle->updateWheelTransform(i, true);
		}
	}

}