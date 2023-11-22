#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_cube = new Cube();
	m_quad = new Quad();

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball.getTransform());

	m_fbo.create(Application::Width, Application::Height);
	m_fbo.attachTexture(AttachmentTex::RG16F);

	m_buddha.loadModel("res/models/buddha.obj", Vector3f(1.0f, 0.0f, 0.0f), -90.0f);
	m_dragon.loadModel("res/models/dragon.obj", Vector3f(1.0f, 0.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.f), 10.0f);

	m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	m_shadowView.lookAt(Vector3f(0.0f, 0.0f, -5.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	glEnable(GL_DEPTH_TEST);
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

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);

		}

		if (move) {
			m_camera.move(directrion * 5.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball.getTransform());

};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	shadowPass(m_buddha, m_transformBuddha.getTransformationMatrix());

	auto shader = Globals::shaderManager.getAssetPointer("absorbtion");

	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transformBuddha.getTransformationMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transformBuddha.getTransformationMatrix()));

	shader->loadMatrix("u_projectionShadow", m_shadowProjection);
	shader->loadMatrix("u_viewShadow", m_shadowView);

	shader->loadVector("DiffuseMaterial", Vector3f(0.0f, 0.75f, 0.75f));

	shader->loadInt("Sampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo.getColorTexture());

	m_buddha.drawRaw();
	glUseProgram(0);

	shadowPass(m_dragon, m_transformDragon.getTransformationMatrix());

	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transformDragon.getTransformationMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transformDragon.getTransformationMatrix()));

	shader->loadMatrix("u_projectionShadow", m_shadowProjection);
	shader->loadMatrix("u_viewShadow", m_shadowView);

	shader->loadVector("DiffuseMaterial", Vector3f(0.0f, 0.75f, 0.75f));

	shader->loadInt("Sampler", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo.getColorTexture());

	m_dragon.drawRaw();
	glUseProgram(0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball.getTransform());
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball.getTransform());
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball.getTransform());
}

void Game::applyTransformation(Matrix4f& mtx) {
	m_transformBuddha.fromMatrix(mtx);
	m_transformBuddha.translate(1.0f, 0.0f, 0.0f);

	m_transformDragon.fromMatrix(mtx);
	m_transformDragon.translate(-1.0f, 0.0f, 0.0f);
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
	m_fbo.resize(Application::Width, Application::Height);
}

void Game::shadowPass(ObjModel& model, const Matrix4f& transform) {
	m_fbo.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	auto shader = Globals::shaderManager.getAssetPointer("depth");
	glUseProgram(shader->m_program);
	shader->loadMatrix("Projection", m_shadowProjection);
	shader->loadMatrix("Modelview", m_shadowView * transform);
	shader->loadMatrix("NormalMatrix", Matrix4f::GetNormalMatrix(m_shadowView * transform));

	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	
	//additve blending output_color = 1 * source_color + 1 * destination_color
	glBlendFunc(GL_ONE, GL_ONE);
	shader->loadFloat("DepthScale", 1.0f);

	glCullFace(GL_FRONT);
	model.drawRaw();

	shader->loadFloat("DepthScale", -1.0f);
	glCullFace(GL_BACK);
	model.drawRaw();

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glUseProgram(0);

	m_fbo.unbind();
}

