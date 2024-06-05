#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <States/Menu.h>

#include "Separable.h"
#include "Application.h"
#include "Globals.h"

Separable::Separable(StateMachine& machine) : State(machine, States::SEPARABLE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-1.30592f, 3.5f, 24.11601f), Vector3f(0.00597954f, 3.5f, 0.00461888f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);
	
	Vector3f center = Vector3f(0.0f, 5.0f, 0.0f);

	Globals::textureManager.loadTexture("albedo", "res/models/statue/statue_d.bmp", true);
	Globals::textureManager.loadTexture("normal", "res/models/statue/statue_n.bmp", true);
	Globals::textureManager.loadTexture("beckmann", "res/textures/BeckmannMap.png", false);
	Globals::textureManager.get("albedo").addAlphaChannel();
	Globals::textureManager.get("albedo").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.get("normal").addAlphaChannel(0);
	Globals::textureManager.get("normal").setFilter(GL_LINEAR_MIPMAP_LINEAR);

	Globals::textureManager.get("beckmann").setLinear();

	m_statue.loadModel("res/models/statue/statue.obj", false, false, false, false, true);
	m_statue.getMesh(0)->setMaterialIndex(-1);
	m_statue.getMesh(1)->setMaterialIndex(-1);
	m_statue.markForDelete();

	m_trackball.reshape(Application::Width, Application::Height);
	applyTransformation(m_trackball);

	lights[0].pos = Vector3f(0.0f, 5.0f, 8.0f);
	lights[0].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[0].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[0].m_depthRT.attachTexture2D(AttachmentTex::DEPTH32F);

	lights[0].fov = 45.0f * PI_ON_180;
	lights[0].falloffWidth = 0.05f;
	lights[0].attenuation = 1.0f / 128.0f;
	lights[0].farPlane = 100.0f;
	lights[0].bias = -0.01f;

	lights[1].pos = Vector3f(0.0f, 5.0f, -8.0f);
	lights[1].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[1].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[1].m_depthRT.attachTexture2D(AttachmentTex::DEPTH32F);

	lights[1].fov = 45.0f * PI_ON_180;
	lights[1].falloffWidth = 0.05f;
	lights[1].attenuation = 1.0f / 128.0f;
	lights[1].farPlane = 100.0f;
	lights[1].bias = -0.01f;

	lights[2].pos = Vector3f(8.0f, 5.0f, 0.0f);
	lights[2].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[2].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[2].m_depthRT.attachTexture2D(AttachmentTex::DEPTH32F);

	lights[2].fov = 45.0f * PI_ON_180;
	lights[2].falloffWidth = 0.05f;
	lights[2].attenuation = 1.0f / 128.0f;
	lights[2].farPlane = 100.0f;
	lights[2].bias = -0.01f;

	lights[3].pos = Vector3f(-8.0f, 5.0f, 0.0f);
	lights[3].m_shadowProjection.perspective(45.0f, 1.0, 1.0f, 100.0f);
	lights[3].m_depthRT.create(DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);
	lights[3].m_depthRT.attachTexture2D(AttachmentTex::DEPTH32F);

	lights[3].fov = 45.0f * PI_ON_180;
	lights[3].falloffWidth = 0.05f;
	lights[3].attenuation = 1.0f / 128.0f;
	lights[3].farPlane = 100.0f;
	lights[3].bias = -0.01f;
	
	model = Model::DRAGON;
	switch (model) {
	case Model::STATUE:
		presetStatue();
		break;
	case Model::DRAGON:
		presetDragon();
		break;
	}


	glGenSamplers(1, &sampler1);
	glSamplerParameteri(sampler1, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler1, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler1, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler1, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler1, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glSamplerParameteri(sampler1, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glBindSampler(8, sampler1);

	glGenSamplers(1, &sampler2);
	glSamplerParameteri(sampler2, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler2, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler2, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler2, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glSamplerParameteri(sampler2, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glBindSampler(9, sampler2);

	glGenSamplers(1, &sampler3);
	glSamplerParameteri(sampler3, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler3, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler3, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler3, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler3, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glSamplerParameteri(sampler3, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glBindSampler(10, sampler3);

	glGenSamplers(1, &sampler4);
	glSamplerParameteri(sampler4, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler4, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler4, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler4, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler4, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glSamplerParameteri(sampler4, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glBindSampler(11, sampler4);


	m_lightColor[0] = lights[1].color[0]; m_lightColor[1] = lights[1].color[1]; m_lightColor[2] = lights[1].color[2];

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::SRGBA);
	//I use a hdr/RGBA32F texture cuz the sss strength could be greater one. According to the reference this should be SRGBA as well.
	m_mainRT.attachTexture2D(AttachmentTex::RGBA32F);
	m_mainRT.attachTexture2D(AttachmentTex::RED32F);
	m_mainRT.attachTexture2D(AttachmentTex::RG16F);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachTexture2D(AttachmentTex::DEPTH_STENCIL);

	m_sssXRT.create(Application::Width, Application::Height);
	m_sssXRT.attachTexture2D(AttachmentTex::RGBA);
	m_sssXRT.attachTexture(m_mainRT.getDepthStencilTexture(), Attachment::DEPTH_STENCIL);

	m_sssYRT.create(Application::Width, Application::Height);
	m_sssYRT.attachTexture2D(AttachmentTex::RGBA);
	m_sssYRT.attachTexture(m_mainRT.getDepthStencilTexture(), Attachment::DEPTH_STENCIL);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClearDepth(1.0f);
	glClearStencil(0);

	glDisable(GL_BLEND);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_DITHER);

	nv_dds::CDDSImage albedo;
	albedo.load("res/models/Dragon/DiffuseMap.dds");	
	Globals::textureManager.get("albedo_dragon").setWidth(albedo.get_width());
	Globals::textureManager.get("albedo_dragon").setHeight(albedo.get_height());
	Globals::textureManager.get("albedo_dragon").setFormat(albedo.get_format());
	Globals::textureManager.get("albedo_dragon").setInternalFormat(GL_RGBA8);
	Globals::textureManager.get("albedo_dragon").setType(GL_UNSIGNED_BYTE);
	Globals::textureManager.get("albedo_dragon").setTarget(GL_TEXTURE_2D);
	Globals::textureManager.get("albedo_dragon").setChannels(albedo.get_components());
	Globals::textureManager.get("albedo_dragon").setAnisotropy(16.0f);

	glGenTextures(1, &Globals::textureManager.get("albedo_dragon").getTextureAccess());
	glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("albedo_dragon").getTexture());
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, albedo.get_width(), albedo.get_height(), 0, albedo.get_format(), GL_UNSIGNED_BYTE, albedo);
	
	for (int i = 0; i < albedo.get_num_mipmaps(); i++){
		glTexImage2D(GL_TEXTURE_2D, i+1, GL_RGBA8, albedo.get_mipmap(i).get_width(), albedo.get_mipmap(i).get_height(), 0, albedo.get_format(), GL_UNSIGNED_BYTE, albedo.get_mipmap(i));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	albedo.clear();

	nv_dds::CDDSImage normal;
	normal.load("res/models/Dragon/NormalMap.dds");
	Globals::textureManager.get("normal_dragon").setWidth(normal.get_width());
	Globals::textureManager.get("normal_dragon").setHeight(normal.get_height());
	Globals::textureManager.get("normal_dragon").setFormat(normal.get_format());
	Globals::textureManager.get("normal_dragon").setInternalFormat(GL_RGBA8);
	Globals::textureManager.get("normal_dragon").setType(GL_UNSIGNED_BYTE);
	Globals::textureManager.get("normal_dragon").setTarget(GL_TEXTURE_2D);
	Globals::textureManager.get("normal_dragon").setChannels(normal.get_components());
	Globals::textureManager.get("normal_dragon").setAnisotropy(16.0f);

	glGenTextures(1, &Globals::textureManager.get("normal_dragon").getTextureAccess());
	glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("normal_dragon").getTexture());

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, normal.get_width(), normal.get_height(), 0, normal.get_format(), GL_UNSIGNED_BYTE, normal);

	for (int i = 0; i < normal.get_num_mipmaps(); i++) {
		glTexImage2D(GL_TEXTURE_2D, i + 1, GL_RGBA8, normal.get_mipmap(i).get_width(), normal.get_mipmap(i).get_height(), 0, normal.get_format(), GL_UNSIGNED_BYTE, normal.get_mipmap(i));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	normal.clear(); 

	nv_dds::CDDSImage specular;
	specular.load("res/models/Dragon/SpecularAOMap.dds");
	Globals::textureManager.get("specular_dragon").setWidth(specular.get_width());
	Globals::textureManager.get("specular_dragon").setHeight(specular.get_height());
	Globals::textureManager.get("specular_dragon").setFormat(specular.get_format());
	Globals::textureManager.get("specular_dragon").setInternalFormat(GL_RGBA8);
	Globals::textureManager.get("specular_dragon").setType(GL_UNSIGNED_BYTE);
	Globals::textureManager.get("specular_dragon").setTarget(GL_TEXTURE_2D);
	Globals::textureManager.get("specular_dragon").setChannels(specular.get_components());
	Globals::textureManager.get("specular_dragon").setAnisotropy(16.0f);

	glGenTextures(1, &Globals::textureManager.get("specular_dragon").getTextureAccess());
	glBindTexture(GL_TEXTURE_2D, Globals::textureManager.get("specular_dragon").getTexture());

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specular.get_width(), specular.get_height(), 0, specular.get_format(), GL_UNSIGNED_BYTE, specular);

	for (int i = 0; i < specular.get_num_mipmaps(); i++) {
		glTexImage2D(GL_TEXTURE_2D, i + 1, GL_RGBA8, specular.get_mipmap(i).get_width(), specular.get_mipmap(i).get_height(), 0, specular.get_format(), GL_UNSIGNED_BYTE, specular.get_mipmap(i));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	specular.clear();


	m_dragon.loadModel("res/models/Dragon/dragon.obj", Vector3f(0.0f, 1.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 5.0f, false, false, false, false, true);
	m_dragon.getMesh(0)->setMaterialIndex(-1);
	m_dragon.markForDelete();

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", Matrix4f::IDENTITY);
	shader->loadMatrix("u_view", Matrix4f::IDENTITY);
	shader->unuse();
}

Separable::~Separable() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	glDeleteSamplers(1, &sampler1);
	sampler1 = 0u;

	glDeleteSamplers(1, &sampler2);
	sampler2 = 0u;

	glDeleteSamplers(1, &sampler3);
	sampler3 = 0u;

	glDeleteSamplers(1, &sampler4);
	sampler4 = 0u;

	Globals::textureManager.erase("albedo");
	Globals::textureManager.erase("normal");
	Globals::textureManager.erase("beckmann");

	Globals::textureManager.erase("albedo_dragon");
	Globals::textureManager.erase("normal_dragon");
	Globals::textureManager.erase("specular_dragon");
	glEnable(GL_BLEND);

	Material::CleanupMaterials();
}

void Separable::fixedUpdate() {

}

void Separable::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_debug = !m_debug;
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		m_sss = !m_sss;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
	}

	m_trackball.idle();
	applyTransformation(m_trackball);
};

void Separable::render() {
	
	glEnable(GL_FRAMEBUFFER_SRGB);
	shdowPass();
	renderGBuffer();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_sss)
		sssPass();

	if(m_addSpecular)
		addSpecular();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_model", m_debug ? Matrix4f::Translate(0.25f, 0.0f, 0.0f) : Matrix4f::IDENTITY);

	m_sss ? m_sssYRT.bindColorTexture() : m_mainRT.bindColorTexture();
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();

	if (m_debug) {
		glEnable(GL_SCISSOR_TEST);

		glScissor(0, 0, Application::Width * 0.25, Application::Height);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		shader->loadMatrix("u_model", Matrix4f::Translate(-0.75f, 0.0f, 0.0f));
		m_mainRT.bindColorTexture(1u);
		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();

		glScissor(Application::Width * 0.25, 0, Application::Width * 0.25, Application::Height);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		shader->loadMatrix("u_model", Matrix4f::Translate(-0.25f, 0.0f, 0.0f));
		m_mainRT.bindColorTexture();
		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();

		glScissor(Application::Width * 0.75f, 0, Application::Width * 0.25, Application::Height);
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();
		shader->loadMatrix("u_model", Matrix4f::Translate(0.75f, 0.0f, 0.0f));
		m_mainRT.bindColorTexture(4u);
		Globals::shapeManager.get("quad").drawRaw();
		shader->unuse();

		glDisable(GL_SCISSOR_TEST);
	}
	glDisable(GL_FRAMEBUFFER_SRGB);

	/*glEnable(GL_FRAMEBUFFER_SRGB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadInt("u_texture", 0);
	shader->loadInt("u_depth", 1);
	//Globals::textureManager.get("albedo_dragon").bind();

	Globals::textureManager.get("specular_dragon").bind(0);

	m_dragon.drawRaw();
	shader->unuse();
	glDisable(GL_FRAMEBUFFER_SRGB);*/
	if (m_drawUi)
		renderUi();
}

