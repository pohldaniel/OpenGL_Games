#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	//setup the camera.
	camera = Camera(Vector3f(0.0f, 0.0f, 20.f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	camera.perspective(45.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 1.0f, 5000.0f);

	//setup some meshes
	car = new Model();
	car->loadObject("res/car/car.obj");

	wheel[0] = new Model();
	wheel[0]->loadObject("res/wheel/wheel.obj");

	wheel[1] = new Model();
	wheel[1]->loadObject("res/wheel/wheel.obj");

	wheel[2] = new Model();
	wheel[2]->loadObject("res/wheel/wheel.obj");

	wheel[3] = new Model();
	wheel[3]->loadObject("res/wheel/wheel.obj");

	room = new Model();
	room->loadObject("res/room/room.obj", Vector3f(0.0, 0.0, 1.0), 0.0, Vector3f(0.0, 0.0, 0.0), 0.05f);


	Globals::physics->AddCar(PhysicsUtils::btTransFrom(btVector3(0.2f, 18.95f, 0.7f), btQuaternion(btVector3(0, 1, 0), -PI / 2.f))); // 0,2,5

	Globals::physics->AddStaticModel(Physics::CreateStaticCollisionShapes2(room, 1), PhysicsUtils::btTransFrom(btVector3(0, 0, 0)), false, btVector3(1, 1, 1));
}

Game::~Game() {}

void Game::fixedUpdate() {
	Globals::physics->StepSimulation(m_fdt);
};

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyDown(Keyboard::KEY_W) || keyboard.keyDown(Keyboard::KEY_UP)) {
		Globals::physics->GetCar()->Forward();
	}

	if (keyboard.keyDown(Keyboard::KEY_S) || keyboard.keyDown(Keyboard::KEY_DOWN)) {
		Globals::physics->GetCar()->Backward();
	}

	if (keyboard.keyDown(Keyboard::KEY_A) || keyboard.keyDown(Keyboard::KEY_LEFT)) {
		Globals::physics->GetCar()->TurnLeft();
	}

	if (keyboard.keyDown(Keyboard::KEY_D) || keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		Globals::physics->GetCar()->TurnRight();
	}
};

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glm::mat4 carMatrix = glm::scale(PhysicsUtils::glmMat4From(Globals::physics->GetCar()->GetWorldTransform()), glm::vec3(CAR_SCALE));

	if (followCamera){
		btVector3 vel = Globals::physics->GetCar()->GetVehicle()->getRigidBody()->getLinearVelocity();
		camera2.Follow(carMatrix, glm::vec3(vel.x(), vel.y(), vel.z()), m_dt);
	}

	for (int i = 0; i < Globals::physics->GetCar()->GetVehicle()->getNumWheels(); i++){
		Globals::physics->GetCar()->GetVehicle()->updateWheelTransform(i, true); //synchronize the wheels with the (interpolated) chassis worldtransform        
		glm::mat4 wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(Globals::physics->GetCar()->GetVehicle()->getWheelInfo(i).m_worldTransform), glm::vec3(CAR_SCALE));

		if (i == PhysicsCar::WHEEL_FRONTRIGHT || i == PhysicsCar::WHEEL_REARRIGHT)
			wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));
	}
	
	// pohledova a projekcni matice - pro kresleni stinovych teles
	glm::mat4 mView = camera2.GetMatrix();
	glm::mat4 mPerspective = glm::perspective(45.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 1000.0f);

	camera.m_viewMatrix[0][0] = mView[0][0]; camera.m_viewMatrix[0][1] = mView[0][1]; camera.m_viewMatrix[0][2] = mView[0][2]; camera.m_viewMatrix[0][3] = mView[0][3];
	camera.m_viewMatrix[1][0] = mView[1][0]; camera.m_viewMatrix[1][1] = mView[1][1]; camera.m_viewMatrix[1][2] = mView[1][2]; camera.m_viewMatrix[1][3] = mView[1][3];
	camera.m_viewMatrix[2][0] = mView[2][0]; camera.m_viewMatrix[2][1] = mView[2][1]; camera.m_viewMatrix[2][2] = mView[2][2]; camera.m_viewMatrix[2][3] = mView[2][3];
	camera.m_viewMatrix[3][0] = mView[3][0]; camera.m_viewMatrix[3][1] = mView[3][1]; camera.m_viewMatrix[3][2] = mView[3][2]; camera.m_viewMatrix[3][3] = mView[3][3];

	camera.m_projMatrix[0][0] = mPerspective[0][0]; camera.m_projMatrix[0][1] = mPerspective[0][1]; camera.m_projMatrix[0][2] = mPerspective[0][2]; camera.m_projMatrix[0][3] = mPerspective[0][3];
	camera.m_projMatrix[1][0] = mPerspective[1][0]; camera.m_projMatrix[1][1] = mPerspective[1][1]; camera.m_projMatrix[1][2] = mPerspective[1][2]; camera.m_projMatrix[1][3] = mPerspective[1][3];
	camera.m_projMatrix[2][0] = mPerspective[2][0]; camera.m_projMatrix[2][1] = mPerspective[2][1]; camera.m_projMatrix[2][2] = mPerspective[2][2]; camera.m_projMatrix[2][3] = mPerspective[2][3];
	camera.m_projMatrix[3][0] = mPerspective[3][0]; camera.m_projMatrix[3][1] = mPerspective[3][1]; camera.m_projMatrix[3][2] = mPerspective[3][2]; camera.m_projMatrix[3][3] = mPerspective[3][3];


	car->setModelMatrix(Matrix4f(carMatrix[0][0], carMatrix[0][1], carMatrix[0][2], carMatrix[0][3],
		carMatrix[1][0], carMatrix[1][1], carMatrix[1][2], carMatrix[1][3],
		carMatrix[2][0], carMatrix[2][1], carMatrix[2][2], carMatrix[2][3],
		carMatrix[3][0], carMatrix[3][1], carMatrix[3][2], carMatrix[3][3]));


	Globals::physics->GetCar()->GetVehicle()->updateWheelTransform(0, true); //synchronize the wheels with the (interpolated) chassis worldtransform
	glm::mat4 wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(Globals::physics->GetCar()->GetVehicle()->getWheelInfo(0).m_worldTransform), glm::vec3(CAR_SCALE));

	if (0 == PhysicsCar::WHEEL_FRONTRIGHT || 0 == PhysicsCar::WHEEL_REARRIGHT)
		wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

	wheel[0]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
		wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
		wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
		wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

	Globals::physics->GetCar()->GetVehicle()->updateWheelTransform(1, true);
	wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(Globals::physics->GetCar()->GetVehicle()->getWheelInfo(1).m_worldTransform), glm::vec3(CAR_SCALE));

	if (1 == PhysicsCar::WHEEL_FRONTRIGHT || 1 == PhysicsCar::WHEEL_REARRIGHT)
		wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

	wheel[1]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
		wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
		wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
		wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

	Globals::physics->GetCar()->GetVehicle()->updateWheelTransform(2, true);
	wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(Globals::physics->GetCar()->GetVehicle()->getWheelInfo(2).m_worldTransform), glm::vec3(CAR_SCALE));

	if (2 == PhysicsCar::WHEEL_FRONTRIGHT || 2 == PhysicsCar::WHEEL_REARRIGHT)
		wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

	wheel[2]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
		wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
		wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
		wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

	Globals::physics->GetCar()->GetVehicle()->updateWheelTransform(3, true);
	wheelMatrix = glm::scale(PhysicsUtils::glmMat4From(Globals::physics->GetCar()->GetVehicle()->getWheelInfo(3).m_worldTransform), glm::vec3(CAR_SCALE));

	if (3 == PhysicsCar::WHEEL_FRONTRIGHT || 3 == PhysicsCar::WHEEL_REARRIGHT)
		wheelMatrix = glm::rotate(wheelMatrix, 180.f, glm::vec3(0.f, 1.f, 0.f));

	wheel[3]->setModelMatrix(Matrix4f(wheelMatrix[0][0], wheelMatrix[0][1], wheelMatrix[0][2], wheelMatrix[0][3],
		wheelMatrix[1][0], wheelMatrix[1][1], wheelMatrix[1][2], wheelMatrix[1][3],
		wheelMatrix[2][0], wheelMatrix[2][1], wheelMatrix[2][2], wheelMatrix[2][3],
		wheelMatrix[3][0], wheelMatrix[3][1], wheelMatrix[3][2], wheelMatrix[3][3]));

	car->draw(camera);
	room->draw(camera);
	wheel[0]->draw(camera);
	wheel[1]->draw(camera);
	wheel[2]->draw(camera);
	wheel[3]->draw(camera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}