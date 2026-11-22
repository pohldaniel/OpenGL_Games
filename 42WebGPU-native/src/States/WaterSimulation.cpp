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
	m_camera.setMovingSpeed(10.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

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
	m_sphereMoveBuffer.createBuffer(32u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	std::string faces[] = { "res/textures/cubemaps/clouds/clouds_cube_px.jpg", "res/textures/cubemaps/clouds/clouds_cube_nx.jpg", "res/textures/cubemaps/clouds/clouds_cube_py.jpg", "res/textures/cubemaps/clouds/clouds_cube_ny.jpg", "res/textures/cubemaps/clouds/clouds_cube_pz.jpg", "res/textures/cubemaps/clouds/clouds_cube_nz.jpg" };
	m_wgpTextureCube.loadCubeFromFiles(faces);
	m_wgpTextureTiles.loadFromFile("res/textures/tiles.jpg", true);

	m_textureA.createEmpty(WATER_WIDTH, WATER_HEIGHT, 1u, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	m_textureB.createEmpty(WATER_WIDTH, WATER_HEIGHT, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	m_textureCaustics.createEmpty(CAUSTICS_SIZE, CAUSTICS_SIZE, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA8Unorm);

	wgpContext.addSahderModule("WATER_DROP", "res/shader/water_drop.wgsl");
	wgpContext.createRenderPipeline("WATER_DROP", "RP_WATER_DROP", VL_NONE, std::bind(&WaterSimulation::OnBindGroupLayoutsDrop, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Undefined,
		{ FRAGMENT_STATE, ColorMode::WRITE_RGBA });

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

	wgpContext.addSahderModule("WATER_SPHERE_MOVE", "res/shader/water_sphere_move.wgsl");
	wgpContext.createRenderPipeline("WATER_SPHERE_MOVE", "RP_WATER_SPHERE_MOVE", VL_NONE, std::bind(&WaterSimulation::OnBindGroupLayoutsSphereMove, this),
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

	wgpContext.addSahderModule("WATER_SPHERE", "res/shader/water_sphere.wgsl");
	wgpContext.createRenderPipeline("WATER_SPHERE", "RP_WATER_SPHERE", VL_P, std::bind(&WaterSimulation::OnBindGroupLayoutsSphere, this),
		1u,
		WGPUPrimitiveTopology_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ FRAGMENT_STATE | DEPTH_STENCIL_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Back });


	float uniform_data[4] = { 1.0f / WATER_WIDTH, 1.0f / WATER_HEIGHT, 0, 0 };
	wgpuQueueWriteBuffer(wgpContext.queue, m_updateBuffer.getBuffer(), 0u, uniform_data, sizeof(uniform_data));
	createBindGroupPool();
	createBindGroupCaustic();
	createBindGroupWater();
	createBindGroupSphere();
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

	{
		const int detail = SPHERE_DETAIL;
		const int max_vertices = 8 * (detail + 1) * (detail + 2) / 2;
		const int max_indices = 8 * detail * detail * 3;

		std::vector<float> positions(max_vertices * 3);
		std::vector<uint32_t> indices(max_indices);
		std::vector<int> unique_map(max_vertices);

		int unique_count = 0;
		int final_idx_count = 0;

		#define ADD_VERTEX(px, py, pz)                                                 \
		  do {                                                                         \
			int found = -1;                                                            \
			for (int k = 0; k < unique_count && found < 0; k++) {                      \
			  if (fabsf(positions[k * 3] - (px)) < 1e-6f                               \
				  && fabsf(positions[k * 3 + 1] - (py)) < 1e-6f                        \
				  && fabsf(positions[k * 3 + 2] - (pz)) < 1e-6f) {                     \
				found = k;                                                             \
			  }                                                                        \
			}                                                                          \
			if (found < 0) {                                                           \
			  found                           = unique_count;                          \
			  positions[unique_count * 3]     = (px);                                  \
			  positions[unique_count * 3 + 1] = (py);                                  \
			  positions[unique_count * 3 + 2] = (pz);                                  \
			  unique_count++;                                                          \
			}                                                                          \
			unique_map[vertex_idx++] = found;                                          \
		  } while (0)

		for (int octant = 0; octant < 8; octant++) {
			float sx = (octant & 1) ? 1.0f : -1.0f;
			float sy = (octant & 2) ? 1.0f : -1.0f;
			float sz = (octant & 4) ? 1.0f : -1.0f;
			int flip = (sx * sy * sz > 0.0f) ? 1 : 0;

			int vertex_idx = 0;
			int local_indices[256];
			int local_count = 0;

			for (int i = 0; i <= detail; i++) {
				for (int j = 0; i + j <= detail; j++) {
					float a = (float)i / detail;
					float b = (float)j / detail;
					float c = (float)(detail - i - j) / detail;

					a = a + (a - a * a) / 2.0f;
					b = b + (b - b * b) / 2.0f;
					c = c + (c - c * c) / 2.0f;

					float len = sqrtf(a * a + b * b + c * c);
					float px = (a / len) * sx;
					float py = (b / len) * sy;
					float pz = (c / len) * sz;

					ADD_VERTEX(px, py, pz);
					local_indices[local_count++] = unique_map[vertex_idx - 1];
				}
			}

			int k = 0;
			for (int i = 0; i < detail; i++) {
				for (int j = 0; i + j < detail; j++) {
					int a_idx = local_indices[k];
					int b_idx = local_indices[k + detail - i + 1];
					int c_idx = local_indices[k + 1];

					if (flip) {
						indices[final_idx_count++] = a_idx;
						indices[final_idx_count++] = b_idx;
						indices[final_idx_count++] = c_idx;
					}
					else {
						indices[final_idx_count++] = a_idx;
						indices[final_idx_count++] = c_idx;
						indices[final_idx_count++] = b_idx;
					}

					if (i + j < detail - 1) {
						int d_idx = local_indices[k + detail - i + 2];
						if (flip) {
							indices[final_idx_count++] = b_idx;
							indices[final_idx_count++] = d_idx;
							indices[final_idx_count++] = c_idx;
						}
						else {
							indices[final_idx_count++] = b_idx;
							indices[final_idx_count++] = c_idx;
							indices[final_idx_count++] = d_idx;
						}
					}
					k++;
				}
				k++;
			}
		}
		m_sphereVertex.createBuffer(positions.data(), positions.size() * sizeof(float), WGPUBufferUsage_Vertex | WGPUBufferUsage_CopySrc);
		m_sphereIndex.createBuffer(indices.data(), indices.size() * sizeof(uint32_t), WGPUBufferUsage_Index | WGPUBufferUsage_CopySrc);
		#undef ADD_VERTEX
	}

	std::vector<float> heightField(WATER_WIDTH* WATER_HEIGHT, 0.0f);
	for (int i = 0; i < 20; i++) {
		float centerX = random_float();
		float centerY = random_float();
		float radius = 0.03f;
		float strength = (i & 1) ? 0.01f : -0.01f;

		for (uint32_t y = 0; y < WATER_HEIGHT; y++) {
			float uvY = (static_cast<float>(y) + 0.5f) / WATER_HEIGHT;
			for (uint32_t x = 0; x < WATER_WIDTH; x++) {
				float uvX = (static_cast<float>(x) + 0.5f) / WATER_WIDTH;

				float dx = uvX - centerX;
				float dy = uvY - centerY;
				float dist = std::sqrt(dx * dx + dy * dy);

				if (dist < radius) {
					float drop = 1.0f - dist / radius;
					drop = 0.5f - std::cos(drop * 3.14159265f) * 0.5f;
					heightField[y * WATER_WIDTH + x] += drop * strength;
				}
			}
		}
	}

	std::vector<uint16_t> halfPixels(WATER_WIDTH * WATER_HEIGHT * 4, 0);
	for (size_t i = 0; i < heightField.size(); i++) {
		halfPixels[i * 4 + 0] = floatToHalf(heightField[i]);
	}
	m_textureA.uploadData(halfPixels.data(), 8u);

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
	m_trackball.idle();

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(0.0f, 1.0f, 0.0f);
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));
}

void WaterSimulation::render() {
	wgpDraw();
}

void WaterSimulation::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	if (m_pendingDrop.pending) {
		m_pendingDrop.pending = false;
		water_add_drop(m_pendingDrop.x, m_pendingDrop.z, m_pendingDrop.radius, m_pendingDrop.strength);
	}

	{
		run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_UPDATE"), m_updateBuffer.getBuffer(), 16u);
		run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_UPDATE"), m_updateBuffer.getBuffer(), 16u);
		run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_NORMAL"), m_updateBuffer.getBuffer(), 16u);
	}

	WgpRenderer::DrawColor(m_textureCaustics, [this](const WGPURenderPassEncoder& renderPassEncoder) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_CAUSTICS"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_useA ? m_bindGroupCausticA : m_bindGroupCausticB, 0u, nullptr);
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_waterVertex.getBuffer(), 0u, wgpuBufferGetSize(m_waterVertex.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_waterIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_waterIndex.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 240000u, 1u, 0u, 0u, 0u);
	});
	
	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_POOL"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_useA ? m_bindGoupPoolA : m_bindGoupPoolB, 0u, nullptr);
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_poolVertex.getBuffer(), 0u, wgpuBufferGetSize(m_poolVertex.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_poolIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_poolIndex.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 30u, 1u, 0u, 0u, 0u);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_SPHERE"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_useA ? m_bindGroupSphereA : m_bindGroupSphereB, 0u, nullptr);
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_sphereVertex.getBuffer(), 0u, wgpuBufferGetSize(m_sphereVertex.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_sphereIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_sphereIndex.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 2400u, 1u, 0u, 0u, 0u);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_ABOVE"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_useA ? m_bindGroupWaterA : m_bindGroupWaterB, 0u, nullptr);
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_waterVertex.getBuffer(), 0u, wgpuBufferGetSize(m_waterVertex.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_waterIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_waterIndex.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 240000u, 1u, 0u, 0u, 0u);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WATER_UNDER"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_useA ? m_bindGroupWaterA : m_bindGroupWaterB, 0u, nullptr);
		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_waterVertex.getBuffer(), 0u, wgpuBufferGetSize(m_waterVertex.getBuffer()));
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_waterIndex.getBuffer(), WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_waterIndex.getBuffer()));
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, 240000u, 1u, 0u, 0u, 0u);


		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}

	if (m_drawUi)
	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		renderUi(renderPassEncoder);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void WaterSimulation::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	if (event.button == Event::MouseMoveEvent::MouseButton::BUTTON_LEFT) {
		
		Vector3f coords;
		if (getWorldPosition(event.x, event.y, Vector3f(0.0f, 1.0f, 0.0f), coords)) {
			m_pendingDrop.x = coords[0];
			m_pendingDrop.z = coords[2];
			m_pendingDrop.radius = 0.03f;
			m_pendingDrop.strength = 0.01f;
			m_pendingDrop.pending = true;
		}
	}
}

