#include <random>
#include "Game.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 100.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);
Vector3f LIGHT_DIRECTION(-100.0f, 100.0f, -100.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_skybox(SkyBox(m_dt, m_fdt, 2500.0f)) {

	m_quadShader = Globals::shaderManager.getAssetPointer("quad");
	m_quadShadow = Globals::shaderManager.getAssetPointer("quad_shadow");
	m_copyFramebuffer.create(WIDTH, HEIGHT);
	m_copyFramebuffer.attachTexture(Framebuffer::Attachments::COLOR);
	m_copyFramebuffer.attachRenderbuffer(Framebuffer::Attachments::DEPTH_STENCIL);

	m_leftBottom = new Quad(Vector2f(-1.0f, -1.0f), size);
	m_rightBottom = new Quad(Vector2f(1.0f - size[0], -1.0f), size);
	m_quad = new Quad(Vector2f(-1.0f, -1.0f), Vector2f(2.0f, 2.0f));

	offsetX = WIDTH * size[0] * 0.5f;
	offsetY = HEIGHT * size[1] * 0.5f;

	m_albedoFramebuffer.create(ALBEDO_TEXTURE_SIZE, ALBEDO_TEXTURE_SIZE);
	m_albedoFramebuffer.attachTexture(Framebuffer::Attachments::COLOR);

	m_depthFramebuffer.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	m_depthFramebuffer.attachTexture(Framebuffer::Attachments::RED);
	m_depthFramebuffer.attachTexture(Framebuffer::Attachments::DEPTH16F);

	Vector3f pos;
	pos[0] = 150.0f;
	pos[2] = 0.0f;
	pos[1] = 50.0f;

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 1.0f, 5000.0f);
	m_camera.lookAt(pos, Vector3f(0.0f, 30.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setAcceleration(CAMERA_ACCELERATION);
	m_camera.setVelocity(CAMERA_VELOCITY);
	m_camera.setRotationSpeed(0.1f);

	m_teapot.loadModel("res/models/teapot/teapot_smooth.obj", true);
	m_teapot.initAssets(Globals::shaderManager, Globals::textureManager);
	m_teapot.getTransform().setRotPosScale(Vector3f(1.0f, 0.0f, 0.0f), -90.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);

	std::vector<btCollisionShape*> teapotShape = Physics::CreateStaticCollisionShapes(&m_teapot, 1.0f);
	btRigidBody* body = Globals::physics->addStaticModel(teapotShape, Physics::BtTransform(), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT | Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::RAY);

	initTexture(Globals::textureManager.get("null"));
}

Game::~Game() {}

void Game::fixedUpdate() {

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

	if (keyboard.keyPressed(Keyboard::KEY_N)) {
		m_skybox.toggleDayNight();
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		initTexture(Globals::textureManager.get("null"));
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debug = !m_debug;
	}

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_LEFT)) {

		float mouseXndc = (2.0f * mouse.xPosAbsolute()) / (float)WIDTH - 1.0f;
		float mouseYndc = 1.0f - (2.0f * mouse.yPosAbsolute()) / (float)HEIGHT;

		Vector4f rayStartEye = Vector4f(mouseXndc, mouseYndc, -1.0f, 1.0f) ^ Globals::invProjection;
		Vector4f rayEndEye = Vector4f(mouseXndc, mouseYndc, 1.0f, 1.0f) ^ Globals::invProjection;
		rayEndEye = rayEndEye * (1.0f / rayEndEye[3]);


		Vector3f rayStartWorld = rayStartEye * m_camera.getInvViewMatrix();
		Vector3f rayEndWorld = rayEndEye * m_camera.getInvViewMatrix();

		Vector3f rayDirection = rayEndWorld - rayStartWorld;
		Vector3f::Normalize(rayDirection);

		btVector3 origin = btVector3(rayStartWorld[0], rayStartWorld[1], rayStartWorld[2]);
		btVector3 target = btVector3(rayEndWorld[0], rayEndWorld[1], rayEndWorld[2]);

		RayResultCallback callback(origin, target, Physics::collisiontypes::RAY, Physics::collisiontypes::PICKABLE_OBJECT);

		Globals::physics->GetDynamicsWorld()->rayTest(origin, target, callback);
		if (callback.hasHit()) {

			Vector3f pos = Vector3f(callback.m_hitPointWorld[0], callback.m_hitPointWorld[1], callback.m_hitPointWorld[2]);
			Vector3f normal = Vector3f(callback.m_hitNormalWorld[0], callback.m_hitNormalWorld[1], callback.m_hitNormalWorld[2]);

			m_projector_pos = pos + normal * PROJECTOR_BACK_OFF_DISTANCE;
			m_projector_dir = normal;

			float ratio = 64.0f / 64.0f;
			float proportionate_height = m_projector_size * ratio;

			std::random_device                    rd;
			std::mt19937                          gen(rd());
			std::uniform_real_distribution<float> scale_dis(5.0, 20.0);
			std::uniform_real_distribution<float> rotation_dis(-90.0, 90.0);

			m_projector_size = scale_dis(gen);
			m_projector_rotation = rotation_dis(gen);

			Matrix4f rot;
			rot.rotate(m_projector_dir, m_projector_rotation);

			Vector3f rotated_axis = rot * Vector3f(0.0f, 1.0f, 0.0f);

			m_projector_view.lookAt(m_projector_pos, pos, rotated_axis);
			m_projector_proj.orthographic(-m_projector_size, m_projector_size, -proportionate_height, proportionate_height, 0.1f, 5000.0f);
			renderDepthMap();
			applyDecals();
		}
	}

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {

		// rotate camera
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);

		} // end if any rotation

		if (move) {
			//float speed = 1.3f;
			m_camera.updatePosition(directrion, m_dt);
		}

	}// end if any movement

	m_skybox.update();
};


