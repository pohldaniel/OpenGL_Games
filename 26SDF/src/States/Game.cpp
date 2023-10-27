#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"




Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	
	m_slicedCube.create(128, 128, 128);
	m_ground.loadModel("res/mesh/ground.obj");

	m_instances.resize(3);

	m_instances[0].color = Vector3f(0.334582895f, 0.503325939f, 0.222088382f);
	m_instances[0].position = Vector3f(4.0f, 0.0f, 1.0f);
	m_instances[0].rotation = 0.0f;
	m_instances[0].mesh.loadModel("res/mesh/sphere.obj");
	m_instances[0].mesh.getMeshes()[0]->packBuffer();

	bake_sdf(m_instances[0], 0.025f, 4);

	InstanceUniforms uniform;

	uniform.half_extents = Vector4f((m_instances[0].max_extents - m_instances[0].min_extents) / 2.0f, 0.0f);
	uniform.os_center = Vector4f((m_instances[0].max_extents + m_instances[0].min_extents) / 2.0f, 1.0f);
	uniform.sdf_idx = { static_cast<int>(m_textureUniforms.size()), 0, 0, 0 };

	m_instanceUniforms.push_back(uniform);
	m_textureUniforms.push_back(m_instances[0].sdf.makeTextureHandleResident());

	m_instances[1].color = Vector3f(0.103627160f, 0.303517729f, 0.482233524f);
	m_instances[1].position = Vector3f(-4.0f, 0.0f, 1.0f);
	m_instances[1].rotation = 0.0f;
	m_instances[1].mesh.loadModel("res/mesh/cylinder.obj");
	m_instances[1].mesh.getMeshes()[0]->packBuffer();

	bake_sdf(m_instances[1], 0.025f, 4);

	InstanceUniforms uniform2;

	uniform2.half_extents = Vector4f((m_instances[1].max_extents - m_instances[1].min_extents) / 2.0f, 0.0f);
	uniform2.os_center = Vector4f((m_instances[1].max_extents + m_instances[1].min_extents) / 2.0f, 1.0f);
	uniform2.sdf_idx = { static_cast<int>(m_textureUniforms.size()), 0, 0, 0 };

	m_instanceUniforms.push_back(uniform2);
	m_textureUniforms.push_back(m_instances[1].sdf.makeTextureHandleResident());


	m_instances[2].color = Vector3f(0.569844782f, 0.157939225f, 0.157963991f);
	m_instances[2].position = Vector3f(0.0f, 0.0f, 1.0f);
	m_instances[2].rotation = 0.0f;
	m_instances[2].mesh.loadModel("res/mesh/bunny.obj");
	m_instances[2].mesh.getMeshes()[0]->packBuffer();

	bake_sdf(m_instances[2], 0.025f, 4);

	InstanceUniforms uniform3;

	uniform3.half_extents = Vector4f((m_instances[2].max_extents - m_instances[2].min_extents) / 2.0f, 0.0f);
	uniform3.os_center = Vector4f((m_instances[2].max_extents + m_instances[2].min_extents) / 2.0f, 1.0f);
	uniform3.sdf_idx = { static_cast<int>(m_textureUniforms.size()), 0, 0, 0 };

	m_instanceUniforms.push_back(uniform3);
	m_textureUniforms.push_back(m_instances[2].sdf.makeTextureHandleResident());

	glGenBuffers(1, &m_globalUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_globalUbo);
	glBufferData(GL_UNIFORM_BUFFER, 84 , NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_globalUbo, 0, 84);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &m_instanceUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_instanceUbo);
	glBufferData(GL_UNIFORM_BUFFER, 176 * NUM_INSTANCES, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_instanceUbo, 0, 176 * NUM_INSTANCES);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &m_sdfUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_sdfUbo);
	glBufferData(GL_UNIFORM_BUFFER, 16 * NUM_SDFS, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, m_sdfUbo, 0, 16 * NUM_SDFS);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	

	SamplerPack sampler[3] = { { m_textureUniforms[0], 0, 0, 0, 0, 0, 0, 0, 0},
							   { m_textureUniforms[1], 0, 0, 0, 0, 0, 0, 0, 0},
							   { m_textureUniforms[2], 0, 0, 0, 0, 0, 0, 0, 0}};
	

	glBindBuffer(GL_UNIFORM_BUFFER, m_sdfUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * m_textureUniforms.size(), &sampler[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

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
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
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
			m_camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);

	update_transforms();
	updateUbo();
}

