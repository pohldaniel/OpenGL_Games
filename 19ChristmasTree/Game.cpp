#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(40.0f, (double)Application::Width / (double)Application::Height, 0.1f, 100.0f);
	m_camera.lookAt(Vector3f(0.0f, 8.0f, 25.0f), Vector3f(0.0f, 8.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball);

	m_quad = new Quad();
	m_cube = new Cube();

	basic = new Shader("res/basic_vertex.glsl", "res/basic_fragment.glsl");
	ornament = new Shader("res/ornament_vertex.glsl", "res/ornament_fragment.glsl", "res/ornament_geometry.glsl");
	light = new Shader("res/light_vertex.glsl", "res/light_fragment.glsl", "res/light_geometry.glsl");

	defProg = new Shader("res/def_shade_vertex.glsl", "res/def_shade_fragment.glsl");
	defPtProg = new Shader("res/def_shade_pt_vertex.glsl", "res/def_shade_pt_fragment.glsl", "res/def_shade_pt_geometry.glsl");

	displayProg = new Shader("res/pp_vertex.glsl", "res/display_fragment.glsl");
	filterProg = new Shader("res/pp_vertex.glsl", "res/filter_fragment.glsl");
	reflectionProg = new Shader("res/pp_vertex.glsl", "res/reflection_fragment.glsl");

	quad = new Shader("res/quad.vs", "res/quad.fs");
	sky = new Shader("res/skybox.vert", "res/skybox.frag");
	createFBO();
	loadTextures();
	tree.grow();
	tree.genLights();

	m_displayTex = result;
	m_cubemap.setTexture(cubeTex);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
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
	applyTransformation(m_trackball);
};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	if (m_drawSkaybox) {
		glDisable(GL_DEPTH_TEST);


		Matrix4f view = m_camera.getViewMatrix();
		view[3][0] = 0.0f; view[3][1] = 0.0f; view[3][2] = 0.0f;

		glUseProgram(sky->m_program);
		sky->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		sky->loadMatrix("u_view", view);
		sky->loadMatrix("u_model", Matrix4f::IDENTITY);
		m_cubemap.bind(0);
		m_cube->drawRaw();

		Cubemap::Unbind();

		glUseProgram(0);
		glEnable(GL_DEPTH_TEST);

	}else {

		//draw the tree to the FBO
		preDrawTree();

		//reset the viewport, because the FBO rendering will have changed it
		glViewport(0, 0, Application::Width, Application::Height);

		//
		//draw the final tree by tonemapping it directly into the framebuffer
		//
		glBindTexture(GL_TEXTURE_2D, result);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, vFilter);

		glUseProgram(displayProg->m_program);
		displayProg->loadInt("downSampled", 1);
		displayProg->loadFloat("exposure", exposure);
		displayProg->loadFloat("blur", blurVal);

		m_quad->drawRaw();

		glUseProgram(0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_intermediate) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_SCISSOR_TEST);

			glScissor(Application::Width * 0.75f, Application::Height * 0.75f, Application::Width * 0.25f, Application::Height * 0.25f);
			glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(quad->m_program);
			quad->loadMatrix("u_transform", Matrix4f::Translate(0.75f, 0.75f, 0.0f) * Matrix4f::Scale(0.26f, 0.26f, 0.0f));
			m_quad->draw(m_displayTex);
			glUseProgram(0);

			glDisable(GL_SCISSOR_TEST);
		}

		currentTime = (currentTime + 1) % numTimerSamples;
	}

	renderUi();
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
	createFBO();
}

void Game::drawTreeGeometry() {
	//treeTimer.startTimer();
	glUseProgram(basic->m_program);
	basic->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	basic->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
	basic->loadMatrix("u_normal", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_transform.getTransformationMatrix()));

	basic->loadInt("normalTex", 0);
	basic->loadInt("baseTex", 1);

	if (m_drawBranches) {
		//normal map		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dummyNormal); // just faces straight up

												   //diffuse map	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, brownTex);


		tree.drawBranches();
	}

	if (m_drawFronds) {
		//normal map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, frondNormal);

		//diffuse map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, frond);
		tree.drawFronds();
	}
	glUseProgram(0);
	//treeTimer.stopTimer();
}