void WaterSimulation::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {

		Vector3f ray = getRay(event.x, event.y);
		float t;
		Vector3f hit;
		if (hit_test_sphere(m_camera.getPosition(), ray, Vector3f(-0.4f, -0.75f, 0.2f), SPHERE_RADIUS, &t, hit)) {
			std::cout << "-------" << std::endl;
			return;
		}

		Mouse::instance().attach(Application::GetWindow(), false, true);
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);

		Vector3f coords;
		if (getWorldPosition(event.x, event.y, Vector3f(0.0f, 1.0f, 0.0f), coords)) {
			m_pendingDrop.x = coords[0];
			m_pendingDrop.z = coords[2];
			m_pendingDrop.radius = 0.03f;
			m_pendingDrop.strength = 0.01f;
			m_pendingDrop.pending = true;
		}
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void WaterSimulation::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
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

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsSphereMove() {
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

std::vector<WGPUBindGroupLayout> WaterSimulation::OnBindGroupLayoutsSphere() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(6);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = 16u;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
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
	bindingLayoutEntries[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[5].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[5].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
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

void WaterSimulation::createBindGroupCaustic() {
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

void WaterSimulation::createBindGroupWater() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(11u);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = 16u;

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_sphereBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = 16u;

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_wgpTextureTiles.getTextureView();

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

	bindGroupEntries[9].binding = 9u;
	bindGroupEntries[9].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[10].binding = 10u;
	bindGroupEntries[10].textureView = m_wgpTextureCube.getTextureView();

	WGPUBindGroupDescriptor bindGroupDescA = {};
	bindGroupDescA.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_ABOVE"), 0u);
	bindGroupDescA.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescA.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGroupWaterA = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescA);

	bindGroupEntries[6].textureView = m_textureB.getTextureView();

	WGPUBindGroupDescriptor bindGroupDescB = {};
	bindGroupDescB.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_ABOVE"), 0u);
	bindGroupDescB.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescB.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGroupWaterB = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescB);
}

