#include "Game.h"
#include "engine/input/EventDispatcher.h"
#include "Application.h"
#include "Perlin.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_cube = new Cube();
	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 100.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 10.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	fluidSys = new FluidSystem(m_width, m_height, m_depth);
	fluidSys->splat();
	trackball.reshape(Application::Width, Application::Height);
	trackball.setDollyPosition(-5.0f);
	applyTransformation(trackball.getTransform());
	Texture::CreateEmptyTexture3D(m_result, m_width, m_height, m_depth, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
	Texture::SetLFilter3D(m_result, GL_LINEAR);

	cubeCenterVbo = createPointVbo(0, 0, 0);
	cloudTexture = createPyroclasticVolume(128, 0.025f);

	m_intervalls[0].create(Application::Width, Application::Height);
	m_intervalls[0].attachTexture(AttachmentTex::RGBA16F);

	m_intervalls[1].create(Application::Width, Application::Height);
	m_intervalls[1].attachTexture(AttachmentTex::RGBA16F);

	GridVao = CreatePoints();
	QuadVao = CreateQuad();
	CubeVao = CreateCube();

	m_endpoit = new Shader("res/shader/compute/endpoint.vs", "res/shader/compute/endpoint.fs");
	m_ray = new Shader("res/shader/compute/ray.vs", "res/shader/compute/ray.fs");
	m_wireframe = new Shader("res/shader/compute/wireframe.vs", "res/shader/compute/wireframe.fs", "res/shader/compute/wireframe.gs");
	m_streamline = new Shader("res/shader/compute/streamline.vs", "res/shader/compute/streamline.fs", "res/shader/compute/streamline.gs");
	m_splat2 = new Shader("res/shader/compute/splat2.vs", "res/shader/compute/splat2.fs", "res/shader/compute/splat2.gs");

	fbo2 = CreateSurface(PEZ_VIEWPORT_WIDTH, PEZ_VIEWPORT_HEIGHT, texture1, texture2);
	SplatTexture = CreateCpuSplat(QuadVao);

	const float HalfWidth = 0.5f;
	const float HalfHeight = HalfWidth * PEZ_VIEWPORT_HEIGHT / PEZ_VIEWPORT_WIDTH;

	m_projection = Matrix4f::GetPerspective(-HalfWidth, +HalfWidth, -HalfHeight, +HalfHeight, 2.0f, 70.0f);
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

	if (keyboard.keyDown(Keyboard::KEY_C)) {
		fluidSys->reset();
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

	trackball.idle();
	applyTransformation(trackball.getTransform());

};

void Game::render(unsigned int &frameBuffer) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (((rand() & 0xff) > 220)) {
		fluidSys->splat();
	}
	fluidSys->getStateBuffer()->setFiltering(GL_NEAREST);
	fluidSys->step(1.0f);


	fluidSys->getStateBuffer()->setFiltering(GL_LINEAR);
	auto shader = Globals::shaderManager.getAssetPointer("ray_march");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_model);
	shader->loadMatrix("u_invModelView", m_invModel * m_camera.getInvViewMatrix());

	shader->loadFloat("density", 0.01f);
	shader->loadFloat("brightness", 2.0f);

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fluidSys->getStateBuffer()->getTexture());

	m_cube->drawRaw();
	glUseProgram(0);

	///////////////////////////////////////////////////////////////////////////////////
	/*shader = Globals::shaderManager.getAssetPointer("ray_march_c");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_model);
	shader->loadMatrix("u_invModelView", m_invModel * m_camera.getInvViewMatrix());

	shader->loadFloat("density", 0.01f);
	shader->loadFloat("brightness", 2.0f);

	glBindImageTexture(0, fluidSys->getStateBuffer()->getTexture(), 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(1, m_result, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);;
	glDispatchCompute(m_width / 8, m_height / 8, m_depth / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);

	shader = Globals::shaderManager.getAssetPointer("cube");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_model);

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_result);

	m_cube->drawRaw();
	glUseProgram(0);*/
	///////////////////////////////////////////////////////////////////////////////////
	/*glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto shader = Globals::shaderManager.getAssetPointer("singlepass");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_modelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_model);
	shader->loadMatrix("u_invModelView", m_invModel * m_camera.getInvViewMatrix());
	shader->loadInt("Density", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, cloudTexture);

	glBindBuffer(GL_ARRAY_BUFFER, cubeCenterVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_POINTS, 0, 1);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	shader = Globals::shaderManager.getAssetPointer("ray_march");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_model);
	shader->loadMatrix("u_invModelView", m_invModel * m_camera.getInvViewMatrix());

	shader->loadFloat("density", 0.01f);
	shader->loadFloat("brightness", 2.0f);

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, cloudTexture);

	m_cube->drawRaw();
	glUseProgram(0);*/

	/*renderOffscreen();

	glBindTexture(GL_TEXTURE_3D, cloudTexture);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto shader = Globals::shaderManager.getAssetPointer("twopass_ray");
	glUseProgram(shader->m_program);
	shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_model);
	shader->loadMatrix("Modelview", m_camera.getViewMatrix()* m_model);
	shader->loadMatrix("ViewMatrix", m_camera.getViewMatrix());
	shader->loadMatrix("ProjectionMatrix", m_camera.getProjectionMatrix());
	shader->loadInt("RayStartPoints", 1);
	shader->loadInt("RayStopPoints", 2);
	shader->loadVector("EyePosition", m_camera.getPosition());
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_intervalls[0].getColorTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_intervalls[1].getColorTexture());
	glClearColor(0.2f, 0.2f, 0.2f, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_POINTS, 0, 1);
	glUseProgram(0);*/


	/*glClearColor(0, 0.25f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	// Update the ray start & stop surfaces:
	glUseProgram(m_endpoit->m_program);
	m_endpoit->loadMatrix("ModelviewProjection", m_projection * m_camera.getViewMatrix() * m_model);

	glEnable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(CubeVao);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_BLEND);
	glUseProgram(0);

	glUseProgram(m_ray->m_program);

	m_ray->loadMatrix("NormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_model));
	m_ray->loadVector("LightPosition", Vector3f(0.25f, 0.25f, 1.0f));
	m_ray->loadVector("DiffuseMaterial", Vector3f(1.0f, 1.0f, 0.5f));

	m_ray->loadInt("RayStart", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	m_ray->loadInt("RayStop", 1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE1);

	m_ray->loadInt("Volume", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, SplatTexture);


	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, texture2);
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_3D, SplatTexture);
	glBindVertexArray(QuadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Draw the point grid:
	glEnable(GL_BLEND);
	m_streamline = Globals::shaderManager.getAssetPointer("streamline");
	glUseProgram(m_streamline->m_program);
	m_streamline->loadMatrix("ModelviewProjection", m_projection * m_camera.getViewMatrix() * m_model);
	m_streamline->loadInt("Volume", 2);
	glBindVertexArray(GridVao);
	glDrawArrays(GL_POINTS, 0, GridDensity * GridDensity * GridDensity);
	glDisable(GL_BLEND);
	glUseProgram(0);
	// Draw the cube:
	if (true) {
		//BindProgram(WireframeProgram);
		glUseProgram(m_wireframe->m_program);
		m_wireframe->loadMatrix("ModelviewProjection", m_projection * m_camera.getViewMatrix() * m_model);
		m_wireframe->loadMatrix("NormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_model));
		m_wireframe->loadVector("LightPosition", Vector3f(0.25f, 0.25f, 1.0f));


		glBindVertexArray(CubeVao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	}*/
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	trackball.motion(event.x, event.y);
	nv::matrix4f mtx = trackball.getTransform();
	applyTransformation(trackball.getTransform());

}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	trackball.mouse(0, 0, event.x, event.y);
	nv::matrix4f mtx = trackball.getTransform();
	applyTransformation(trackball.getTransform());
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	trackball.mouse(0, 1, event.x, event.y);
	nv::matrix4f mtx = trackball.getTransform();
	applyTransformation(trackball.getTransform());
}

