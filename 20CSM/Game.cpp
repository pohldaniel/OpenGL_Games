#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);
	m_quad = new Quad();
	
	m_camera = Camera();
	m_camera.perspective(45.0, (double)Application::Width / (double)Application::Height, 1.0f, FAR_DIST);
	m_camera.lookAt(Vector3f(75.5f, 30.0f, -110.0f), Vector3f(75.5f - 0.7f, 30.0f, -110.0f + 0.7f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -10.0f, 10.0f);

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball);
	
	quad = new Shader("res/quad.vs", "res/quad.fs");
	progTerrain[0] = new Shader("res/shadow_vertex.glsl", "res/shadow_single_fragment.glsl");
	progTerrain[1] = new Shader("res/shadow_vertex.glsl", "res/shadow_single_hl_fragment.glsl");
	progTerrain[2] = new Shader("res/shadow_vertex.glsl", "res/shadow_multi_leak_fragment.glsl");
	progTerrain[3] = new Shader("res/shadow_vertex.glsl", "res/shadow_multi_noleak_fragment.glsl");
	progTerrain[4] = new Shader("res/shadow_vertex.glsl", "res/shadow_pcf_fragment.glsl");
	progTerrain[5] = new Shader("res/shadow_vertex.glsl", "res/shadow_pcf_trilinear_fragment.glsl");
	progTerrain[6] = new Shader("res/shadow_vertex.glsl", "res/shadow_pcf_4tap_fragment.glsl");
	progTerrain[7] = new Shader("res/shadow_vertex.glsl", "res/shadow_pcf_8tap_random_fragment.glsl");
	progTerrain[8] = new Shader("res/shadow_vertex.glsl", "res/shadow_pcf_gaussian_fragment.glsl");

	progTree[0] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_single_fragment.glsl");
	progTree[1] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_single_hl_fragment.glsl");
	progTree[2] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_multi_leak_fragment.glsl");
	progTree[3] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_multi_noleak_fragment.glsl");
	progTree[4] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_pcf_fragment.glsl");
	progTree[5] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_pcf_trilinear_fragment.glsl");
	progTree[6] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_pcf_4tap_fragment.glsl");
	progTree[7] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_pcf_8tap_random_fragment.glsl");
	progTree[8] = new Shader("res/shadow_vertex_ins.glsl", "res/shadow_pcf_gaussian_fragment.glsl");

	terrain = new TerrainNV();
	terrain->Load();

	int td = terrain->getDim() / 2;
	obj_BSphere[0].center = Vector3f(-td, 50.0, -td);
	obj_BSphere[1].center = Vector3f(-td, 50.0, td);
	obj_BSphere[2].center = Vector3f(td, 50.0, td);
	obj_BSphere[3].center = Vector3f(td, 50.0, -td);
	obj_BSphere[0].radius = 1.0f;
	obj_BSphere[1].radius = 1.0f;
	obj_BSphere[2].radius = 1.0f;
	obj_BSphere[3].radius = 1.0f;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonOffset(1.0f, 4096.0f);

	for (int i = 0; i<MAX_SPLITS; i++) {
		f[i].fov = 45.0 / 57.2957795 + 0.2f;
		f[i].ratio = (double)Application::Width / (double)Application::Height;
	}

	setUpSplitDist(f, 1.0f, FAR_DIST);
	for (int i = cur_num_splits; i<MAX_SPLITS; i++)
		far_bound[i] = 0;

	for (int i = 0; i < cur_num_splits; i++) {
		far_bound[i] = 0.5f*(-f[i].fard * m_camera.getPerspectiveMatrix()[2][2] + m_camera.getPerspectiveMatrix()[3][2]) / f[i].fard + 0.5f;
	}

	glGenFramebuffersEXT(1, &depth_fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depth_fb);
	glDrawBuffer(GL_NONE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glGenTextures(1, &depth_tex_ar);

	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, depth_tex_ar);
	glTexImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, GL_DEPTH_COMPONENT24, depth_size, depth_size, MAX_SPLITS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	
	shad_cpm.resize(MAX_SPLITS);
	shadow.resize(MAX_SPLITS);

	m_canyi.loadDDSRawFromFile("res/gcanyon.dds", GL_COMPRESSED_RGBA_S3TC_DXT1_ANGLE);
	m_canyi.setLinear(GL_LINEAR_MIPMAP_NEAREST);

	tex = m_canyi.getTexture();
	activeProgramTerrain = progTerrain[program];
	activeProgramTree = progTree[program];

	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, depth_tex_ar);
	if (program < 4) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	} else {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, 0);

	m_frustum = new Frustum(m_camera.getPerspectiveMatrix(), 0.02f);
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
			m_camera.move(directrion * 20.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball);
};

