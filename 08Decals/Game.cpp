#include <random>
#include "Game.h"
#include "engine/input/EventDispatcher.h"
#include "Application.h"

const float HEIGHTMAP_ROUGHNESS = 1.2f;
const float HEIGHTMAP_SCALE = 2.0f;

const int HEIGHTMAP_RESOLUTION = 128;
const int HEIGHTMAP_WIDTH = 8192;
const float CAMERA_Y_OFFSET = 100.0f;
const Vector3f CAMERA_ACCELERATION(400.0f, 400.0f, 400.0f);
const Vector3f CAMERA_VELOCITY(200.0f, 200.0f, 200.0f);
Vector3f LIGHT_DIRECTION(-100.0f, 100.0f, -100.0f);

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME), m_skybox(SkyBox(m_dt, m_fdt, 2500.0f)) {
	EventDispatcher::AddMouseListener(this);
	m_quadShader = Globals::shaderManager.getAssetPointer("quad");
	m_quadShadow = Globals::shaderManager.getAssetPointer("quad_shadow");
	m_copyFramebuffer.create(Application::Width, Application::Height);
	m_copyFramebuffer.attachTexture(Framebuffer::Attachments::RGBA);
	m_copyFramebuffer.attachRenderbuffer(Framebuffer::Attachments::DEPTH_STENCIL);

	m_leftBottom = new Quad(Vector2f(-1.0f, -1.0f), size);
	m_rightBottom = new Quad(Vector2f(1.0f - size[0], -1.0f), size);
	m_leftTop = new Quad(Vector2f(-1.0f, 1.0f - size[1]), size);
	m_rightTop = new Quad(Vector2f(1.0f - size[0], 1.0f - size[1]), size);

	m_quad = new Quad(Vector2f(-1.0f, -1.0f), Vector2f(2.0f, 2.0f));

	offsetX = Application::Width * size[0] * 0.5f;
	offsetY = Application::Height * size[1] * 0.5f;

	m_albedoFramebuffer.create(ALBEDO_TEXTURE_SIZE, ALBEDO_TEXTURE_SIZE);
	m_albedoFramebuffer.attachTexture(Framebuffer::Attachments::RGBA);

	m_depthFramebuffer.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	m_depthFramebuffer.attachTexture(Framebuffer::Attachments::RED);
	m_depthFramebuffer.attachTexture(Framebuffer::Attachments::DEPTH16F);

	Vector3f pos;
	pos[0] = 150.0f;
	pos[2] = 0.0f;
	pos[1] = 50.0f;

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 5000.0f);
	m_camera.lookAt(pos, Vector3f(0.0f, 30.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setAcceleration(CAMERA_ACCELERATION);
	m_camera.setVelocity(CAMERA_VELOCITY);
	m_camera.setRotationSpeed(0.1f);

	m_teapot.loadModel("res/models/teapot/teapot_smooth.obj", false, true);
	m_teapot.initAssets(Globals::shaderManager, Globals::textureManager);
	m_teapot.getTransform().setRotPosScale(Vector3f(1.0f, 0.0f, 0.0f), -90.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f);

	std::vector<btCollisionShape*> teapotShape = Physics::CreateStaticCollisionShapes(&m_teapot, 1.0f);
	btRigidBody* body = Globals::physics->addStaticModel(teapotShape, Physics::BtTransform(), false, btVector3(1, 1, 1), Physics::collisiontypes::RENDERABLE_OBJECT | Physics::collisiontypes::PICKABLE_OBJECT, Physics::collisiontypes::RAY);

	initTexture(Globals::textureManager.get("null"));


	m_vertices.push_back(-1.0f); m_vertices.push_back(-1.0f); m_vertices.push_back(1.0f);
	m_vertices.push_back(1.0f); m_vertices.push_back(-1.0f); m_vertices.push_back(1.0f);
	m_vertices.push_back(1.0f); m_vertices.push_back(1.0f); m_vertices.push_back(1.0f);
	m_vertices.push_back(-1.0f); m_vertices.push_back(1.0f); m_vertices.push_back(1.0f);

	m_vertices.push_back(-1.0f); m_vertices.push_back(-1.0f); m_vertices.push_back(-1.0f);
	m_vertices.push_back(1.0f); m_vertices.push_back(-1.0f); m_vertices.push_back(-1.0f);
	m_vertices.push_back(1.0f); m_vertices.push_back(1.0f); m_vertices.push_back(-1.0f);
	m_vertices.push_back(-1.0f); m_vertices.push_back(1.0f); m_vertices.push_back(-1.0f);

	unsigned int indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	m_vertices.clear();
	m_vertices.shrink_to_fit();

	m_gBuffer.create(Application::Width, Application::Height);
	m_gBuffer.attachTexture(Framebuffer::Attachments::RGBA);
	m_gBuffer.attachTexture(Framebuffer::Attachments::RGBA32F);
	m_gBuffer.attachTexture(Framebuffer::Attachments::RGB32F);
	m_gBuffer.attachTexture(Framebuffer::Attachments::RGB32F);
	m_gBuffer.attachTexture(Framebuffer::Attachments::RGB32F);
	m_gBuffer.attachTexture(Framebuffer::Attachments::DEPTH32F);

	m_decalBuffer.create(Application::Width, Application::Height);
	m_decalBuffer.attachTexture(Framebuffer::Attachments::RGBA);
	m_decalBuffer.attachTexture(Framebuffer::Attachments::RGBA32F);
	m_decalBuffer.attachTexture(Framebuffer::Attachments::DEPTH32F, m_gBuffer.getDepthTexture());

	m_finalBuffer.create(Application::Width, Application::Height);
	m_finalBuffer.attachTexture(Framebuffer::Attachments::RGBA);

	Globals::textureManager.createEmptyTexture("destination", 
												Globals::textureManager.get("stone").getWidth(),
												Globals::textureManager.get("stone").getHeight(),
												Globals::textureManager.get("stone").getInternalFormat(),
												Globals::textureManager.get("stone").getFormat(),
												Globals::textureManager.get("stone").getType());
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
		if (renderMode == RenderMode::TEXTURE)
			initTexture(Globals::textureManager.get("null"));
		if (renderMode == RenderMode::DEFFERED)
			m_decal_instances.clear();
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debug = !m_debug;
	}

	if (keyboard.keyPressed(Keyboard::KEY_I)) {
		m_useFilter = !m_useFilter;
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		if (renderMode == RenderMode::DEFFERED)
			renderMode = RenderMode::TEXTURE;
		else if (renderMode == RenderMode::TEXTURE)
			renderMode = RenderMode::DEFFERED;
	}

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::MouseButton::BUTTON_LEFT)) {

		float mouseXndc = (2.0f * mouse.xPosAbsolute()) / (float)Application::Width - 1.0f;
		float mouseYndc = 1.0f - (2.0f * mouse.yPosAbsolute()) / (float)Application::Height;

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

			if (renderMode == RenderMode::TEXTURE) {

				m_projector_pos = pos + normal * PROJECTOR_BACK_OFF_DISTANCE;
				m_projector_dir = -normal;

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

			if (renderMode == RenderMode::DEFFERED) {
				DecalInstance instance;

				instance.m_hit_pos = pos;
				instance.m_hit_normal = normal;

				instance.m_hit_distance = callback.m_closestHitFraction;
				instance.m_projector_pos = pos + normal * m_projector_outer_depth;
				instance.m_projector_dir = -normal;

				Vector3f default_up = Vector3f(0.0f, 0.0f, 1.0f);

				if (instance.m_hit_normal[0] > instance.m_hit_normal[1] && instance.m_hit_normal[0] > instance.m_hit_normal[2])
					default_up = Vector3f(0.0f, -1.0f, 0.0f);
				else if (instance.m_hit_normal[2] > instance.m_hit_normal[1] && instance.m_hit_normal[2] > instance.m_hit_normal[0])
					default_up = Vector3f(0.0f, -1.0f, 0.0f);

				Matrix4f rot;
				rot.rotate(instance.m_projector_dir, 0.0f);
				Vector3f rotated_axis = rot * Vector3f(0.0f, 1.0f, 0.0f);

				if (Globals::textureManager.get("decal").getWidth() > Globals::textureManager.get("decal").getHeight()) {
					instance.m_aspect_ratio[0] = 1.0f;
					instance.m_aspect_ratio[1] = float(Globals::textureManager.get("decal").getWidth()) / float(Globals::textureManager.get("decal").getHeight());
				}else {
					instance.m_aspect_ratio[0] = float(Globals::textureManager.get("decal").getHeight()) / float(Globals::textureManager.get("decal").getWidth());
					instance.m_aspect_ratio[1] = 1.0f;
				}

				instance.m_projector_view.lookAt(instance.m_projector_pos, pos, rotated_axis + Vector3f(0.001f, 0.0f, 0.0f));
				instance.m_projector_view_inv.invLookAt(instance.m_projector_pos, pos, rotated_axis + Vector3f(0.001f, 0.0f, 0.0f));
				instance.m_projector_proj.orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, m_projector_outer_depth + m_projector_inner_depth);
				instance.m_projector_proj_inv.invOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, m_projector_outer_depth + m_projector_inner_depth);
				instance.m_projector_view_proj = instance.m_projector_proj * instance.m_projector_view;
				instance.m_decal_overlay_color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
				m_decal_instances.push_back(instance);
			}

		}
	}

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotateSmoothly(dx, dy, 0.0f);

		} 

		if (move) {
			m_camera.updatePosition(directrion, m_dt);
		}

	}
	m_skybox.update();
};