void Separable::shdowPass() {

	for (int i = 0; i < 4; i++) {
		lights[i].m_depthRT.bind();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto shader = Globals::shaderManager.getAssetPointer("depth_sep");
		shader->use();
		shader->loadMatrix("u_viewProjection", lights[i].m_shadowProjection * lights[i].m_shadowView);
		shader->loadMatrix("u_model", m_transform.getTransformationMatrix());

		switch (model) {
		case Model::STATUE:
			m_statue.drawRaw();
			break;
		case Model::DRAGON:
			m_dragon.drawRaw();
			break;
		}

		lights[i].m_depthRT.unbind();
	}
}

void Separable::renderGBuffer() {

	if (!m_showBlurRadius) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	m_mainRT.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearTexImage(m_mainRT.getColorTexture(5), 0, GL_RED, GL_FLOAT, clear);
	//glClearBufferfv(GL_COLOR, 5, clear);

	glClearTexImage(m_mainRT.getColorTexture(1), 0, GL_RGBA, GL_FLOAT, m_spec);
	//glClearTexImage(m_mainRT.getColorTexture(1), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_specu);

	
	auto shader = Globals::shaderManager.getAssetPointer("main_sep");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_modelView", m_camera.getViewMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", m_transform.getTransformationMatrix());
	shader->loadMatrix("u_normalMatrix", Matrix4f::GetNormalMatrix(m_transform.getTransformationMatrix()));
	shader->loadVector("u_color", Vector4f(m_albedoColor[0], m_albedoColor[1], m_albedoColor[2], 1.0f));
	shader->loadVector("u_cameraPosition", m_camera.getPosition());
	currViewProj = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix();

	shader->loadMatrix("u_currWorldViewProj", currViewProj);
	shader->loadMatrix("u_prevWorldViewProj", prevViewProj);

	shader->loadFloat("sssWidth", m_sssWidth);
	shader->loadFloat("translucency", m_translucency);
	shader->loadFloat("specularIntensity", m_specularIntensity);
	shader->loadFloat("specularRoughness", m_specularRoughness);
	shader->loadFloat("specularFresnel", m_specularFresnel);
	shader->loadFloat("weight", m_weight);
	shader->loadFloat("bumpiness", m_bumpiness);
	shader->loadFloat("translucencyStrength", m_translucencyStrength);
	shader->loadFloat("u_strength", m_strength);
	shader->loadBool("tangentSpace", m_tangentSpaceNormal);
	shader->loadBool("specularAOMap", m_specularAOMap);
	shader->loadBool("u_shadow", m_shadow);
	
	for (int i = 0; i < 4; i++) {
		shader->loadMatrix(std::string("u_projectionShadowBias[" + std::to_string(i) + "]").c_str(), Matrix4f::BIAS * lights[i].m_shadowProjection);
		shader->loadMatrix(std::string("u_projectionShadow[" + std::to_string(i) + "]").c_str(), lights[i].m_shadowProjection);
		shader->loadMatrix(std::string("u_viewShadow[" + std::to_string(i) + "]").c_str(), lights[i].m_shadowView);


		shader->loadVector(std::string("lightPosition[" + std::to_string(i) + "]").c_str(), lights[i].pos);
		shader->loadVector(std::string("lightDirection[" + std::to_string(i) + "]").c_str(), lights[i].viewDirection);
		shader->loadVector(std::string("lightColor[" + std::to_string(i) + "]").c_str(), lights[i].color);
		shader->loadFloat(std::string("lightFalloffStart[" + std::to_string(i) + "]").c_str(), cos(0.5f * (45.0f * PI_ON_180)));

		shader->loadInt(std::string("u_depthMap[" + std::to_string(i) + "]").c_str(), i);
		shader->loadInt(std::string("u_shadowMap[" + std::to_string(i) + "]").c_str(), i + 8);
		lights[i].m_depthRT.bindDepthTexture(i);
		lights[i].m_depthRT.bindDepthTexture(i + 8);
	}

	shader->loadInt("u_albedo", 4);
	shader->loadInt("u_normal", 5);
	shader->loadInt("u_specular", 6);
	shader->loadInt("u_beckmann", 7);

	switch (model) {
	case Model::STATUE:
		Globals::textureManager.get("albedo").bind(4);
		Globals::textureManager.get("normal").bind(5);
		Globals::textureManager.get("beckmann").bind(7);
		m_statue.drawRaw();
		break;
	case Model::DRAGON:
		Globals::textureManager.get("albedo_dragon").bind(4);
		Globals::textureManager.get("normal_dragon").bind(5);
		Globals::textureManager.get("specular_dragon").bind(6);
		Globals::textureManager.get("beckmann").bind(7);
		m_dragon.drawRaw();
		break;
	}

	shader->unuse();

	m_mainRT.unbind();
	prevViewProj = currViewProj;

	if (!m_showBlurRadius) {
		glDisable(GL_STENCIL_TEST);
	}
}

