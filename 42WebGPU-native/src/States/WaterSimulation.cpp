#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <engine/input/GamePad.h>
#include <States/Menu.h>

#include <WebGPU/WgpRenderer.h>

#include "Globals.h"
#include "Application.h"
#include "WaterSimulation.h"

WaterSimulation::WaterSimulation(StateMachine& machine) : State(machine, States::WATER_SIMULATION) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f , -1.0f, 1.0f);


	m_camera.lookAt(4.0f, 25.0f, 200.5f);
	m_camera.move(0.0f, -0.5f, 0.0f);
	m_camera.setMovingSpeed(1.0f);
	m_camera.setRotationSpeed(0.1f);

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, false, false);
	m_cube.rewind();

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(50.0f, 100.0f, -100.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	Vector4f direction(2.0f, 2.0f, -1.0f, 0.0f);
	Vector4f::Normalize(direction);
	m_lightBuffer.createBuffer(direction.getData(), 16u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);


	float shadow_data[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	m_shadowBuffer.createBuffer(shadow_data, 16u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	float sphere_data[4] = { -0.4f, -0.75f, 0.2f, SPHERE_RADIUS };
	m_sphereBuffer.createBuffer(sphere_data, 16u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	
	m_dropBuffer.createBuffer(32u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_updateBuffer.createBuffer(16u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	std::string faces[] = { "res/textures/cubemaps/clouds/clouds_cube_px.jpg", "res/textures/cubemaps/clouds/clouds_cube_nx.jpg", "res/textures/cubemaps/clouds/clouds_cube_py.jpg", "res/textures/cubemaps/clouds/clouds_cube_ny.jpg", "res/textures/cubemaps/clouds/clouds_cube_pz.jpg", "res/textures/cubemaps/clouds/clouds_cube_nz.jpg" };
	m_wgpTextureCube.loadCubeFromFiles(faces);
	m_wgpTextureTiles.loadFromFile("res/textures/tiles.jpg", true);

	m_textureA.createEmpty(WATER_WIDTH, WATER_HEIGHT, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	m_textureB.createEmpty(WATER_WIDTH, WATER_HEIGHT, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	m_textureCaustics.createEmpty(CAUSTICS_SIZE, CAUSTICS_SIZE, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA8Unorm);

	wgpContext.addSahderModule("WATER_DROP", "res/shader/water_drop.wgsl");
	wgpContext.createRenderPipeline("WATER_DROP", "RP_WATER_DROP", VL_NONE, std::bind(&WaterSimulation::OnBindGroupLayoutsDrop, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_RGBA16Float);

	wgpContext.addSahderModule("WATER_UPDATE", "res/shader/water_update.wgsl");
	wgpContext.createRenderPipeline("WATER_UPDATE", "RP_WATER_UPDATE", VL_NONE, std::bind(&WaterSimulation::OnBindGroupLayoutsUpdate, this), 
		1u,
		WGPUPrimitiveTopology_Undefined, 
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Undefined,
		{ FRAGMENT_STATE, ColorMode::WRITE_RGBA });

	wgpContext.addSahderModule("WATER_NORMAL", "res/shader/water_normal.wgsl");
	wgpContext.createRenderPipeline("WATER_NORMAL", "RP_WATER_NORMAL", VL_NONE, std::bind(&WaterSimulation::OnBindGroupLayoutsNormal, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Undefined,
		{ FRAGMENT_STATE , ColorMode::WRITE_RGBA });

	wgpContext.addSahderModule("WATER_SPHERE", "res/shader/water_sphere.wgsl");
	wgpContext.createRenderPipeline("WATER_SPHERE", "RP_WATER_SPHERE", VL_NONE, std::bind(&WaterSimulation::OnBindGroupLayoutsSphere, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Undefined,
		{ FRAGMENT_STATE , ColorMode::WRITE_RGBA });

	wgpContext.addSahderModule("WATER_CAUSTICS", "res/shader/water_caustics.wgsl");
	wgpContext.createRenderPipeline("WATER_CAUSTICS", "RP_WATER_CAUSTICS", VL_P, std::bind(&WaterSimulation::OnBindGroupLayoutsCaustics, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_RGBA8Unorm, 
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Undefined,
		{ FRAGMENT_STATE | BLEND_STATE, ColorMode::WRITE_RGBA, DepthMode::PASS, StencilMode::DEFAULT, BlendMode::ADDITIVE_BLENDING_ONE, WGPUTextureFormat_Undefined, WGPUCullMode_None });

	wgpContext.addSahderModule("WATER_POOL", "res/shader/water_pool.wgsl");
	wgpContext.createRenderPipeline("WATER_POOL", "RP_WATER_POOL", VL_P, std::bind(&WaterSimulation::OnBindGroupLayoutsPool, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ FRAGMENT_STATE | DEPTH_STENCIL_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Back });

	wgpContext.addSahderModule("WATER_ABOVE", "res/shader/water_above.wgsl");
	wgpContext.createRenderPipeline("WATER_ABOVE", "RP_WATER_ABOVE", VL_P, std::bind(&WaterSimulation::OnBindGroupLayoutsWater, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ FRAGMENT_STATE | DEPTH_STENCIL_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Front });


	wgpContext.addSahderModule("WATER_UNDER", "res/shader/water_under.wgsl");
	wgpContext.createRenderPipeline("WATER_UNDER", "RP_WATER_UNDER", VL_P, std::bind(&WaterSimulation::OnBindGroupLayoutsWater, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ FRAGMENT_STATE | DEPTH_STENCIL_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Back });

	wgpContext.addSahderModule("SKYBOX", "res/shader/env_cube.wgsl");
	wgpContext.createRenderPipeline("SKYBOX", "RP_SKYBOX", VL_P,
		std::bind(&WaterSimulation::OnBindGroupLayoutsSkybox, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_LessEqual,
		{ DEPTH_STENCIL_STATE | FRAGMENT_STATE }
	);

	m_wgpCube.create(m_cube);
	m_wgpCube.setBindGroups("BG", std::bind(&WaterSimulation::OnBindGroupsSkybox, this));

	float uniform_data[4] = { 1.0f / WATER_WIDTH, 1.0f / WATER_HEIGHT, 0, 0 };
	wgpuQueueWriteBuffer(wgpContext.queue, m_updateBuffer.getBuffer(), 0u, uniform_data, sizeof(uniform_data));
	createBindGroupPool();
	createBindCaustic();
	{
		float positions[5 * 4 * 3];
		uint32_t indices[5 * 6];

		int v_idx = 0;
		int i_idx = 0;

		const int cube_faces[5][4] = {
		  {0, 4, 2, 6},
		  {1, 3, 5, 7},
		  {2, 6, 3, 7},
		  {0, 2, 1, 3},
		  {4, 5, 6, 7},
		};

		for (int face = 0; face < 5; face++) {
			int vertex_offset = v_idx / 3;

			for (int j = 0; j < 4; j++) {
				int octant = cube_faces[face][j];
				float x = (octant & 1) * 2.0f - 1.0f;
				float y = ((octant & 2) >> 1) * 2.0f - 1.0f;
				float z = ((octant & 4) >> 2) * 2.0f - 1.0f;
				positions[v_idx++] = x;
				positions[v_idx++] = y;
				positions[v_idx++] = z;
			}

			indices[i_idx++] = vertex_offset + 0;
			indices[i_idx++] = vertex_offset + 1;
			indices[i_idx++] = vertex_offset + 2;
			indices[i_idx++] = vertex_offset + 2;
			indices[i_idx++] = vertex_offset + 1;
			indices[i_idx++] = vertex_offset + 3;
		}

		m_poolVertex.createBuffer(reinterpret_cast<const void*>(positions), sizeof(positions), WGPUBufferUsage_Vertex | WGPUBufferUsage_CopySrc);
		m_poolIndex.createBuffer(reinterpret_cast<const void*>(indices), sizeof(indices), WGPUBufferUsage_Index | WGPUBufferUsage_CopySrc);
	}


	{
		const int detail = WATER_SURFACE_DETAIL;
		const int vertex_count = (detail + 1) * (detail + 1) * 3;
		const int index_count = detail * detail * 6;

		std::vector<float> positions(vertex_count);
		std::vector<uint32_t> indices(index_count);

		int pos_idx = 0;
		for (int z = 0; z <= detail; z++) {
			float t = (float)z / detail;
			for (int x = 0; x <= detail; x++) {
				float s = (float)x / detail;
				positions[pos_idx++] = 2.0f * s - 1.0f;
				positions[pos_idx++] = 2.0f * t - 1.0f;
				positions[pos_idx++] = 0.0f;
			}
		}

		int idx_idx = 0;
		for (int z = 0; z < detail; z++) {
			for (int x = 0; x < detail; x++) {
				int i = x + z * (detail + 1);
				indices[idx_idx++] = i;
				indices[idx_idx++] = i + 1;
				indices[idx_idx++] = i + detail + 1;
				indices[idx_idx++] = i + detail + 1;
				indices[idx_idx++] = i + 1;
				indices[idx_idx++] = i + detail + 2;
			}
		}

		m_waterVertex.createBuffer(positions.data(), positions.size() * sizeof(float), WGPUBufferUsage_Vertex | WGPUBufferUsage_CopySrc);
		m_waterIndex.createBuffer(indices.data(), indices.size() * sizeof(uint32_t), WGPUBufferUsage_Index | WGPUBufferUsage_CopySrc);
	}

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	wgpContext.OnDraw = std::bind(&WaterSimulation::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

WaterSimulation::~WaterSimulation() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
}

void WaterSimulation::fixedUpdate() {

}

void WaterSimulation::update() {
	Keyboard& keyboard = Keyboard::instance();
	Mouse& mouse = Mouse::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	bool playerMove = false;

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

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(1.0f, 0.0f, 0.0f);
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	
}

void WaterSimulation::render() {
	wgpDraw();
}

void WaterSimulation::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	{
		run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_UPDATE"), m_updateBuffer.getBuffer(), 16u);
		run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_UPDATE"), m_updateBuffer.getBuffer(), 16u);
		run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_NORMAL"), m_updateBuffer.getBuffer(), 16u);
	}

	{

		WgpRenderer::DrawColor(m_textureCaustics, [this](const WGPURenderPassEncoder& renderPassEncoder) {
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_CAUSTICS"));
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_useA ? m_bindGroupCausticA : m_bindGroupCausticB, 0u, nullptr);
			wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_waterVertex.getBuffer(), 0u, wgpuBufferGetSize(m_waterVertex.getBuffer()));
			wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_waterIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_waterIndex.getBuffer()));
			wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 240000u, 1u, 0u, 0u, 0u);
		});
	}

	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		WGPUBindGroup water_bind_group = m_useA ? m_bindGoupPoolA : m_bindGoupPoolB;

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_POOL"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGoupPoolA, 0u, NULL);
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_poolVertex.getBuffer(), 0u, wgpuBufferGetSize(m_poolVertex.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_poolIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_poolIndex.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 30u, 1u, 0u, 0u, 0u);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}


	/*WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_SKYBOX"));
	m_wgpCube.draw(renderPassEncoder);

	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);*/
}

void WaterSimulation::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void WaterSimulation::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void WaterSimulation::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void WaterSimulation::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void WaterSimulation::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD

	if (event.keyCode == VK_LMENU) {		
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void WaterSimulation::OnKeyUp(const Event::KeyboardEvent& event) {

}

void WaterSimulation::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

void WaterSimulation::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
	ImGui_ImplWGPU_NewFrame();
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

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	
	ImGui::End();	
	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsDrop() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[0].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = 32u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsUpdate() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[0].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = 16u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsNormal() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[0].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = 16u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsSphere() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[0].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = 32u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsCaustics() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 16u;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = 16u;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[3].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[4].buffer.minBindingSize = 16u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsPool() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(9);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[3].buffer.minBindingSize = 16u;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[4].buffer.minBindingSize = 16u;

	bindingLayoutEntries[5].binding = 5u;
	bindingLayoutEntries[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[5].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[6].binding = 6u;
	bindingLayoutEntries[6].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[6].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[6].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[7].binding = 7u;
	bindingLayoutEntries[7].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[7].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[7].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[8].binding = 8u;
	bindingLayoutEntries[8].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[8].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[8].buffer.minBindingSize = 16u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsWater() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(11);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = 16u;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = 16u;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[4].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[5].binding = 5u;
	bindingLayoutEntries[5].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[5].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[6].binding = 6u;
	bindingLayoutEntries[6].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[6].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[6].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[7].binding = 7u;
	bindingLayoutEntries[7].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[7].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[7].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[8].binding = 8u;
	bindingLayoutEntries[8].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[8].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[8].buffer.minBindingSize = 16u;

	bindingLayoutEntries[9].binding = 9u;
	bindingLayoutEntries[9].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[9].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[10].binding = 10u;
	bindingLayoutEntries[10].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[10].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[10].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_Cube;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsSkybox() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> WaterSimulation::OnBindGroupsSkybox() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpTextureCube.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_SKYBOX"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void WaterSimulation::createBindGroupPool() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(9u);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpTextureTiles.getTextureView();

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[3].offset = 0u;
	bindGroupEntries[3].size = 16u;

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].buffer = m_sphereBuffer.getBuffer();
	bindGroupEntries[4].offset = 0u;
	bindGroupEntries[4].size = 16u;

	bindGroupEntries[5].binding = 5u;
	bindGroupEntries[5].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[6].binding = 6u;
	bindGroupEntries[6].textureView = m_textureA.getTextureView();

	bindGroupEntries[7].binding = 7u;
	bindGroupEntries[7].textureView = m_textureCaustics.getTextureView();

	bindGroupEntries[8].binding = 8u;
	bindGroupEntries[8].buffer = m_shadowBuffer.getBuffer();
	bindGroupEntries[8].offset = 0u;
	bindGroupEntries[8].size = 16u;

	WGPUBindGroupDescriptor bindGroupDescA = {};
	bindGroupDescA.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_POOL"), 0u);
	bindGroupDescA.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescA.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGoupPoolA = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescA);

	bindGroupEntries[6].textureView = m_textureB.getTextureView();

	WGPUBindGroupDescriptor bindGroupDescB = {};
	bindGroupDescB.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_POOL"), 0u);
	bindGroupDescB.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescB.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGoupPoolB = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescB);
}

void WaterSimulation::createBindCaustic() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(5u);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = 16u;

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_sphereBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = 16u;

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_textureA.getTextureView();

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].buffer = m_shadowBuffer.getBuffer();
	bindGroupEntries[4].offset = 0u;
	bindGroupEntries[4].size = 16u;

	WGPUBindGroupDescriptor bindGroupDescA = {};
	bindGroupDescA.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_CAUSTICS"), 0u);
	bindGroupDescA.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescA.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGroupCausticA = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescA);

	bindGroupEntries[2].textureView = m_textureB.getTextureView();

	WGPUBindGroupDescriptor bindGroupDescB = {};
	bindGroupDescB.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_CAUSTICS"), 0u);
	bindGroupDescB.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescB.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGroupCausticB = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescB);

}

void WaterSimulation::run_simulation_pass(WGPURenderPipeline pipeline, WGPUBuffer uniform_buffer, uint32_t uniform_size) {
	WGPUTextureView input_view = m_useA ? m_textureA.getTextureView() : m_textureB.getTextureView();
	//WGPUTextureView output_view = m_useA ? m_textureB.getTextureView() : m_textureA.getTextureView();

	std::vector<WGPUBindGroupEntry> bindGroupEntries(3u);
	
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].textureView = input_view;

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);



	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = uniform_buffer;
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = uniform_size;

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(pipeline, 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	WgpRenderer::DrawColor(m_useA ? m_textureB : m_textureA, [this, &pipeline, &bindGroup](const WGPURenderPassEncoder& renderPassEncoder) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, pipeline);
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, bindGroup, 0u, nullptr);
		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);
	});



	wgpuBindGroupRelease(bindGroup);

	m_useA = !m_useA;
}