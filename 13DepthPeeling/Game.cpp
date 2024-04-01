#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

#define HDMODEL 0

GLenum g_drawBuffers[] = { GL_COLOR_ATTACHMENT0,
						GL_COLOR_ATTACHMENT1,
						GL_COLOR_ATTACHMENT2,
						GL_COLOR_ATTACHMENT3,
						GL_COLOR_ATTACHMENT4,
						GL_COLOR_ATTACHMENT5,
						GL_COLOR_ATTACHMENT6
};


Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_quad = new Quad();

	m_camera = Camera();
	m_camera.perspective(45.0f * _180_ON_PI, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 1.5f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-2.5f);
	applyTransformation(m_trackball.getTransform());

	m_bunny.loadModel("res/models/bunny.obj");
	#if HDMODEL
	m_dragon.loadModel("res/models/dragon_hd.obj");
	#else
	m_dragon.loadModel("res/models/dragon.obj");
	#endif
	m_buddha.loadModel("res/models/buddha.obj", Vector3f(1.0f, 0.0f, 0.0f), -90.0f);

	glDisable(GL_CULL_FACE);
	glGenQueries(1, &m_queryId);
	//////////////////////////////////////////Front to Back Peel///////////////////////////////////////////////////////
	InitF2BPeelingRenderTargets();

	GLuint vertex = Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/shader/F2B/base_shade_vertex.glsl");
	GLuint fragment = Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/F2B/shade_fragment.glsl");

	m_shaderF2BInit = new Shader();
	m_shaderF2BInit->attachShader(vertex);
	m_shaderF2BInit->attachShader(fragment);
	m_shaderF2BInit->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/F2B/front_peeling_init_fragment.glsl"));
	m_shaderF2BInit->linkShaders();

	m_shaderF2BPeel = new Shader();
	m_shaderF2BPeel->attachShader(vertex);
	m_shaderF2BPeel->attachShader(fragment);
	m_shaderF2BPeel->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/F2B/front_peeling_peel_fragment.glsl"));
	m_shaderF2BPeel->linkShaders();

	m_shaderF2BBlend = new Shader("res/shader/F2B/base_vertex.glsl", "res/shader/F2B/front_peeling_blend_fragment.glsl");
	m_shaderF2BFinal = new Shader("res/shader/F2B/base_vertex.glsl", "res/shader/F2B/front_peeling_final_fragment.glsl");

	//////////////////////////////////////////Dual Peel///////////////////////////////////////////////////////
	InitDualPeelingRenderTargets();

	m_shaderDualInit = new Shader("res/shader/dual/dual_peeling_init_vertex.glsl", "res/shader/dual/dual_peeling_init_fragment.glsl");

	m_shaderDualPeel = new Shader();
	m_shaderDualPeel->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/shader/dual/dual_peeling_peel_vertex.glsl"));
	m_shaderDualPeel->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/dual/shade_fragment.glsl"));
	m_shaderDualPeel->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/dual/dual_peeling_peel_fragment.glsl"));
	m_shaderDualPeel->linkShaders();

	m_shaderDualBlend = new Shader("res/shader/dual/dual_peeling_blend_vertex.glsl", "res/shader/dual/dual_peeling_blend_fragment.glsl");
	m_shaderDualFinal = new Shader("res/shader/dual/dual_peeling_final_vertex.glsl", "res/shader/dual/dual_peeling_final_fragment.glsl");

	//////////////////////////////////////////Average Sum///////////////////////////////////////////////////////
	InitAccumulationRenderTargets();

	m_shaderAverageInit = new Shader(); 
	m_shaderAverageInit->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/shader/average/wavg_init_vertex.glsl"));
	m_shaderAverageInit->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/average/shade_fragment.glsl"));
	m_shaderAverageInit->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/average/wavg_init_fragment.glsl"));
	m_shaderAverageInit->linkShaders();

	m_shaderAverageFinal = new Shader("res/shader/average/wavg_final_vertex.glsl", "res/shader/average/wavg_final_fragment.glsl");
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_shaderWeightedInit = new Shader();
	m_shaderWeightedInit->attachShader(Shader::LoadShaderProgram(GL_VERTEX_SHADER, "res/shader/weighted/wsum_init_vertex.glsl"));
	m_shaderWeightedInit->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/weighted/shade_fragment.glsl"));
	m_shaderWeightedInit->attachShader(Shader::LoadShaderProgram(GL_FRAGMENT_SHADER, "res/shader/weighted/wsum_init_fragment.glsl"));
	m_shaderWeightedInit->linkShaders();

	m_shaderWeightedFinal = new Shader("res/shader/weighted/wsum_final_vertex.glsl", "res/shader/weighted/wsum_final_fragment.glsl");
}