void Separable::sssPass() {
	if (!m_showBlurRadius) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);
	}

	m_sssXRT.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	auto shader = Globals::shaderManager.getAssetPointer("sss_sep");
	shader->use();
	shader->loadVector("dir", Vector2f((GLfloat)Application::Height / (GLfloat)Application::Width, 0.0f));
	shader->loadFloat("u_showBlurRadius", m_showBlurRadius);
	shader->loadFloat("sssWidth", m_sssWidth);
	shader->loadBool("u_followSurface", m_folowSurface);

	shader->loadInt("u_colorTex", 0);
	shader->loadInt("u_strengthTex", 1);
	shader->loadInt("u_depthTex", 2);
	
	m_mainRT.bindColorTexture(0u, 0u, true);
	m_mainRT.bindColorTexture(1u, 1u, true);
	m_mainRT.bindColorTexture(2u, 2u, true);

	Globals::shapeManager.get("quad").drawRaw();

	m_sssYRT.bind();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shader->loadVector("dir", Vector2f(0.0f, 1.0f));

	shader->loadInt("u_colorTex", 0);
	m_sssXRT.bindColorTexture(0u, 0u, true);

	Globals::shapeManager.get("quad").drawRaw();

	shader->unuse();
	m_sssYRT.unbind();

	if (!m_showBlurRadius) {
		glDisable(GL_STENCIL_TEST);
	}
}