void Game::render(unsigned int &frameBuffer) {

	shdowPass();
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("mesh");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", Globals::projection);
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	shader->loadMatrix("u_projectionShadows", m_projector_view);
	shader->loadMatrix("u_viewShadows", m_projector_proj);

	shader->loadInt("u_texture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedoFramebuffer.getColorTexture());

	m_teapot.drawRaw();
	glUseProgram(0);

	m_skybox.render(m_camera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (m_debug) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_copyFramebuffer.getFramebuffer());
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_SCISSOR_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		glScissor(0, 0, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_quadShader->m_program);
		m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_leftBottom->render(m_albedoFramebuffer.getColorTexture());
		glUseProgram(0);

		glScissor(WIDTH - offsetX, 0, offsetX, offsetY);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(m_quadShadow->m_program);
		m_quadShadow->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_rightBottom->render(m_depthFramebuffer.getColorTexture());
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_SCISSOR_TEST);
	}
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Game::applyDecals() {
	glEnable(GL_BLEND);

	m_albedoFramebuffer.bind();

	auto shader = Globals::shaderManager.getAssetPointer("decals");
	glUseProgram(shader->m_program);
	shader->loadMatrix("decal_view_proj", m_projector_proj * m_projector_view);
	shader->loadVector("cam_pos", m_camera.getPosition());
	shader->loadInt("s_Decal", 0);
	Globals::textureManager.get("boots").bind(0);
	shader->loadInt("s_Depth", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_depthFramebuffer.getDepthTexture());

	m_teapot.drawRaw();
	glUseProgram(0);

	m_albedoFramebuffer.unbind();

	glDisable(GL_BLEND);
}

void Game::renderDepthMap() {
	glEnable(GL_DEPTH_TEST);
	m_depthFramebuffer.bind();

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("depth_dec");
	glUseProgram(shader->m_program);
	shader->loadMatrix("decal_view_proj", m_projector_proj * m_projector_view);
	m_teapot.drawRaw();
	glUseProgram(0);

	m_depthFramebuffer.unbind();
}

void Game::shdowPass() {
	glEnable(GL_DEPTH_TEST);
	m_depthFramebuffer.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("depth_dec");
	glUseProgram(shader->m_program);
	shader->loadMatrix("decal_view_proj", Globals::projection * m_camera.getViewMatrix());
	m_teapot.drawRaw();
	glUseProgram(0);
	m_depthFramebuffer.unbind();
}

void Game::initTexture() {
	glDisable(GL_BLEND);

	glDisable(GL_CULL_FACE);
	m_albedoFramebuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture_init");
	glUseProgram(shader->m_program);
	shader->loadMatrix("view_proj", Globals::projection * m_camera.getViewMatrix());

	m_teapot.drawRaw();
	glUseProgram(0);

	m_albedoFramebuffer.unbind();
}

void Game::initTexture(Texture& texture) {
	glDisable(GL_BLEND);

	glDisable(GL_CULL_FACE);
	m_albedoFramebuffer.bind();
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("quad");
	glUseProgram(shader->m_program);
	m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	m_quad->render(texture.getTexture());
	glUseProgram(0);

	m_albedoFramebuffer.unbind();
}