Game::~Game() {}

void Game::fixedUpdate() {

}

void Game::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_N)) {

		if (model == Model::BUNNY)
			model = Model::DRAGON;
		else if (model == Model::DRAGON)
			model = Model::BUDDHA;
		else if (model == Model::BUDDHA)
			model = Model::BUNNY;
	}

	if (keyboard.keyDown(Keyboard::KEY_COMMA)) {
		m_opacity = (std::max)(0.0f, m_opacity - 0.01f);
	}

	if (keyboard.keyDown(Keyboard::KEY_PERIOD)) {
		m_opacity = (std::min)(1.0f, m_opacity + 0.01f);
	}

	if (keyboard.keyPressed(Keyboard::KEY_ADD)) {
		m_numPasses = (std::max)(4, m_numPasses - 1);
	}

	if (keyboard.keyPressed(Keyboard::KEY_SUBTRACT)) {
		m_numPasses = (std::min)(30, m_numPasses + 1);
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {

		if (renderMode == RenderMode::DUAL_PEELING_MODE)
			renderMode = RenderMode::F2B_PEELING_MODE;
		else if (renderMode == RenderMode::F2B_PEELING_MODE)
			renderMode = RenderMode::WEIGHTED_AVERAGE_MODE;
		else if (renderMode == RenderMode::WEIGHTED_AVERAGE_MODE)
			renderMode = RenderMode::WEIGHTED_SUM_MODE;
		else if (renderMode == RenderMode::WEIGHTED_SUM_MODE)
			renderMode = RenderMode::DUAL_PEELING_MODE;
	}

	if (keyboard.keyPressed(Keyboard::KEY_B)) {
		g_useOQ = !g_useOQ;
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
			m_camera.move(direction * 5.0f * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball.getTransform());

};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_numGeoPasses = 0;

	switch(renderMode) {
		case DUAL_PEELING_MODE:
			RenderDualPeeling();
			break;
		case F2B_PEELING_MODE:
			RenderFrontToBackPeeling();
			break;
		case WEIGHTED_AVERAGE_MODE:
			RenderAverageColors();
			break;
		case WEIGHTED_SUM_MODE:
			RenderWeightedSum();
			break;
	}
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
	m_transform.fromMatrix(mtx);
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
}