void Game::renderMesh(const ObjModel* mesh, const Matrix4f& model, const Vector3f& color) {
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadVector("u_Color", color);
	shader->loadMatrix("u_model", model);
	
	mesh->drawRaw();
	
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();
	
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();

	shader->loadBool("u_SDFSoftShadows", m_soft_shadows);
	shader->loadFloat("u_SDFTMin", m_t_min);
	shader->loadFloat("u_SDFTMax", m_t_max);
	shader->loadFloat("u_SDFSoftShadowsK", m_soft_shadows_k);
	shader->loadFloat("u_AOStepSize", m_ao_step_size);
	shader->loadFloat("u_AOStrength", m_ao_strength);
	shader->loadInt("u_AONumSteps", m_ao_num_steps);
	shader->loadBool("u_AO", m_ao);

	shader->loadVector("u_LightPos", m_light_pos);
	shader->loadVector("u_LightDirection", Vector3f(0.0f, m_light_pitch, -1.0f).normalize());
	shader->loadFloat("u_LightInnerCutoff", std::cosf(m_light_inner_cutoff * PI_ON_180));
	shader->loadFloat("u_LightOuterCutoff", std::cosf(m_light_outer_cutoff * PI_ON_180));
	shader->loadFloat("u_LightRange", m_light_range);

	renderMesh(&m_ground, Matrix4f::IDENTITY, Vector3f(0.5f));

	for (const auto& instance : m_instances)
		renderMesh(&instance.mesh, instance.transform, instance.color);

	shader->unuse();

	/*glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	auto shader = Globals::shaderManager.getAssetPointer("texture3d");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
	

	m_slicedCube.drawRaw();
	shader->unuse();*/

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {

	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::bake_sdf(Instance& instance, float grid_step_size, int padding) {
	Vector3f  min_extents = instance.mesh.getAABB().position - (Vector3f(grid_step_size) * float(padding));
	Vector3f  max_extents = instance.mesh.getAABB().position + instance.mesh.getAABB().size + (Vector3f(grid_step_size) * float(padding));
	Vector3f  grid_origin = min_extents + Vector3f(grid_step_size / 2.0f);
	Vector3f  box_size = max_extents - min_extents;

	std::array<int, 3> volume_size = { std::ceil(box_size[0] / grid_step_size), std::ceil(box_size[1] / grid_step_size), std::ceil(box_size[2] / grid_step_size) };

	instance.volume_size = volume_size;
	instance.grid_origin = grid_origin;
	instance.grid_step_size = grid_step_size;
	instance.min_extents = min_extents;
	instance.max_extents = max_extents;

	instance.sdf.createTexture3D(volume_size[0], volume_size[1], volume_size[2], GL_R32F, GL_RED, GL_FLOAT);
	instance.sdf.setFilter(GL_LINEAR, GL_LINEAR);
	instance.sdf.setWrapMode(GL_CLAMP_TO_EDGE);

	auto shader = Globals::shaderManager.getAssetPointer("sdf_comp");
	shader->use();
	shader->loadVector("u_GridStepSize", Vector3f(grid_step_size));
	shader->loadVector("u_GridOrigin", grid_origin);
	shader->loadUnsignedInt("u_NumTriangles", instance.mesh.getNumberOfTriangles());
	shader->loadVector("u_VolumeSize", volume_size);

	glBindImageTexture(0, instance.sdf.getTexture(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, instance.mesh.getMeshes()[0]->getVbo());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instance.mesh.getMeshes()[0]->getIbo());


	const uint32_t NUM_THREADS_X = 8;
	const uint32_t NUM_THREADS_Y = 8;
	const uint32_t NUM_THREADS_Z = 1;

	uint32_t size_x = static_cast<uint32_t>(ceil(float(volume_size[0]) / float(NUM_THREADS_X)));
	uint32_t size_y = static_cast<uint32_t>(ceil(float(volume_size[1]) / float(NUM_THREADS_Y)));
	uint32_t size_z = static_cast<uint32_t>(ceil(float(volume_size[2]) / float(NUM_THREADS_Z)));

	glDispatchCompute(size_x, size_y, size_z);

	glFinish();
}

void Game::updateUbo() {

	glBindBuffer(GL_UNIFORM_BUFFER, m_globalUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 84, &m_globalUniforms);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_instanceUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 176 * m_instances.size(), &m_instanceUniforms[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);	
}

void Game::update_transforms() {
	// Update camera matrices.
	m_globalUniforms.view_proj = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix();
	m_globalUniforms.cam_pos = Vector4f(m_camera.getPosition(), 0.0f);
	m_globalUniforms.num_instances = m_instances.size();

	for (int i = 0; i < m_instances.size(); i++){
		auto& instance = m_instances[i];

		instance.transform = Matrix4f::Translate(instance.position);
			
		//if (instance.animate)
			//instance.transform = instance.transform * glm::rotate(glm::mat4(1.0f), glm::radians(float(glfwGetTime()) * 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//else
			//instance.transform = instance.transform * glm::rotate(glm::mat4(1.0f), glm::radians(instance.rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		m_instanceUniforms[i].inverse_transform = Matrix4f::InvTranslate(instance.position);
		m_instanceUniforms[i].ws_center = m_instanceUniforms[i].os_center * instance.transform;

		Vector3f axis[] = {
			Vector3f(1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(0.0f, 0.0f, 1.0f)
		};

		for (int j = 0; j < 3; j++)
			m_instanceUniforms[i].ws_axis[j] = Vector4f(axis[j] * instance.transform, 0.0f);
	}
}