void Game::applyTransformation(nv::matrix4f& mtx) {
	m_model.set(mtx._11, mtx._12, mtx._13, mtx._14,
		mtx._21, mtx._22, mtx._23, mtx._24,
		mtx._31, mtx._32, mtx._33, mtx._34,
		mtx._41, mtx._42, mtx._43, mtx._44);

	m_invModel.set(m_model[0][0], m_model[1][0], m_model[2][0], 0.0f,
		m_model[0][1], m_model[1][1], m_model[2][1], 0.0f,
		m_model[0][2], m_model[1][2], m_model[2][2], 0.0f,
		-(m_model[3][0] * m_model[0][0] + m_model[3][1] * m_model[0][1] + m_model[3][2] * m_model[0][2]),
		-(m_model[3][0] * m_model[1][0] + m_model[3][1] * m_model[1][1] + m_model[3][2] * m_model[1][2]),
		-(m_model[3][0] * m_model[2][0] + m_model[3][1] * m_model[2][1] + m_model[3][2] * m_model[2][2]), 1.0f);

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
}

unsigned int Game::createPyroclasticVolume(int n, float r) {
	GLuint handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_3D, handle);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	init();
	unsigned char *data = new unsigned char[n*n*n];
	unsigned char *ptr = data;

	float frequency = 3.0f / n;
	float center = n / 2.0f + 0.5f;

	for (int x = 0; x < n; ++x) {
		for (int y = 0; y < n; ++y) {
			for (int z = 0; z < n; ++z) {
				float dx = center - x;
				float dy = center - y;
				float dz = center - z;

				float off = fabsf((float)PerlinNoise3D(
					x*frequency,
					y*frequency,
					z*frequency,
					5,
					6, 3));

				float d = sqrtf(dx*dx + dy*dy + dz*dz) / (n);
				bool isFilled = (d - off) < r;
				*ptr++ = isFilled ? 255 : 0;
			}
		}
	}

	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE, n, n, n, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);

	delete[] data;
	return handle;
}