//
//  Draw light points
//
//////////////////////////////////////////////////////////////////////
void Game::drawLights() {
	//lightsTimer.startTimer();

	if (m_drawLights) {
		glUseProgram(light->m_program);
		light->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		light->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		tree.drawLights();
	}
	//lightsTimer.stopTimer();
}
//
//  Draw deferred lighting
//
//////////////////////////////////////////////////////////////////////
void Game::drawLighting() {
	//lightingTimer.startTimer();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, normal);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, pos);

	// bind deferred shading buffers
	//draw a simple directional light to give base lighting
	glUseProgram(defProg->m_program);
	defProg->loadInt("normalTex", 0);
	defProg->loadInt("materialTex", 1);
	defProg->loadInt("positionTex", 2);

	m_quad->drawRaw();

	if (m_drawLights) {
		//draw light points as point sprites
		glUseProgram(defPtProg->m_program);
		defPtProg->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		defPtProg->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());

		defPtProg->loadInt("normalTex", 0);
		defPtProg->loadInt("materialTex", 1);
		defPtProg->loadInt("positionTex", 2);
		tree.drawLights();
	}

	if (m_drawReflections) {
		// add reflections to image with a full-screen pass
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

		glUseProgram(reflectionProg->m_program);

		reflectionProg->loadInt("normalTex", 0);
		reflectionProg->loadInt("materialTex", 1);
		reflectionProg->loadInt("positionTex", 2);
		reflectionProg->loadInt("cubeTex", 3);

		m_quad->drawRaw();

	}

	//lightingTimer.stopTimer();
}

//
//  Draw ornament geometry into the deferred shading FBO.
//
//////////////////////////////////////////////////////////////////////
void Game::drawOrnamentGeometry() {
	//ornamentTimer.startTimer();

	if (m_drawOrnaments) {

		glUseProgram(ornament->m_program);
		ornament->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		ornament->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_transform.getTransformationMatrix());
		tree.drawOrnaments();
		glUseProgram(0);
	}
	//ornamentTimer.stopTimer();
}

//
//  Draw post-processing effects
//
//////////////////////////////////////////////////////////////////////
void Game::drawPost() {
	//postTimer.startTimer();


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, horFilterFBO);

	glViewport(0, 0, Application::Width >> dsLevel, Application::Height >> dsLevel);

	glBindTexture(GL_TEXTURE_2D, result);

	//downsample the result texture, using gen-mipmaps
	glGenerateMipmapEXT(GL_TEXTURE_2D);

	glUseProgram(filterProg->m_program);
	filterProg->loadVector("direction", Vector2f(1.0f, 0.0f));
	filterProg->loadFloat("sigma", sigmaVal);
	filterProg->loadFloat("level", dsLevel);

	m_quad->drawRaw();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, vertFilterFBO);

	glBindTexture(GL_TEXTURE_2D, hFilter);

	filterProg->loadVector("direction", Vector2f(0.0f, 1.0f));
	filterProg->loadFloat("level", 0.0f);

	m_quad->drawRaw();

	//postTimer.stopTimer();
}

//
//  Draw the tree to the FBO
//
//////////////////////////////////////////////////////////////////////
void Game::preDrawTree() {
	GLenum bufs[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, deferredShadingFBO);

	glViewport(0, 0, Application::Width, Application::Height);

	glDrawBuffers(3, bufs);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	// Draw the eye-space normals and positions for the tree geometry into the shading fbo
	drawTreeGeometry();

	// Draw the ornaments into the shading fbo
	drawOrnamentGeometry();

	//switch to the lighting fbo
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, lightAccumFBO);

	// this allows occluded lights to still illuminate local geometry
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, Application::Width, Application::Height);

	glDrawBuffers(1, bufs);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	// draw the lighting in
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	drawLighting();

	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//draw light points
	glEnable(GL_DEPTH_TEST);

	drawLights();

	glDisable(GL_BLEND);

	//downsample the framebuffer, and apply a gaussian filter
	drawPost();

	glUseProgram(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glViewport(0, 0, Application::Width, Application::Height);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void Game::loadTextures() {
	GLfloat dummy[] = { 0.5f, 0.5f, 1.0f, 0.0f };
	GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat brown[] = { 0.1f, 0.1f, 0.0f, 1.0f };

	glGenTextures(1, &dummyNormal);
	glGenTextures(1, &frondNormal);
	glGenTextures(1, &frond);
	glGenTextures(1, &whiteTex);
	glGenTextures(1, &brownTex);
	//glGenTextures(1, &cubeTex);

	//
	//  simple 'out' normal (z == 1 / x,y == 0)
	//
	glBindTexture(GL_TEXTURE_2D, dummyNormal);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_FLOAT, dummy);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//
	// dummy white texture
	//
	glBindTexture(GL_TEXTURE_2D, whiteTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_FLOAT, white);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//
	// simple dark brown texture for tree trunka and branches
	//
	glBindTexture(GL_TEXTURE_2D, brownTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_FLOAT, brown);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_needles.loadFromFile("res/FraserFirNeedles_MD_1.dds");
	m_needles.setAnisotropy(16.0f);
	m_needles.setLinear();
	frond = m_needles.getTexture();

	m_needlesN.loadFromFile("res/FraserFirNeedles_MD_1_Normal.dds");
	m_needlesN.setLinear();
	frondNormal = m_needlesN.getTexture();

	glBindTexture(GL_TEXTURE_2D, 0);

	//m_hdriCross.loadCrossHDRIFromFile("res/rnl_cross.dds");
	m_hdriCross.loadCrossHDRIFromFile("res/rnl_cross_mmp_s.hdr");
	cubeTex = m_hdriCross.getTexture();

}