void Separable::addSpecular() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	m_sss ? m_sssYRT.bindWrite() : m_mainRT.bindWrite();
	auto shader = Globals::shaderManager.getAssetPointer("spec_sep");
	shader->use();
	shader->loadInt("u_texture", 0);
	m_mainRT.bindColorTexture(1u, 0u, true);
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();
	m_sss ? m_sssYRT.unbind() : m_mainRT.unbind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

void Separable::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Separable::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Separable::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Separable::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Separable::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Separable::OnKeyUp(Event::KeyboardEvent& event) {

}

void Separable::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Separable::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

	m_mainRT.resize(Application::Width, Application::Height);
	m_sssXRT.resize(Application::Width, Application::Height);
	m_sssYRT.resize(Application::Width, Application::Height);
}

void Separable::renderUi() {
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
	ImGui::Checkbox("Draw Debug", &m_debug);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("SSS", &m_sss);
	if (ImGui::Checkbox("Show Blurradius", &m_showBlurRadius)) {
		if (m_showBlurRadius) {
			m_wasFollow = m_folowSurface;
			m_folowSurface = false;
		}else {
			m_folowSurface = m_wasFollow;
		}
	}
	ImGui::SliderFloat("SSS Width", &m_sssWidth, 5.0f, 1000.0f);
	ImGui::SliderFloat("Translucency", &m_translucency, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Intensity", &m_specularIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Roughness", &m_specularRoughness, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Fresnel", &m_specularFresnel, 0.0f, 1.0f);
	ImGui::SliderFloat("Bumpiness", &m_bumpiness, 0.0f, 1.0f);
	ImGui::Checkbox("Add Specular", &m_addSpecular);
	ImGui::Checkbox("Follow Surface", &m_folowSurface);
	ImGui::Checkbox("Enable Sahdow", &m_shadow);

	ImGui::NewLine();
	ImGui::SliderFloat("Strength", &m_strength, 2.0f, 50.0f);
	ImGui::SliderFloat("Weight", &m_weight, 1.0f, 100.0f);
	ImGui::SliderFloat("Translucency Strength", &m_translucencyStrength, 1.0f, 1000.0f);
	
	if (ImGui::ColorEdit3("Light Color", m_lightColor)) {

		switch (model) {
		case Model::STATUE:
			lights[1].color[0] = m_lightColor[0]; lights[0].color[1] = m_lightColor[1]; lights[0].color[2] = m_lightColor[2];
			break;
		case Model::DRAGON:
			//lights[0].color[0] = m_lightColor[0]; lights[0].color[1] = m_lightColor[1]; lights[0].color[2] = m_lightColor[2];
			lights[1].color[0] = m_lightColor[0]; lights[1].color[1] = m_lightColor[1]; lights[1].color[2] = m_lightColor[2];
			//lights[2].color[0] = m_lightColor[0]; lights[2].color[1] = m_lightColor[1]; lights[2].color[2] = m_lightColor[2];
			//lights[3].color[0] = m_lightColor[0]; lights[3].color[1] = m_lightColor[1]; lights[3].color[2] = m_lightColor[2];
			break;
		}

	}

	ImGui::ColorEdit3("Albedo Color", m_albedoColor);
	
	int currentModel = model;
	if (ImGui::Combo("Model", &currentModel, "Statue\0Dragon\0\0")) {
		model = static_cast<Model>(currentModel);

		switch (model) {
		case Model::STATUE:
			presetStatue();
			break;
		case Model::DRAGON:
			presetDragon();
			break;
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Separable::presetDragon() {
	m_tangentSpaceNormal = false;
	m_specularAOMap = true;
	m_sssWidth = 10.0f;
	m_translucency = 0.1f;
	m_specularIntensity = 1.0f;
	m_specularRoughness = 0.100f;
	m_specularFresnel = 0.409f;
	m_weight = 4.0f;
	m_strength = 20.0f;
	m_translucencyStrength = 5.0;

	m_center = Vector3f(0.0f, 5.0f, 0.0f);

	lights[0].color = Vector3f(1.0f, 1.0f, 1.0f);
	lights[0].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[0].pos);
	lights[0].m_shadowView.lookAt(lights[0].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));


	lights[1].color = Vector3f(1.0f, 1.0f, 1.0f);
	lights[1].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[1].pos);
	lights[1].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));


	lights[2].color = Vector3f(1.0f, 1.0f, 1.0f);
	lights[2].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[2].pos);
	lights[2].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));

	lights[3].color = Vector3f(1.0f, 1.0f, 1.0f);
	lights[3].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[3].pos);
	lights[3].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-1.30592f, 5.0f, 8.11601f), Vector3f(0.00597954f, 5.0f, 0.00461888f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.setDollyPosition(m_center);
	m_trackball.setCenterOfRotation(Vector3f(0.0f, 0.0f, 0.0f));
	m_trackball.reset();
}