void Game::render(unsigned int &frameBuffer) {
	
	shadowPass();
	/*renderScene();
	
	if (m_showDepthTex) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glUseProgram(quad->m_program);
		quad->loadInt("u_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, depth_tex_ar);

		for (int i = 0; i < cur_num_splits; i++) {
			quad->loadMatrix("u_transform", Matrix4f::Translate(-0.4f + i * 0.4f, -0.75f, 0.0f) * Matrix4f::Scale(0.2f, 0.2f * (float)Application::Width / (float)Application::Height, 0.0f));
			quad->loadInt("u_layer", i);			
			m_quad->drawRaw();
		}
		glUseProgram(0);
		
	}*/

	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	overviewCam();
	//renderUi();	
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::applyTransformation(TrackBall& arc) {

	m_transform.fromMatrix(arc.getTransform(Vector3f(0.0f, 1.0f, 0.0f), 180.0f));
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(40.0, (double)Application::Width / (double)Application::Height, 1.0, 100.0);
	m_trackball.reshape(Application::Width, Application::Height);

	for (int i = 0; i<MAX_SPLITS; i++) {
		f[i].fov = 45.0 / 57.2957795 + 0.2f;
		f[i].ratio = (double)Application::Width / (double)Application::Height;
	}
}

void Game::renderScene() {	
	// approximate the atmosphere's filtering effect as a linear function
	float sky_color[4] = { 0.8f, light_dir[1] * 0.1f + 0.7f, light_dir[1] * 0.4f + 0.5f, 1.0f };
	glClearColor(sky_color[0], sky_color[1], sky_color[2], sky_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (int i = 0; i < cur_num_splits; i++){
		shadow[i] = Matrix4f::BIAS * shadow[i] * m_camera.getInvViewMatrix();
	}
	auto shader = activeProgramTerrain;
	glUseProgram(shader->m_program);

	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
	shader->loadMatrixArray("u_viewProjectionShadows", shad_cpm, cur_num_splits);
	shader->loadMatrixArray("u_shadow", shadow, cur_num_splits);
	shader->loadFloatArray("far_d", far_bound, cur_num_splits);
	shader->loadUnsignedInt("u_numCascades", cur_num_splits);
	shader->loadVector("u_fogParameter.color", Vector3f(sky_color[0], sky_color[1], sky_color[2]));
	shader->loadVector("texSize", Vector2f((float)depth_size, 1.0f / (float)depth_size));

	shader->loadInt("stex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depth_tex_ar);

	shader->loadInt("tex", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex);

	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

	terrain->DrawRawTerrain();
	glUseProgram(0);


	shader = activeProgramTree;
	glUseProgram(shader->m_program);

	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix());
	shader->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix()));
	shader->loadMatrixArray("u_viewProjectionShadows", shad_cpm, cur_num_splits);
	shader->loadMatrixArray("u_shadow", shadow, cur_num_splits);
	shader->loadFloatArray("far_d", far_bound, cur_num_splits);
	shader->loadUnsignedInt("u_numCascades", cur_num_splits);
	shader->loadVector("u_fogParameter.color", Vector3f(sky_color[0], sky_color[1], sky_color[2]));
	shader->loadVector("texSize", Vector2f((float)depth_size, 1.0f / (float)depth_size));

	shader->loadInt("stex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depth_tex_ar);

	shader->loadInt("tex", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex);

	shader->loadVector("u_color", Vector4f(0.917647f, 0.776471f, 0.576471f, 1.0f) * 0.2f);
	terrain->DrawRawTrunk();

	shader->loadVector("u_color", Vector4f(0.301961f, 0.588235f, 0.309804f, 1.0f) * 0.2f);
	terrain->DrawRawLeave();

	glUseProgram(0);
}

void Game::shadowPass() {
	Matrix4f lightView;
	lightView.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-light_dir[0], -light_dir[1], -light_dir[2]), Vector3f(-1.0f, 0.0f, 0.0f));
	Matrix4f lightProj;
	glViewport(0, 0, depth_size, depth_size);
	// redirect rendering to the depth texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, depth_fb);
	// and render only to the shadowmap
	// offset the geometry slightly to prevent z-fighting
	// note that this introduces some light-leakage artifacts
	
	// draw all faces since our terrain is not closed.
	//glDisable(GL_CULL_FACE);
	// for all shadow maps:
	for (int i = 0; i < cur_num_splits; i++){
		// compute the camera frustum slice boundary points in world space
		updateFrustumPoints(f[i], m_camera.getPosition(), m_camera.getViewDirection());
		// adjust the view frustum of the light, so that it encloses the camera frustum slice fully.
		// note that this function sets the projection matrix as it sees best fit
		// minZ is just for optimization to cull trees that do not affect the shadows

		float minZ = applyCropMatrix(f[i], lightView, lightProj);
		glFramebufferTextureLayerEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, depth_tex_ar, 0, i);

		// clear the depth texture from last time
		glClear(GL_DEPTH_BUFFER_BIT);

		// draw the scene
		terrain->Draw(minZ, lightProj, lightView);

		shad_cpm[i] = lightProj * lightView;
		shadow[i] = lightProj * lightView;
	}

	//glEnable(GL_CULL_FACE);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0, 0, Application::Width, Application::Height);
}