void WaterSimulation::createBindGroupSphere() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(6u);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = 16u;

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_sphereBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = 16u;

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].textureView = m_textureA.getTextureView();

	bindGroupEntries[5].binding = 5u;
	bindGroupEntries[5].textureView = m_textureCaustics.getTextureView();

	WGPUBindGroupDescriptor bindGroupDescA = {};
	bindGroupDescA.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_SPHERE"), 0u);
	bindGroupDescA.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescA.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGroupSphereA = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescA);

	bindGroupEntries[4].textureView = m_textureB.getTextureView();

	WGPUBindGroupDescriptor bindGroupDescB = {};
	bindGroupDescB.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WATER_SPHERE"), 0u);
	bindGroupDescB.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDescB.entries = (WGPUBindGroupEntry*)bindGroupEntries.data();

	m_bindGroupSphereB = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescB);
}

void WaterSimulation::run_simulation_pass(WGPURenderPipeline pipeline, WGPUBuffer uniform_buffer, uint32_t uniform_size) {
	WGPUTextureView input_view = m_useA ? m_textureA.getTextureView() : m_textureB.getTextureView();
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

void WaterSimulation::water_add_drop(float x, float y, float radius, float strength) {
	float uniform_data[8] = { x, y, radius, strength, 0.0f, 0.0f, 0.0f, 0.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, m_dropBuffer.getBuffer(), 0u, uniform_data, sizeof(uniform_data));
	run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_DROP"), m_dropBuffer.getBuffer(), 32u);
}

void WaterSimulation::water_move_sphere(const Vector3f& oldCenter, const Vector3f& newCenter, float radius) {
	float uniform_data[8] = { oldCenter[0], oldCenter[1], oldCenter[2], radius,
						      newCenter[0], newCenter[1], newCenter[2], 0.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, m_sphereMoveBuffer.getBuffer(), 0u, uniform_data, sizeof(uniform_data));
	run_simulation_pass(wgpContext.renderPipelines.at("RP_WATER_SPHERE_MOVE"), m_dropBuffer.getBuffer(), 32u);
}

bool WaterSimulation::hit_test_sphere(const Vector3f& origin, const Vector3f& ray, const Vector3f& center, float radius, float* t_out, Vector3f& hit_out){
	Vector3f offset = origin - center;

	float a = Vector3f::Dot(ray, ray);
	float b = 2.0f * Vector3f::Dot(offset, ray);
	float c = Vector3f::Dot(offset, offset) - radius * radius;
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant > 0.0f) {
		float t = (-b - sqrtf(discriminant)) / (2.0f * a);
		if (t > 0.0f) {
			*t_out = t;
			hit_out[0] = origin[0] + ray[0] * t;
			hit_out[1] = origin[1] + ray[1] * t;
			hit_out[2] = origin[2] + ray[2] * t;
			return true;
		}
	}
	return false;
}