//
//
//////////////////////////////////////////////////////////////////////
void Game::createFBO() {

	if (deferredShadingFBO) {
		glDeleteFramebuffersEXT(1, &deferredShadingFBO);
		glDeleteFramebuffersEXT(1, &lightAccumFBO);
		glDeleteFramebuffersEXT(1, &horFilterFBO);
		glDeleteFramebuffersEXT(1, &vertFilterFBO);
		glDeleteTextures(1, &normal);
		glDeleteTextures(1, &pos);
		glDeleteTextures(1, &material);
		glDeleteTextures(1, &result);
		glDeleteTextures(1, &hFilter);
		glDeleteTextures(1, &vFilter);
		glDeleteTextures(1, &depthBuffer);
	}

	glGenFramebuffersEXT(1, &deferredShadingFBO);
	glGenFramebuffersEXT(1, &lightAccumFBO);
	glGenFramebuffersEXT(1, &horFilterFBO);
	glGenFramebuffersEXT(1, &vertFilterFBO);
	glGenTextures(1, &normal);
	glGenTextures(1, &pos);
	glGenTextures(1, &material);
	glGenTextures(1, &result);
	glGenTextures(1, &hFilter);
	glGenTextures(1, &vFilter);
	glGenTextures(1, &depthBuffer);

	//
	// Create the deferred shading FBO (normal, position, material, depth)
	//

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, deferredShadingFBO);

	glBindTexture(GL_TEXTURE_2D, pos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, pos, 0);

	glBindTexture(GL_TEXTURE_2D, normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, normal, 0);

	glBindTexture(GL_TEXTURE_2D, material);
	glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGBA8*/ GL_RGBA16F_ARB, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, material, 0);

	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, Application::Width, Application::Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("Error: bad frame buffer config\n");

	//
	// Light accum FBO
	//

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, lightAccumFBO);

	glBindTexture(GL_TEXTURE_2D, result);
	glTexImage2D(GL_TEXTURE_2D, 0, hdrFormat, Application::Width, Application::Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmapEXT(GL_TEXTURE_2D);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, result, 0);

	//use the same depth buffer as the deferred shading
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, depthBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("Error: bad frame buffer config for resolve\n");

	//
	// Filter FBOs
	// 

	//horizontal filter FBO
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, horFilterFBO);

	glBindTexture(GL_TEXTURE_2D, hFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, hdrFormat, Application::Width >> dsLevel, Application::Height >> dsLevel, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, hFilter, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("Error: bad frame buffer config for vertical filter\n");


	//vertical filter FBO
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, vertFilterFBO);

	glBindTexture(GL_TEXTURE_2D, vFilter);
	glTexImage2D(GL_TEXTURE_2D, 0, hdrFormat, Application::Width >> dsLevel, Application::Height >> dsLevel, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, vFilter, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("Error: bad frame buffer config for vertical filter\n");

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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
	ImGui::Checkbox("Draw Skybox", &m_drawSkaybox);
	ImGui::Checkbox("Draw Fronds", &m_drawFronds);
	ImGui::Checkbox("Draw Branches", &m_drawBranches);
	ImGui::Checkbox("Draw Ornaments", &m_drawOrnaments);
	ImGui::Checkbox("Draw Lights", &m_drawLights);
	ImGui::Checkbox("Draw Reflections", &m_drawReflections);
	ImGui::SliderFloat("Exposure", &exposure, 0.0f, 16.0f);
	ImGui::SliderFloat("Sigma", &sigmaVal, 0.0f, 5.0f);
	ImGui::SliderFloat("Blur Amount", &blurVal, 0.0f, 1.0f);
	if (ImGui::SliderInt("Blur Buffer Size", &dsLevel, 0, 8)) {
		createFBO();
	}

	ImGui::Checkbox("Draw intermediate Texture", &m_intermediate);

	if (m_intermediate) {
		int currentBuffer = m_buffer;
		if (ImGui::Combo("Buffer", &currentBuffer, "Result\0H Filter\0V Filter\0Normal\0Position\0Material\0\0")) {
			m_buffer = static_cast<Buffer>(currentBuffer);

			switch (m_buffer) {
			case RESULT:
				m_displayTex = result;
			case HFILTER:
				m_displayTex = hFilter;
				break;
			case VFILTER:
				m_displayTex = vFilter;
				break;
			case NORMAL:
				m_displayTex = normal;
				break;
			case POSITION:
				m_displayTex = pos;
				break;
			case MATERIAL:
				m_displayTex = material;
				break;

			}
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}