unsigned int Game::createPointVbo(float x, float y, float z) {
	float p[] = { x, y, z };
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(p), &p[0], GL_STATIC_DRAW);
	return vbo;
}

void Game::renderOffscreen() {
	glEnable(GL_CULL_FACE);
	glBindBuffer(GL_ARRAY_BUFFER, cubeCenterVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	auto shader = Globals::shaderManager.getAssetPointer("twopass_int");
	glUseProgram(shader->m_program);
	shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix()* m_model);
	shader->loadMatrix("Modelview", m_camera.getViewMatrix()* m_model);
	shader->loadMatrix("ViewMatrix", m_camera.getViewMatrix());
	shader->loadMatrix("ProjectionMatrix", m_camera.getProjectionMatrix());
	shader->loadInt("RayStartPoints", 1);
	shader->loadInt("RayStopPoints", 2);
	shader->loadVector("EyePosition", m_camera.getPosition());

	Vector4f rayOrigin(Matrix4f::Transpose(m_camera.getViewMatrix()) * m_camera.getPosition());
	shader->loadVector("RayOrigin", Vector3f(rayOrigin[0], rayOrigin[1], rayOrigin[2]));

	float _focalLength = 1.0f / std::tan(0.7f / 2);
	shader->loadFloat("FocalLength", _focalLength);
	shader->loadVector("WindowSize", Vector2f(float(Application::Width), float(Application::Height)));

	m_intervalls[0].bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_POINTS, 0, 1);


	m_intervalls[1].bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_BACK);
	glDrawArrays(GL_POINTS, 0, 1);



	m_intervalls[1].unbind();
	glDisable(GL_CULL_FACE);
}

GLenum* Game::EnumArray(GLenum a, GLenum b) {
	static GLenum enums[2];
	enums[0] = a;
	enums[1] = b;
	return &enums[0];
}