void Separable::presetStatue() {
	m_tangentSpaceNormal = true;
	m_specularAOMap = false;
	m_sssWidth = 50.0f;
	m_translucency = 0.65f;
	m_specularIntensity = 1.0f;
	m_specularRoughness = 0.08f;
	m_specularFresnel = 0.81f;
	m_weight = 2.0f;
	m_strength = 10.0f;
	m_translucencyStrength = 5.0;

	m_center = m_statue.getCenter();

	lights[0].color = Vector3f(0.3f, 0.3f, 0.3f);
	lights[0].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[0].pos);
	lights[0].m_shadowView.lookAt(lights[0].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));


	lights[1].color = Vector3f(0.0f, 1.0f, 0.5f);
	lights[1].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[1].pos);
	lights[1].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));


	lights[2].color = Vector3f(0.3f, 0.3f, 0.3f);
	lights[2].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[2].pos);
	lights[2].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));

	lights[3].color = Vector3f(0.3f, 0.3f, 0.3f);
	lights[3].viewDirection = Vector3f::Normalize(m_transform.getTransformationMatrix() * m_center - lights[3].pos);
	lights[3].m_shadowView.lookAt(lights[1].pos, m_transform.getTransformationMatrix() * m_center, Vector3f(0.0f, 1.0f, 0.0f));

	m_lightColor[0] = lights[1].color[0]; m_lightColor[1] = lights[1].color[1]; m_lightColor[2] = lights[1].color[2];

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-1.30592f, 3.5f, 4.11601f), Vector3f(0.00597954f, 3.5f, 0.00461888f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	m_trackball.setDollyPosition(Vector3f(0.0f, 0.0f, 0.0f));
	m_trackball.setCenterOfRotation(Vector3f(0.0f, 2.37866f, 0.0f));
	m_trackball.reset();
}