void Game::render(unsigned int &frameBuffer) {
	if(m_useFilter)
		runInvertFilter(Globals::textureManager.get("stone").getTexture(), Globals::textureManager.get("destination").getTexture(), Globals::textureManager.get("stone").getWidth(), Globals::textureManager.get("stone").getHeight());

	if (renderMode == RenderMode::TEXTURE)
		shdowPass();

	if (renderMode == RenderMode::DEFFERED) {
		renderGBuffer();
		renderDecals();
		combinerPass();
		renderDeferred();
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (renderMode == RenderMode::TEXTURE) {
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
			glBlitFramebuffer(0, 0, Application::Width, Application::Height, 0, 0, Application::Width, Application::Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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

			glScissor(Application::Width - offsetX, 0, offsetX, offsetY);
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

	if (renderMode == RenderMode::DEFFERED) {
		auto shader = Globals::shaderManager.getAssetPointer("quad");
		glUseProgram(shader->m_program);
		m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		m_quad->render(m_finalBuffer.getColorTexture(0));
		glUseProgram(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if (m_debug) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_copyFramebuffer.getFramebuffer());
			glBlitFramebuffer(0, 0, Application::Width, Application::Height, 0, 0, Application::Width, Application::Height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_SCISSOR_TEST);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

			glScissor(0, Application::Height - offsetY, offsetX, offsetY);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(m_quadShader->m_program);
			m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			m_leftTop->render(m_decalBuffer.getColorTexture(0));
			glUseProgram(0);

			glScissor(Application::Width - offsetX, Application::Height - offsetY, offsetX, offsetY);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(m_quadShader->m_program);
			m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			m_rightTop->render(m_decalBuffer.getColorTexture(1));
			glUseProgram(0);

			glScissor(0, 0, offsetX, offsetY);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(m_quadShader->m_program);
			m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			m_leftBottom->render(m_gBuffer.getColorTexture(0));
			glUseProgram(0);

			glScissor(Application::Width - offsetX, 0, offsetX, offsetY);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(m_quadShader->m_program);
			m_quadShader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			m_rightBottom->render(m_gBuffer.getColorTexture(1));
			glUseProgram(0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_SCISSOR_TEST);
		}
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

void Game::renderGBuffer() {
	glEnable(GL_DEPTH_TEST);
	m_gBuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("g_buffer");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", Globals::projection);
	shader->loadMatrix("u_view", m_camera.getViewMatrix());

	shader->loadInt("s_Albedo", 0);
	m_useFilter ? Globals::textureManager.get("destination").bind(0) : Globals::textureManager.get("stone").bind(0);

	shader->loadInt("s_Normal", 1);
	Globals::textureManager.get("stone_normal").bind(1);

	m_teapot.drawRaw();
	glUseProgram(0);

	m_gBuffer.unbind();
}

void Game::renderDecals() {
	glEnable(GL_DEPTH_TEST);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	m_decalBuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("decals_defferred");
	glUseProgram(shader->m_program);
	for (int i = 0; i < m_decal_instances.size(); i++) {
		shader->loadMatrix("view_proj", Globals::projection * m_camera.getViewMatrix());
		shader->loadMatrix("u_InvDecalVP", m_decal_instances[i].m_projector_view_inv * m_decal_instances[i].m_projector_proj_inv);

		shader->loadMatrix("inv_view_proj", m_camera.getInvViewMatrix() * Globals::invProjection);
		shader->loadMatrix("u_DecalVP", m_decal_instances[i].m_projector_view_proj);

		shader->loadMatrix("u_DecalModel", m_decal_instances[i].m_projector_view_inv);
		shader->loadVector("u_DecalOverlayColor", m_decal_instances[i].m_decal_overlay_color);
		shader->loadVector("u_AspectRatio", m_decal_instances[i].m_aspect_ratio);

		shader->loadInt("s_Decal", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("decal").getTexture());

		shader->loadInt("s_DecalNormal", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("decal_n").getTexture());

		shader->loadInt("s_Depth", 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.getDepthTexture());

		shader->loadInt("s_SourceNormal", 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorTexture(2));

		shader->loadInt("s_Tangent", 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorTexture(3));

		shader->loadInt("s_Bitangent", 5);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorTexture(4));

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//m_quad->render();
	}
	glUseProgram(0);
	m_decalBuffer.unbind();

	glDepthMask(GL_TRUE);

}

void Game::combinerPass() {
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	m_gBuffer.bind();

	auto shader = Globals::shaderManager.getAssetPointer("combiner");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);

	shader->loadInt("s_Albedo_decal", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_decalBuffer.getColorTexture(0));

	shader->loadInt("s_Normal_decal", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_decalBuffer.getColorTexture(1));

	m_quad->render();
	glUseProgram(0);

	m_gBuffer.unbind();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Game::renderDeferred() {

	m_finalBuffer.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("deferred");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);

	shader->loadInt("s_Albedo", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorTexture(0));

	shader->loadInt("s_Normal", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.getColorTexture(1));

	m_quad->render();
	glUseProgram(0);

	m_finalBuffer.unbind();
}

void Game::runInvertFilter(GLuint inputTex, GLuint outputTex, int width, int height) {
	
	auto shader = Globals::shaderManager.getAssetPointer("invert");
	glUseProgram(shader->m_program);

	glBindImageTexture(0, inputTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(width / 16, height / 16, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(0);

}

void Game::resize(int deltaW, int deltaH) {

	m_copyFramebuffer.resize(Application::Width, Application::Height);

	m_gBuffer.resize(Application::Width, Application::Height);
	m_decalBuffer.resize(Application::Width, Application::Height);
	m_finalBuffer.resize(Application::Width, Application::Height);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 5000.0f);

	offsetX = Application::Width * size[0] * 0.5f;
	offsetY = Application::Height * size[1] * 0.5f;
}