void Game::RenderFrontToBackPeeling() {
	
	// ---------------------------------------------------------------------
	// 1. Peel the first layer
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, m_frontColorBlenderFboId);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(m_shaderF2BInit->m_program);
	m_shaderF2BInit->loadFloat("uAlpha", m_opacity);
	m_shaderF2BInit->loadMatrix("uNormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	m_shaderF2BInit->loadMatrix("uModelViewMatrix", m_camera.getProjectionMatrix() *  m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	drawModel();
	m_numGeoPasses++;

	glUseProgram(0);

	// ---------------------------------------------------------------------
	// 2. Depth Peeling + Blending
	// ---------------------------------------------------------------------

	int numLayers = (m_numPasses - 1) * 2;
	for (int layer = 1; g_useOQ || layer < numLayers; layer++) {
		int currId = layer % 2;
		int prevId = 1 - currId;

		glBindFramebuffer(GL_FRAMEBUFFER, m_frontFboId[currId]);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		if (g_useOQ) {
			glBeginQuery(GL_SAMPLES_PASSED, m_queryId);
		}

		glUseProgram(m_shaderF2BPeel->m_program);
		m_shaderF2BPeel->loadFloat("uAlpha", m_opacity);

		m_shaderF2BPeel->loadMatrix("uNormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
		m_shaderF2BPeel->loadMatrix("uModelViewMatrix", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		m_shaderF2BPeel->loadInt("DepthTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, m_frontDepthTexId[prevId]);
		drawModel();
		m_numGeoPasses++;

		glUseProgram(0);

		if (g_useOQ) {
			glEndQuery(GL_SAMPLES_PASSED);
		}

		// ---------------------------------------------------------------------
		// 2.2. Blend the current layer
		// ---------------------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, m_frontColorBlenderFboId);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		// UNDER operator
		glBlendEquation(GL_FUNC_ADD);
		glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(m_shaderF2BBlend->m_program);
		m_shaderF2BBlend->loadMatrix("uModelViewMatrix", Matrix4f::IDENTITY);

		m_shaderF2BBlend->loadInt("TempTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, m_frontColorTexId[currId]);
		m_quad->drawRaw();
		glUseProgram(0);

		glDisable(GL_BLEND);

		GLuint sample_count;
		glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, &sample_count);
		if (sample_count == 0){
			break;
		}
	}
	

	
	// ---------------------------------------------------------------------
	// 3. Compositing Pass
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(m_shaderF2BFinal->m_program);
	m_shaderF2BFinal->loadMatrix("uModelViewMatrix", Matrix4f::IDENTITY);
	m_shaderF2BFinal->loadVector("uBackgroundColor", Vector3f(1.0f, 1.0f, 1.0f));

	m_shaderF2BFinal->loadInt("ColorTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, m_frontColorBlenderTexId);

	m_quad->drawRaw();

	glUseProgram(0);
}

void Game::RenderDualPeeling() {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// ---------------------------------------------------------------------
	// 1. Initialize Min-Max Depth Buffer
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingSingleFboId);

	// Render targets 1 and 2 store the front and back colors
	// Clear to 0.0 and use MAX blending to filter written color
	// At most one front color and one back color can be written every pass
	glDrawBuffers(2, &g_drawBuffers[1]);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
	glDrawBuffer(g_drawBuffers[0]);
	glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendEquation(GL_MAX);

	glUseProgram(m_shaderDualInit->m_program);
	m_shaderDualInit->loadMatrix("uModelViewMatrix", m_camera.getProjectionMatrix() *  m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	drawModel();
	m_numGeoPasses++;

	glUseProgram(0);

	glDrawBuffer(g_drawBuffers[6]);
	glClearColor(1.0, 1.0, 1.0 , 0);
	glClear(GL_COLOR_BUFFER_BIT);

	int currId = 0;

	for (int pass = 1; g_useOQ || pass < m_numPasses; pass++) {
		currId = pass % 2;
		int prevId = 1 - currId;
		int bufId = currId * 3;

		//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_dualPeelingFboId[currId]);

		glDrawBuffers(2, &g_drawBuffers[bufId + 1]);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawBuffer(g_drawBuffers[bufId + 0]);
		glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render target 0: RG32F MAX blending
		// Render target 1: RGBA MAX blending
		// Render target 2: RGBA MAX blending
		glDrawBuffers(3, &g_drawBuffers[bufId + 0]);
		glBlendEquation(GL_MAX);

		glUseProgram(m_shaderDualPeel->m_program);
		m_shaderDualPeel->loadMatrix("uNormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
		m_shaderDualPeel->loadMatrix("uModelViewMatrix", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		m_shaderDualPeel->loadFloat("Alpha", m_opacity);

		m_shaderDualPeel->loadInt("DepthBlenderTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, g_dualDepthTexId[prevId]);

		m_shaderDualPeel->loadInt("FrontBlenderTex", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[prevId]);

		drawModel();
		m_numGeoPasses++;

		glUseProgram(0);

		// Full screen pass to alpha-blend the back color
		glDrawBuffer(g_drawBuffers[6]);

		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (g_useOQ) {
			glBeginQuery(GL_SAMPLES_PASSED, m_queryId);
		}

		glUseProgram(m_shaderDualBlend->m_program);
		m_shaderDualBlend->loadMatrix("uModelViewMatrix", Matrix4f::IDENTITY);

		m_shaderDualBlend->loadInt("TempTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[currId]);
		m_quad->drawRaw();
		glUseProgram(0);

		if (g_useOQ) {
			glEndQuery(GL_SAMPLES_PASSED);
			GLuint sample_count;
			glGetQueryObjectuiv(m_queryId, GL_QUERY_RESULT, &sample_count);
			if (sample_count == 0) {
				break;
			}
		}
	}

	glDisable(GL_BLEND);

	// ---------------------------------------------------------------------
	// 3. Final Pass
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	glUseProgram(m_shaderDualFinal->m_program);
	m_shaderDualFinal->loadMatrix("uModelViewMatrix", Matrix4f::IDENTITY);

	m_shaderDualFinal->loadInt("DepthBlenderTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_dualDepthTexId[currId]);

	m_shaderDualFinal->loadInt("FrontBlenderTex", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[currId]);

	m_shaderDualFinal->loadInt("BackBlenderTex", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId);

	m_quad->drawRaw();
	glUseProgram(0);
}

void Game::RenderAverageColors() {
	glDisable(GL_DEPTH_TEST);

	// ---------------------------------------------------------------------
	// 1. Accumulate Colors and Depth Complexity
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, g_accumulationFboId);
	glDrawBuffers(2, g_drawBuffers);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	glUseProgram(m_shaderAverageInit->m_program);
	m_shaderAverageInit->loadFloat("Alpha", m_opacity);
	m_shaderAverageInit->loadMatrix("uNormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	m_shaderAverageInit->loadMatrix("uModelViewMatrix", m_camera.getProjectionMatrix() *  m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	drawModel();
	m_numGeoPasses++;
	glUseProgram(0);
	
	glDisable(GL_BLEND);

	// ---------------------------------------------------------------------
	// 2. Approximate Blending
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	glUseProgram(m_shaderAverageFinal->m_program);
	m_shaderAverageFinal->loadMatrix("uModelViewMatrix", Matrix4f::IDENTITY);
	m_shaderAverageFinal->loadVector("BackgroundColor", Vector3f(1.0f, 1.0f, 1.0f));

	m_shaderAverageFinal->loadInt("ColorTex0", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_accumulationTexId[0]);

	m_shaderAverageFinal->loadInt("ColorTex1", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_accumulationTexId[1]);

	m_quad->drawRaw();

	glUseProgram(0);
}

void Game::RenderWeightedSum() {
	glDisable(GL_DEPTH_TEST);

	// ---------------------------------------------------------------------
	// 1. Accumulate (alpha * color) and (alpha)
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, g_accumulationFboId);
	glDrawBuffer(g_drawBuffers[0]);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	glUseProgram(m_shaderWeightedInit->m_program);
	m_shaderWeightedInit->loadFloat("Alpha", m_opacity);
	m_shaderWeightedInit->loadMatrix("uNormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));
	m_shaderWeightedInit->loadMatrix("uModelViewMatrix", m_camera.getProjectionMatrix() *  m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	drawModel();
	m_numGeoPasses++;
	glUseProgram(0);

	glDisable(GL_BLEND);

	// ---------------------------------------------------------------------
	// 2. Weighted Sum
	// ---------------------------------------------------------------------

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

	glUseProgram(m_shaderWeightedFinal->m_program);
	m_shaderWeightedFinal->loadMatrix("uModelViewMatrix", Matrix4f::IDENTITY);
	m_shaderWeightedFinal->loadVector("BackgroundColor", Vector3f(1.0f, 1.0f, 1.0f));

	m_shaderWeightedFinal->loadInt("ColorTex", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_accumulationTexId[0]);

	m_quad->drawRaw();

	glUseProgram(0);
}


void Game::InitF2BPeelingRenderTargets() {
	glGenTextures(2, m_frontDepthTexId);
	glGenTextures(2, m_frontColorTexId);
	glGenFramebuffers(2, m_frontFboId);

	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_RECTANGLE, m_frontDepthTexId[i]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32F_NV, Application::Width, Application::Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_RECTANGLE, m_frontColorTexId[i]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, Application::Width, Application::Height,0, GL_RGBA, GL_FLOAT, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_frontFboId[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, m_frontDepthTexId[i], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, m_frontColorTexId[i], 0);
	}

	glGenTextures(1, &m_frontColorBlenderTexId);
	glBindTexture(GL_TEXTURE_RECTANGLE, m_frontColorBlenderTexId);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, 0);

	glGenFramebuffers(1, &m_frontColorBlenderFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frontColorBlenderFboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, m_frontDepthTexId[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, m_frontColorBlenderTexId, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::InitDualPeelingRenderTargets() {
	glGenTextures(2, g_dualDepthTexId);
	glGenTextures(2, g_dualFrontBlenderTexId);
	glGenTextures(2, g_dualBackTempTexId);
	glGenFramebuffers(1, &g_dualPeelingSingleFboId);
	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_RECTANGLE, g_dualDepthTexId[i]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_FLOAT_RG32_NV, Application::Width, Application::Height, 0, GL_RGB, GL_FLOAT, 0);

		glBindTexture(GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[i]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, 0);

		glBindTexture(GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[i]);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, 0);
	}

	glGenTextures(1, &g_dualBackBlenderTexId);
	glBindTexture(GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, Application::Width, Application::Height, 0, GL_RGB, GL_FLOAT, 0);

	glGenFramebuffers(1, &g_dualBackBlenderFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, g_dualBackBlenderFboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingSingleFboId);

	int j = 0;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, g_dualDepthTexId[j], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[j], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[j], 0);

	j = 1;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_RECTANGLE, g_dualDepthTexId[j], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[j], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[j], 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::InitAccumulationRenderTargets() {
	glGenTextures(2, g_accumulationTexId);

	glBindTexture(GL_TEXTURE_RECTANGLE, g_accumulationTexId[0]);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_RECTANGLE, g_accumulationTexId[1]);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_FLOAT_R32_NV, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenFramebuffers(1, &g_accumulationFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, g_accumulationFboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, g_accumulationTexId[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, g_accumulationTexId[1], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::RenderFullscreenQuad() {
	m_quad->drawRaw();
}

void Game::drawModel() {
	switch (model) {
	case BUNNY:
		m_bunny.drawRaw();
		break;
	case DRAGON:
		m_dragon.drawRaw();
		break;
	case BUDDHA:
		m_buddha.drawRaw();
		break;
	}
}