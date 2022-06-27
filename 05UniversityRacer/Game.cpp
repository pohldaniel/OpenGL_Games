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

	chair = new Model();
	chair->loadObject("res/chairs/chairs.obj", Vector3f(0.0, 0.0, 1.0), 0.0, Vector3f(0, 0, 0), 0.05f);

	middesk = new Model();
	middesk->loadObject("res/desk-mid/desk-mid.obj", Vector3f(0.0, 0.0, 1.0), 0.0, Vector3f(0, 0, 0), 0.05f);

	sidedesk = new Model();
	sidedesk->loadObject("res/desk-side/desk-side.obj", Vector3f(0.0, 0.0, 1.0), 0.0, Vector3f(0, 0, 0), 0.05f);

	Globals::physics->AddCar(PhysicsUtils::btTransFrom(btVector3(36.2f, 8.95f, -21.7f), btQuaternion(btVector3(0, 1, 0), -PI / 2.f))); // 0,2,5
	Globals::physics->AddStaticModel(Physics::CreateStaticCollisionShapes2(room, 1.0f), PhysicsUtils::btTransFrom(btVector3(0, 0, 0)), false, btVector3(1, 1, 1));

	glm::mat4 rows[] = {
		glm::translate(glm::mat4(1.0), glm::vec3(-740, 19, -70)),
		glm::translate(glm::mat4(1.0), glm::vec3(-740, 39, -170)),
		glm::translate(glm::mat4(1.0), glm::vec3(-740, 59, -270)),
		glm::translate(glm::mat4(1.0), glm::vec3(-740, 79, -370)),
		glm::translate(glm::mat4(1.0), glm::vec3(-740, 99, -470))
	};

	std::vector<btCollisionShape*> chairShapes = Physics::CreateStaticCollisionShapes2(chair, 1.0f);

	for (unsigned int rowI = 0; rowI < 5; rowI++){
		int offsetX = 0; // posunuti zidle na radku

		for (unsigned int i = 0; i < 13; i++){
			if (i == 3 || i == 10)
				offsetX += 100;

			glm::mat4 col = glm::translate(rows[rowI], glm::vec3(offsetX, 0, 0));
			col[3][0] = col[3][0] * 0.05f;
			col[3][1] = col[3][1] * 0.05f;
			col[3][2] = col[3][2] * 0.05f;

			chairs.push_back(Matrix4f(col[0][0], col[0][1], col[0][2], col[0][3],
									col[1][0], col[1][1], col[1][2], col[1][3], 
									col[2][0], col[2][1], col[2][2], col[2][3], 
									col[3][0], col[3][1], col[3][2], col[3][3]));

			//container->queueDraw(chairs, col); // jen testovaci; ulozi se index na posledni pridanou zidli
			offsetX += 105;

			Globals::physics->AddStaticModel(chairShapes, PhysicsUtils::btTransFrom(col), false);
		}
	}
	chair->createInstances(chairs);
	
	glm::mat4 rows2[] = {
		glm::translate(glm::mat4(1.0), glm::vec3(-365, 13, -43)),
		glm::translate(glm::mat4(1.0), glm::vec3(-365, 33, -143)),
		glm::translate(glm::mat4(1.0), glm::vec3(-365, 53, -243)),
		glm::translate(glm::mat4(1.0), glm::vec3(-365, 73, -343)),
		glm::translate(glm::mat4(1.0), glm::vec3(-365, 93, -443))
	};

	std::vector<btCollisionShape*> middeskShapes = Physics::CreateStaticCollisionShapes2(middesk, 1.0f);

	for (unsigned int rowI = 0; rowI < 5; rowI++){
		glm::mat4 col = glm::translate(rows2[rowI], glm::vec3(0, 0, 0));
		col[3][0] = col[3][0] * 0.05f;
		col[3][1] = col[3][1] * 0.05f;
		col[3][2] = col[3][2] * 0.05f;

		middesks.push_back(Matrix4f(col[0][0], col[0][1], col[0][2], col[0][3],
									col[1][0], col[1][1], col[1][2], col[1][3],
									col[2][0], col[2][1], col[2][2], col[2][3],
									col[3][0], col[3][1], col[3][2], col[3][3]));

		Globals::physics->AddStaticModel(middeskShapes, PhysicsUtils::btTransFrom(col), false);

		//container->queueDraw(middesk, col);
		//shadowVolumes->addModel(middesk, col);
	}
	middesk->createInstances(middesks);

	glm::mat4 rows3[] = {
		glm::translate(glm::mat4(1.0), glm::vec3(-785,  20, -15)),
		glm::translate(glm::mat4(1.0), glm::vec3(-785,  40, -115)),
		glm::translate(glm::mat4(1.0), glm::vec3(-785,  60, -215)),
		glm::translate(glm::mat4(1.0), glm::vec3(-785,  80, -315)),
		glm::translate(glm::mat4(1.0), glm::vec3(-785, 100, -415))
	};

	std::vector<btCollisionShape*> sidedeskShapes = Physics::CreateStaticCollisionShapes2(sidedesk, 1.0f);

	glm::vec3 otherside(1250 * 0.05f, 0, 0);

	for (unsigned int rowI = 0; rowI < 5; rowI++){
		// leva strana (z pohledu z katedry)
		glm::mat4 col = glm::translate(rows3[rowI], glm::vec3(0, 0, 0));
		col[3][0] = col[3][0] * 0.05f;
		col[3][1] = col[3][1] * 0.05f;
		col[3][2] = col[3][2] * 0.05f;

		sidedesks.push_back(Matrix4f(col[0][0], col[0][1], col[0][2], col[0][3],
			col[1][0], col[1][1], col[1][2], col[1][3],
			col[2][0], col[2][1], col[2][2], col[2][3],
			col[3][0], col[3][1], col[3][2], col[3][3]));


		//container->queueDraw(sidedesk, col);

		Globals::physics->AddStaticModel(sidedeskShapes, PhysicsUtils::btTransFrom(col), false);

		// prava strana
		glm::mat4 mat = glm::translate(col, otherside);
		sidedesks.push_back(Matrix4f(
			mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]));


		//container->queueDraw(sidedesk, mat);
		//shadowVolumes->addModel(sidedesk, mat);

		Globals::physics->AddStaticModel(sidedeskShapes, PhysicsUtils::btTransFrom(mat), false);
	}

	sidedesk->createInstances(sidedesks);
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
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
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

	room->draw(camera);
	car->draw(camera);
	
	wheel[0]->draw(camera);
	wheel[1]->draw(camera);
	wheel[2]->draw(camera);
	wheel[3]->draw(camera);

	chair->drawInstanced(camera);
	middesk->drawInstanced(camera);
	sidedesk->drawInstanced(camera);
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}