GLuint Game::CreateSurface(GLsizei width, GLsizei height, unsigned int& tex1, unsigned int& tex2) {
	unsigned int m_fbo;

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &tex1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_HALF_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex1, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &tex2);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_HALF_FLOAT, 0);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex2, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawBuffers(2, EnumArray(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1));
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return  m_fbo;
}

GLuint Game::CreateQuad()
{
	short positions[] = {
		-1, -1,
		1, -1,
		-1,  1,
		1,  1,
	};

	// Create the VAO:
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create the VBO:
	GLuint vbo;
	GLsizeiptr size = sizeof(positions);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);

	// Set up the vertex layout:
	GLsizeiptr stride = 2 * sizeof(positions[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, stride, 0);

	return vao;
}

GLuint Game::CreateCube()
{
#define O -1.0f,
#define X +1.0f,
	float positions[] = { O O O O O X O X O O X X X O O X O X X X O X X X };
#undef O
#undef X

	short indices[] = {
		7, 3, 1, 1, 5, 7, // Z+
		0, 2, 6, 6, 4, 0, // Z-
		6, 2, 3, 3, 7, 6, // Y+
		1, 0, 4, 4, 5, 1, // Y-
		3, 2, 0, 0, 1, 3, // X-
		4, 6, 7, 7, 5, 4, // X+
	};

	// Create the VAO:
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create the VBO for positions:
	{
		GLuint vbo;
		GLsizeiptr size = sizeof(positions);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
	}

	// Create the VBO for indices:
	{
		GLuint vbo;
		GLsizeiptr size = sizeof(indices);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	// Set up the vertex layout:
	GLsizeiptr stride = 3 * sizeof(positions[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	return vao;
}

GLuint Game::CreateCpuSplat(GLuint quadVao)
{
	const int Size = 64;
	const float InnerScale = 0.4f;
	const float RadiusScale = 0.001f;

	char* pixels = new char[Size*Size*Size];
	{
		// http://www.stat.wisc.edu/~mchung/teaching/MIA/reading/diffusion.gaussian.kernel.pdf.pdf
		float doubleVariance = 2.0f * InnerScale * InnerScale;
		float normalizationConstant = 1.0f / std::pow(std::sqrt(TWO_PI) * InnerScale, 3.0f);
		char* pDest = pixels;
		float maxDensity = 0;
		float sumDensity = 0;
		float minDensity = 100.0f;
		for (int z = 0; z < Size; ++z) {
			for (int y = 0; y < Size; ++y) {
				for (int x = 0; x < Size; ++x) {
					int cx = x - Size / 2;
					int cy = y - Size / 2;
					int cz = z - Size / 2;
					float r2 = RadiusScale * float(cx*cx + cy*cy + cz*cz);
					float density = normalizationConstant * std::exp(-r2 / doubleVariance);
					maxDensity = std::max(maxDensity, density);
					minDensity = std::min(minDensity, density);
					sumDensity += density;
					*pDest++ = (char)(255.0f * density);
				}
			}
		}
	}

	GLuint textureHandle;
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_3D, textureHandle);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, Size, Size, Size, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;

	return textureHandle;
}

GLuint Game::CreatePoints() {
	const int size = GridDensity;
	std::vector<float> pointGrid(size * size * size * 3);
	std::vector<float>::iterator pPoint = pointGrid.begin();
	for (int z = 0; z < size; ++z) {
		for (int y = 0; y < size; ++y) {
			for (int x = 0; x < size; ++x) {
				*pPoint++ = -1 + 2 * float(x) / (size - 1);
				*pPoint++ = -1 + 2 * float(y) / (size - 1);
				*pPoint++ = -1 + 2 * float(z) / (size - 1);
			}
		}
	}

	// Create the VAO:
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create the VBO:
	GLuint vbo;
	GLsizeiptr byteCount = pointGrid.size() * sizeof(float);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, byteCount, &pointGrid[0], GL_STATIC_DRAW);

	// Set up the vertex layout:
	GLsizeiptr stride = 3 * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	return vao;
}