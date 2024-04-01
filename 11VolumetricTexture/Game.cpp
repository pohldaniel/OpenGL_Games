#include "engine/input/EventDispatcher.h"

#include "Game.h"
#include "Application.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	EventDispatcher::AddMouseListener(this);

	m_cube = new Cube();
	m_quad = new Quad();

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 100.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	fluidSys = new FluidSystem(m_width, m_height, m_depth);
	fluidSys->splat();

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setDollyPosition(-5.0f);
	applyTransformation(m_trackball.getTransform());

	Texture::CreateTexture3D(m_result, m_width, m_height, m_depth, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
	Texture::SetFilter3D(m_result, GL_LINEAR);

	cubeCenterVbo = createPointVbo(0, 0, 0);
	cloudTexture = createPyroclasticVolume(128, 0.025f);

	m_intervalls[0].create(Application::Width, Application::Height);
	m_intervalls[0].attachTexture(AttachmentTex::RGBA16F);

	m_intervalls[1].create(Application::Width, Application::Height);
	m_intervalls[1].attachTexture(AttachmentTex::RGBA16F);

	GridVao = CreatePoints();


	m_surface.create(Application::Width, Application::Height);
	m_surface.attachTexture(AttachmentTex::RGB16F);
	m_surface.attachTexture(AttachmentTex::RGB16F);

	PointList positions = CreatePathline();
	SplatTexture = CreateSplat(positions);
	SplatTextureCpu = CreateCpuSplat();
	
	scan[0] = CreateTexture("res/boston_teapot_256x256x178.raw", 256, 256, 178);
	scan[1] = CreateTexture("res/bonsai_256x256x256.raw", 256, 256, 256, false);
	scan[2] = CreateTexture("res/suzanne_128x128x128.raw", 128, 128, 128);

	//approach to generate volume data using blender
	//scan[2] = Create("res/suzanne", 128, 128, 128);
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

	if (keyboard.keyDown(Keyboard::KEY_C)) {
		fluidSys->reset();
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		m_cpuSplat = !m_cpuSplat;
	}

	if (keyboard.keyPressed(Keyboard::KEY_B)) {
		m_drawBorder = !m_drawBorder;
	}

	if (keyboard.keyPressed(Keyboard::KEY_1)) {
		m_currentModel = 0;
	}

	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		m_currentModel = 1;
	}

	if (keyboard.keyPressed(Keyboard::KEY_3)) {
		m_currentModel = 2;
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

	glClearColor(0, 0.25f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	renderOffscreen();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto shader = Globals::shaderManager.getAssetPointer("twopass_ray");
	glUseProgram(shader->m_program);
	shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_tranformCloud2.getTransformationMatrix());
	shader->loadMatrix("Modelview", m_camera.getViewMatrix()* m_tranformCloud2.getTransformationMatrix());
	shader->loadMatrix("ViewMatrix", m_camera.getViewMatrix());
	shader->loadMatrix("ProjectionMatrix", m_camera.getProjectionMatrix());
	shader->loadVector("backgroundColor", Vector4f(0.0f, 0.25f, 0.5f, 1.0f));
	shader->loadInt("RayStartPoints", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_intervalls[0].getColorTexture());
	shader->loadInt("RayStopPoints", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_intervalls[1].getColorTexture());

	shader->loadInt("Density", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, cloudTexture);

	shader->loadVector("EyePosition", m_camera.getPosition());

	glDrawArrays(GL_POINTS, 0, 1);
	glUseProgram(0);
	
	
	glDisable(GL_BLEND);
	if (((rand() & 0xff) > 220)) {
		fluidSys->splat();
	}
	fluidSys->getStateBuffer()->setFiltering(GL_NEAREST);
	fluidSys->step(1.0f);

	glEnable(GL_DEPTH_TEST);
	fluidSys->getStateBuffer()->setFiltering(GL_LINEAR);
	shader = Globals::shaderManager.getAssetPointer("ray_march");
	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_tranformFluid.getTransformationMatrix());
	shader->loadMatrix("u_invModelView", m_tranformFluid.getInvTransformationMatrix() * m_camera.getInvViewMatrix());
	shader->loadFloat("density", 0.01f);
	shader->loadFloat("brightness", 2.0f);

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, fluidSys->getStateBuffer()->getTexture());

	m_cube->drawRaw();
	glUseProgram(0);

	glUseProgram(shader->m_program);
	shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_tranformModel.getTransformationMatrix());
	shader->loadMatrix("u_invModelView", m_tranformModel.getInvTransformationMatrix() * m_camera.getInvViewMatrix());
	shader->loadFloat("density", 0.01f);
	shader->loadFloat("brightness", 2.0f);

	shader->loadInt("u_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, scan[m_currentModel]);

	m_cube->drawRaw();
	glUseProgram(0);

	/////////////////////////////////Compute Raymarcher//////////////////////////////////////////////////
	/**shader = Globals::shaderManager.getAssetPointer("ray_march_c");
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
	
	if (!m_drawBorder) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		auto shader = Globals::shaderManager.getAssetPointer("singlepass");
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_modelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_tranformCloud1.getTransformationMatrix());
		shader->loadMatrix("u_invModelView", m_tranformCloud1.getInvTransformationMatrix() * m_camera.getInvViewMatrix());
		shader->loadInt("Density", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, cloudTexture);

		glBindVertexArray(cubeCenterVbo);

		glDrawArrays(GL_POINTS, 0, 1);

		glDisableVertexAttribArray(0);
		glUseProgram(0);
		glDisable(GL_CULL_FACE);

	}else {
		////////////////////////////////////////////////////////////////////////////////////
		auto shader = Globals::shaderManager.getAssetPointer("ray_march");
		glUseProgram(shader->m_program);
		shader->loadMatrix("u_projection", m_camera.getProjectionMatrix());
		shader->loadMatrix("u_modelView", m_camera.getViewMatrix() * m_tranformCloud1.getTransformationMatrix());
		shader->loadMatrix("u_invModelView", m_tranformCloud1.getInvTransformationMatrix() * m_camera.getInvViewMatrix());

		shader->loadFloat("density", 0.01f);
		shader->loadFloat("brightness", 2.0f);

		shader->loadInt("u_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, cloudTexture);

		m_cube->drawRaw();
		glUseProgram(0);
	}
	////////////////////////////////////////////////////////////////////////////////////

	glBlendFunc(GL_ONE, GL_ONE);
	glDisable(GL_DEPTH_TEST);

	// Update the ray start & stop surfaces:
	shader = Globals::shaderManager.getAssetPointer("endpoint");
	glUseProgram(shader->m_program);
	shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_tranformSplat.getTransformationMatrix());

	glEnable(GL_BLEND);

	m_surface.bind();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_cube->drawRaw();
	m_surface.unbind();

	glDisable(GL_BLEND);
	glUseProgram(0);

	shader = Globals::shaderManager.getAssetPointer("ray");
	glUseProgram(shader->m_program);

	shader->loadMatrix("NormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_tranformSplat.getTransformationMatrix()));
	shader->loadVector("LightPosition", Vector3f(0.25f, 0.25f, 1.0f));
	shader->loadVector("DiffuseMaterial", Vector3f(1.0f, 1.0f, 0.5f));

	shader->loadInt("RayStart", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_surface.getColorTexture(0));

	shader->loadInt("RayStop", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_surface.getColorTexture(1));


	shader->loadInt("Volume", 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, m_cpuSplat ? SplatTextureCpu : SplatTexture);

	shader->loadInt("Noise", 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("noise").getTexture());
	m_quad->drawRaw();
	glUseProgram(0);

	// Draw the point grid:
	glEnable(GL_BLEND);
	shader = Globals::shaderManager.getAssetPointer("streamline");
	glUseProgram(shader->m_program);
	shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_tranformSplat.getTransformationMatrix());
	shader->loadInt("Volume", 2);
	glBindVertexArray(GridVao);
	glDrawArrays(GL_POINTS, 0, GridDensity * GridDensity * GridDensity);
	glDisable(GL_BLEND);
	glUseProgram(0);

	// Draw the cube:
	if (true) {
		shader = Globals::shaderManager.getAssetPointer("wireframe");
		glUseProgram(shader->m_program);
		shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_tranformSplat.getTransformationMatrix());
		shader->loadMatrix("NormalMatrix", Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_tranformSplat.getTransformationMatrix()));
		shader->loadVector("LightPosition", Vector3f(0.25f, 0.25f, 1.0f));
		m_cube->drawRaw();
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
	m_tranformSplat.fromMatrix(mtx);
	m_tranformSplat.translate(3.0f, -2.0f, 0.0f);

	m_tranformFluid.fromMatrix(mtx);
	m_tranformFluid.translate(-3.0f, 2.0f, 0.0f);

	m_tranformCloud1.fromMatrix(mtx);
	m_tranformCloud1.translate(3.0f, 2.0f, 0.0f);

	m_tranformCloud2.fromMatrix(mtx);
	m_tranformCloud2.translate(-3.0f, -2.0f, 0.0f);

	m_tranformModel.fromMatrix(mtx);
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 1000.0f);
}

//http://magnuswrenninge.com/volumetricmethods
unsigned int Game::createPyroclasticVolume(int n, float r) {
	unsigned char *data = new unsigned char[n*n*n];
	unsigned char *ptr = data;

	float frequency = 3.0f / n;
	float center = n / 2.0f + 0.5f;
	PerlinNoise pn = PerlinNoise();
	for (int x = 0; x < n; ++x) {
		for (int y = 0; y < n; ++y) {
			for (int z = 0; z < n; ++z) {
				float dx = center - x;
				float dy = center - y;
				float dz = center - z;

				float off = fabsf((float)pn.cloudNoise(
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

	GLuint handle;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	Texture::CreateTexture3D(handle, n, n, n, GL_LUMINANCE8, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	Texture::SetFilter3D(handle, GL_LINEAR);

	delete[] data;
	return handle;
}

unsigned int Game::createPointVbo(float x, float y, float z) {
	float p[] = { x, y, z };
	
	GLuint vbo;
	glGenBuffers(1, &vbo);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(p), &p[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	return vao;
}

void Game::renderOffscreen() {
	
	m_intervalls[0].bind();

	glBindVertexArray(cubeCenterVbo);

	glEnable(GL_CULL_FACE);
	auto shader = Globals::shaderManager.getAssetPointer("twopass_int");
	glUseProgram(shader->m_program);
	shader->loadMatrix("ModelviewProjection", m_camera.getProjectionMatrix() * m_camera.getViewMatrix() * m_tranformCloud2.getTransformationMatrix());
	shader->loadMatrix("Modelview", m_camera.getViewMatrix()* m_tranformCloud2.getTransformationMatrix());
	shader->loadMatrix("ViewMatrix", m_camera.getViewMatrix());
	shader->loadMatrix("ProjectionMatrix", m_camera.getProjectionMatrix());
	shader->loadInt("RayStartPoints", 1);
	shader->loadInt("RayStopPoints", 2);
	shader->loadVector("EyePosition", m_camera.getPosition());

	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_FRONT);
	glDrawArrays(GL_POINTS, 0, 1);

	m_intervalls[1].bind();
	glClear(GL_COLOR_BUFFER_BIT);
	glCullFace(GL_BACK);
	glDrawArrays(GL_POINTS, 0, 1);


	glDisable(GL_CULL_FACE);
	m_intervalls[1].unbind();	
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
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, pointGrid.size() * sizeof(float), &pointGrid[0], GL_STATIC_DRAW);

	// Set up the vertex layout:
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	return vao;
}

GLuint Game::CreateSplat(PointList positions) {
	const int Size = 64;
	unsigned int m_texture;

	Texture::CreateTexture3D(m_texture, Size, Size, Size, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
	Texture::SetFilter3D(m_texture, GL_LINEAR);
	Framebuffer splatFbo;

	splatFbo.create(64, 64);
	splatFbo.attachTexture(m_texture, Attachment::COLOR, Target::TEXTURE);

	const float innerScale = 0.4f;
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	splatFbo.bind();

	auto shader = Globals::shaderManager.getAssetPointer("splat2");
	glUseProgram(shader->m_program);
	shader->loadFloat("InverseSize", 1.0f / (float)Size);
	shader->loadFloat("InverseVariance", -1.0f / (2.0f * innerScale * innerScale));
	shader->loadFloat("NormalizationConstant", 1.0f / std::pow(std::sqrt(TWO_PI) * innerScale, 3.0f));

	PointList::const_iterator i = positions.begin();
	for (; i != positions.end(); ++i) {

		PointList::const_iterator next = i;
		if (++next == positions.end())
			next = positions.begin();
		Vector3f velocity = (*next - *i);

		shader->loadVector("Center", Vector4f((*i)[0], (*i)[1], (*i)[2], 0.0f));
		shader->loadVector("Color", velocity);
		m_quad->drawRawInstanced(Size);
	}

	glUseProgram(0);

	splatFbo.unbind();
	glDisable(GL_BLEND);
	
	return m_texture;
}

GLuint Game::CreateCpuSplat() {
	const int Size = 64;
	const float InnerScale = 0.4f;
	const float RadiusScale = 0.001f;

	unsigned char* pixels = new unsigned char[Size*Size*Size];
	
	// http://www.stat.wisc.edu/~mchung/teaching/MIA/reading/diffusion.gaussian.kernel.pdf.pdf
	float doubleVariance = 2.0f * InnerScale * InnerScale;
	float normalizationConstant = 1.0f / std::pow(std::sqrt(TWO_PI) * InnerScale, 3.0f);
	unsigned char* pDest = pixels;
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
				maxDensity = (std::max)(maxDensity, density);
				minDensity = (std::max)(minDensity, density);
				sumDensity += density;
				*pDest++ = (unsigned char)(255.0f * density);
			}
		}
	}
	
	GLuint textureHandle;
	Texture::CreateTexture3D(textureHandle, Size, Size, Size, GL_R8, GL_RED, GL_UNSIGNED_BYTE, pixels);
	Texture::SetFilter3D(textureHandle, GL_LINEAR);

	delete[] pixels;
	return textureHandle;
}


PointList Game::CreatePathline() {
	PointList path(64);

	const float dtheta = TWO_PI / float(path.size());
	const float r = 0.5f;

	PointList::iterator i = path.begin();
	for (float theta = 0; i != path.end(); ++i, theta += dtheta) {
		(*i)[0] = r * std::cos(theta);
		(*i)[1] = r * std::sin(theta);
		(*i)[2] = 0.0f;
	}

	return path;
}

GLuint Game::CreateTexture(const char *filename, int width, int height, int depth, bool flipvertical) {
	FILE *filePtr;

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return 0;

	// File has only image data. The dimension of the data should be known.
	unsigned char* data = (unsigned char*)malloc(width * height * depth);

	// verify memory allocation
	if (!data) {
		free(data);
		fclose(filePtr);
		return 0;
	}

	fread(data, 1, width * height * depth, filePtr);
	std::vector<unsigned char> pixel;
	if (flipvertical) {
		for (int i = 0; i < depth - 1; i++) {
			std::vector<unsigned char> subimage(data + width*height * i, data + width*height * (i + 1));
			Texture::FlipVertical(subimage.data(), width, height);
			pixel.insert(pixel.end(), subimage.begin(), subimage.end());
		}
	}else {
		pixel = std::vector<unsigned char>(data, data + width*height * depth);
	}

	GLuint textureHandle;
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_3D, textureHandle);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &pixel[0]);

	free(data);

	return textureHandle;
}

GLuint Game::Create(const char *path, int width, int height, int slices, bool flipvertical) {
	FILE *filePtr;

	std::vector<unsigned char> pixel;
	std::vector<std::string> names;
	std::string search_path = std::string(path) + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				
				unsigned char* data = Texture::LoadFromFile(std::string(path) + "/" + fd.cFileName, false);
				if(flipvertical)
					Texture::FlipVertical(data, width * 4, height);

				pixel.insert(pixel.end(), data , data + width * height * 4);
				free(data);
			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

	if (true) {
		std::vector<unsigned char> pixelNew(width * height * slices);
		for (int i = 0, k = 0; k < width * height * slices * 4; k = k + 4, i++) {
			pixelNew[i] = pixel[k];
		}

		GLuint textureHandle;
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_3D, textureHandle);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_LUMINANCE8, width, height, slices, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &pixelNew[0]);

		std::ofstream binFile("suzanne.raw", std::ios::out | std::ios::binary);
		if (binFile.is_open()){
			binFile.write((char*)pixelNew.data(), width * height * slices * sizeof(char));
			binFile.close();
		}

		return textureHandle;
	}else {
		GLuint textureHandle;
		glGenTextures(1, &textureHandle);
		glBindTexture(GL_TEXTURE_3D, textureHandle);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, slices, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixel[0]);
		return textureHandle;
	}	
}