void Game::setUpSplitDist(frustum f[MAX_SPLITS], float nd, float fd) {
	float lambda = split_weight;
	float ratio = fd / nd;
	f[0].neard = nd;

	for (int i = 1; i<cur_num_splits; i++) {
		float si = i / (float)cur_num_splits;

		f[i].neard = lambda*(nd*powf(ratio, si)) + (1 - lambda)*(nd + (fd - nd)*si);
		f[i - 1].fard = f[i].neard * 1.005f;
	}
	f[cur_num_splits - 1].fard = fd;
}

// Compute the 8 corner points of the current view frustum
void Game::updateFrustumPoints(frustum& f, const Vector3f& center, const Vector3f& view_dir) {
	Matrix4f lightView = Matrix4f::Rotate(view_dir, center);

	// these heights and widths are half the heights and widths of
	// the near and far plane rectangles
	float near_height = tan(f.fov / 2.0f) * f.neard;
	float near_width = near_height * f.ratio;
	float far_height = tan(f.fov / 2.0f) * f.fard;
	float far_width = far_height * f.ratio;

	f.point[0] = Vector4f(-near_width, -near_height, f.neard, 1.0f) * lightView;
	f.point[1] = Vector4f(-near_width, near_height, f.neard, 1.0f) * lightView;
	f.point[2] = Vector4f(near_width, near_height, f.neard, 1.0f) * lightView;
	f.point[3] = Vector4f(near_width, -near_height, f.neard, 1.0f) * lightView;

	f.point[4] = Vector4f(-far_width, -far_height, f.fard, 1.0f) * lightView;
	f.point[5] = Vector4f(-far_width, far_height, f.fard, 1.0f) * lightView;
	f.point[6] = Vector4f(far_width, far_height, f.fard, 1.0f) * lightView;
	f.point[7] = Vector4f(far_width, -far_height, f.fard, 1.0f) * lightView;
} 