float WaterSimulation::random_float_min_max(float min, float max) {
	return ((max - min) * ((float)rand() / (float)RAND_MAX)) + min;
}

float WaterSimulation::random_float() {
	return random_float_min_max(0.0f, 1.0f);
}

uint16_t WaterSimulation::floatToHalf(float value) {
	uint32_t f;
	std::memcpy(&f, &value, sizeof(float));

	uint32_t sign = (f >> 16) & 0x8000;
	int32_t exp = ((f >> 23) & 0xFF) - 127 + 15;
	uint32_t mant = f & 0x007FFFFF;

	if (exp <= 0) {
		if (exp < -10) return static_cast<uint16_t>(sign);
		mant = (mant | 0x00800000) >> (1 - exp);
		return static_cast<uint16_t>(sign | (mant >> 13));
	}else if (exp == 0xFF - 127 + 15) {
		if (mant == 0) return static_cast<uint16_t>(sign | 0x7C00);
		return static_cast<uint16_t>(sign | 0x7C00 | (mant >> 13) | 1);
	}else if (exp > 30) {
		return static_cast<uint16_t>(sign | 0x7C00);
	}

	return static_cast<uint16_t>(sign | (exp << 10) | (mant >> 13));
}

bool WaterSimulation::getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection) {
	float mouseXndc = (2.0f * xPos) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * yPos) / static_cast<float>(Application::Height);

	float tanfov = m_camera.getTanFov();
	float aspect = (static_cast<float>(Application::Width) / static_cast<float>(Application::Height));

	Vector3f rayStartWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getNear();
	Vector3f rayEndWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getFar();
	Vector3f direction = Vector3f::Normalize(rayEndWorld - rayStartWorld);

	float denom = Vector3f::Dot(direction, planeNormal);
	if (std::abs(denom) > 1e-6f) {
		float t = Vector3f::Dot(-rayStartWorld, planeNormal) / denom;
		if (t >= 0.0f) {
			outIntersection = rayStartWorld + direction * t;
			return true;
		}
	}
	return false;
}

Vector3f WaterSimulation::getRay(int xPos, int yPos) {
	float mouseXndc = (2.0f * xPos) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * yPos) / static_cast<float>(Application::Height);

	float tanfov = m_camera.getTanFov();
	float aspect = (static_cast<float>(Application::Width) / static_cast<float>(Application::Height));

	Vector3f rayStartWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getNear();
	Vector3f rayEndWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getFar();
	return Vector3f::Normalize(rayEndWorld - rayStartWorld);
}