// this function builds a projection matrix for rendering from the shadow's POV.
// First, it computes the appropriate z-range and sets an orthogonal projection.
// Then, it translates and scales it, so that it exactly captures the bounding box
// of the current frustum slice
float Game::applyCropMatrix(frustum &f, const Matrix4f& lightView, Matrix4f& lightProj) {
	float maxX = -1000.0f;
	float maxY = -1000.0f;
	float maxZ;
	float minX = 1000.0f;
	float minY = 1000.0f;
	float minZ;
	Matrix4f shade_proj;
	Matrix4f shade_mvp;
	Matrix4f shad_crop;

	Matrix4f nv_mvp;
	nv_mvp.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-light_dir[0], -light_dir[1], -light_dir[2]), Vector3f(-1.0f, 0.0f, 0.0f));

	// find the z-range of the current frustum as seen from the light
	// in order to increase precision
	//glGetFloatv(GL_MODELVIEW_MATRIX, shad_modelview);
	//nv_mvp.set_value(shad_modelview);

	// note that only the z-component is need and thus
	// the multiplication can be simplified
	// transf.z = shad_modelview[2] * f.point[0].x + shad_modelview[6] * f.point[0].y + shad_modelview[10] * f.point[0].z + shad_modelview[14];

	Vector4f transf = Vector4f(f.point[0]) ^ nv_mvp;	
	minZ = transf[2];
	maxZ = transf[2];
	for (int i = 1; i<8; i++){

		transf = Vector4f(f.point[i]) ^ nv_mvp;
		if (transf[2] > maxZ) maxZ = transf[2];
		if (transf[2] < minZ) minZ = transf[2];
	}
	// make sure all relevant shadow casters are included
	// note that these here are dummy objects at the edges of our scene
	for (int i = 0; i<NUM_OBJECTS; i++) {
		transf = Vector4f(obj_BSphere[i].center) ^ nv_mvp;
		if (transf[2] + obj_BSphere[i].radius > maxZ) maxZ = transf[2] + obj_BSphere[i].radius;
		//	if(transf.z - obj_BSphere[i].radius < minZ) minZ = transf.z - obj_BSphere[i].radius;
	}
	
	shade_proj.orthographic(-1.0, 1.0, -1.0, 1.0, -maxZ, -minZ);
	shade_mvp = shade_proj * nv_mvp;
	
	// find the extends of the frustum slice as projected in light's homogeneous coordinates
	nv_mvp.set(shade_mvp);
	for (int i = 0; i<8; i++) {
		transf = Vector4f(f.point[i]) ^ nv_mvp;

		transf[0] /= transf[3];
		transf[1] /= transf[3];
		if (transf[0] > maxX) maxX = transf[0];
		if (transf[0] < minX) minX = transf[0];
		if (transf[1] > maxY) maxY = transf[1];
		if (transf[1] < minY) minY = transf[1];
	}

	float scaleX = 2.0f / (maxX - minX);
	float scaleY = 2.0f / (maxY - minY);
	float offsetX = -0.5f*(maxX + minX)*scaleX;
	float offsetY = -0.5f*(maxY + minY)*scaleY;
	
	nv_mvp.identity();
	nv_mvp[0][0] = scaleX;
	nv_mvp[1][1] = scaleY;
	nv_mvp[0][3] = offsetX;
	nv_mvp[1][3] = offsetY;
	Matrix4f::Transpose(nv_mvp);


	shad_crop.set(nv_mvp);
	lightProj = shad_crop * shade_proj;
	return minZ;
}

void Game::regenerateDepthTex(GLuint depth_size) {
	
	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, depth_tex_ar);
	glTexImage3D(GL_TEXTURE_2D_ARRAY_EXT, 0, GL_DEPTH_COMPONENT24, depth_size, depth_size, MAX_SPLITS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, 0);
}

void Game::overviewCam() {

	Vector3f campos = m_camera.getPosition();
	//std::cout << campos[0] << "  " << campos[1] << "  " << campos[2] << std::endl;

	m_frustum->draw(m_camera, Vector3f(0.5f, -0.5f, -2.0f), Vector3f(0.1f, 0.1f, 0.1f));
	m_frustum->drawPlane(m_camera, Vector3f(0.0f, 0.0f, -2.0f), Vector3f(0.1f, 0.1f, 0.1f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f), m_frustum->m_planes[0]);
	m_frustum->drawPlane(m_camera, Vector3f(0.0f, 0.0f, -2.0f), Vector3f(0.1f, 0.1f, 0.1f), Vector4f(0.0f, 1.0f, 1.0f, 1.0f), m_frustum->m_planes[1]);
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
	ImGui::Checkbox("Draw Wirframe", &Globals::enableWireframe);
	ImGui::Checkbox("Depth Tex", &m_showDepthTex);
	if (ImGui::SliderInt("Num Splits", &cur_num_splits, 1, MAX_SPLITS)) {
		setUpSplitDist(f, 1.0f, FAR_DIST);

		for (int i = 0; i < cur_num_splits; i++) {
			far_bound[i] = 0.5f*(-f[i].fard * m_camera.getPerspectiveMatrix()[2][2] + m_camera.getPerspectiveMatrix()[3][2]) / f[i].fard + 0.5f;
		}
	}

	int currentRenderMode = program;
	if (ImGui::Combo("Program", &currentRenderMode, "Base\0Splits\0Smooth Shadow\0Smooth Shadow no leak\0PCF\0PCF Trilinear\0PCF taps\0PCF random taps\0PCF gaussian\0\0")) {
		program = static_cast<Program>(currentRenderMode);
		activeProgramTerrain = progTerrain[program];
		activeProgramTree = progTree[program];

		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, depth_tex_ar);
		if (currentRenderMode < 4) {
			glDisable(GL_POLYGON_OFFSET_FILL);
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		}else {
			glEnable(GL_POLYGON_OFFSET_FILL);
			glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, 0);
	}
	

	if (ImGui::SliderInt("slider int 2", &v, 0, 3, items[v])) {
		depth_size = static_cast<int>(pow(2, 9 + v));
		regenerateDepthTex(depth_size);
		std::cout << depth_size << std::endl;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}