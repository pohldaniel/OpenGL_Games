#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <engine/input/GamePad.h>
#include <States/Menu.h>

#include "Globals.h"
#include "Application.h"
#include "WireframeOverlay.h"

WireframeOverlay::WireframeOverlay(StateMachine& machine) : State(machine, States::WIREFRAME_OVERLAY) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f , -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-300.0f, 0.0f, 300.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(200.0f);
	m_camera.setRotationSpeed(0.1f);
	m_trackball.reshape(Application::Width, Application::Height);

	m_teapot.loadModel("res/models/teapot.obj", false, false, true);
	m_sphere.buildSphere({ 0.0f, 0.0f, 0.0f }, 20.0f, 0.0f, 32u, 16u, false, true);
	m_jem.buildSphere({ 0.0f, 0.0f, 0.0f }, 20.0f, 0.0f, 5u, 3u, false, true);
	m_jem.flatShading();

	m_rock.buildSphere({ 0.0f, 0.0f, 0.0f }, 20.0f, 0.05f, 32u, 16u, false, true);
	/*for (uint64_t i = 0; i < m_rock.getVertexBuffer().size() / m_rock.getStride(); i++) {
		float* pos = &m_rock.vertexBuffer()[i * 6u];
		float offset = 0.1f * 20.0f * (randFloat() - 0.5f);
		float len = 20.0f;
		pos[0] += (pos[0] / len) * offset;
		pos[1] += (pos[1] / len) * offset;
		pos[2] += (pos[2] / len) * offset;		
	}*/
	m_rock.flatShading();

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

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	wgpContext.addSahderModule("WIREFRAME_BASE", "res/shader/wireframe_base.wgsl");
	wgpContext.createRenderPipeline("WIREFRAME_BASE", "RP_WIREFRAME_BASE", VL_PN, std::bind(&WireframeOverlay::OnBindGroupLayouts, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Undefined, {}, {m_depthbias, m_depthBiasSlopeScale, 0.0f} }
	);

	wgpContext.addSahderModule("WIREFRAME_LINE", "res/shader/wireframe_line.wgsl");
	wgpContext.createRenderPipeline("WIREFRAME_LINE", "RP_WIREFRAME_LINE", VL_NONE, std::bind(&WireframeOverlay::OnBindGroupLayoutsWireframe, this), 
		1u, 
		WGPUPrimitiveTopology_LineList
	);

	wgpContext.addSahderModule("WIREFRAME_BARYCENTRIC", "res/shader/wireframe_barycentric.wgsl");
	wgpContext.createRenderPipeline("WIREFRAME_BARYCENTRIC", "RP_WIREFRAME_BARYCENTRIC", VL_NONE, std::bind(&WireframeOverlay::OnBindGroupLayoutsWireframe, this));

	m_wgpModels.resize(4u);
	m_wgpModels[0].create(m_teapot);
	m_wgpModels[1].create(m_sphere);
	m_wgpModels[2].create(m_jem);
	m_wgpModels[3].create(m_rock);

	wgpContext.setClearColor({ 0.3f, 0.3f, 0.3f, 1.0f });
	wgpContext.OnDraw = std::bind(&WireframeOverlay::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	for (uint32_t i = 0; i < NUM_OBJECTS; ++i) {
		Vector4f color = { randFloat(), randFloat(), randFloat(), 1.0f};

		Object object;
		object.geometryIndex = rand() % 4;
		object.uniformBuffer.createBuffer(sizeof(Matrix4f) + sizeof(Vector4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

		wgpuQueueWriteBuffer(wgpContext.queue, object.uniformBuffer.getBuffer(), 0u, &Matrix4f::IDENTITY,  sizeof(Matrix4f));
		wgpuQueueWriteBuffer(wgpContext.queue, object.uniformBuffer.getBuffer(), sizeof(Matrix4f), &color, sizeof(Vector4f));

		std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
		bindGroupEntries[0].binding = 0u;
		bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
		bindGroupEntries[0].offset = 0u;
		bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

		bindGroupEntries[1].binding = 1u;
		bindGroupEntries[1].buffer = object.uniformBuffer.getBuffer();
		bindGroupEntries[1].offset = 0u;
		bindGroupEntries[1].size = wgpuBufferGetSize(object.uniformBuffer.getBuffer());

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WIREFRAME_BASE"), 0u);
		bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
		bindGroupDesc.entries = bindGroupEntries.data();
		object.bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
		
		m_objects.push_back(object);
	}

	for (uint32_t i = 0; i < NUM_OBJECTS; ++i) {
		Object& object = m_objects[i];

		LineUniforms lineUniforms;
		lineUniforms.stride = 6u;
		lineUniforms.thickness = m_thickness;
		lineUniforms.alpha_threshold = m_alphaTthreshold;

		object.uniformBufferLine.createBuffer(&lineUniforms, sizeof(LineUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

		std::vector<WGPUBindGroupEntry> bindGroupEntries(5);
		bindGroupEntries[0].binding = 0u;
		bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
		bindGroupEntries[0].offset = 0u;
		bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

		bindGroupEntries[1].binding = 1u;
		bindGroupEntries[1].buffer = object.uniformBuffer.getBuffer();
		bindGroupEntries[1].offset = 0u;
		bindGroupEntries[1].size = wgpuBufferGetSize(object.uniformBuffer.getBuffer());

		bindGroupEntries[2].binding = 2u;
		bindGroupEntries[2].buffer = object.uniformBufferLine.getBuffer();
		bindGroupEntries[2].offset = 0u;
		bindGroupEntries[2].size = wgpuBufferGetSize(object.uniformBufferLine.getBuffer());

		bindGroupEntries[3].binding = 3u;
		bindGroupEntries[3].buffer = m_wgpModels[object.geometryIndex].getMesh().getVertexBuffer().getBuffer();
		bindGroupEntries[3].offset = 0u;
		bindGroupEntries[3].size = wgpuBufferGetSize(m_wgpModels[object.geometryIndex].getMesh().getVertexBuffer().getBuffer());

		bindGroupEntries[4].binding = 4u;
		bindGroupEntries[4].buffer = m_wgpModels[object.geometryIndex].getMesh().getIndexBuffer().getBuffer();
		bindGroupEntries[4].offset = 0u;
		bindGroupEntries[4].size = wgpuBufferGetSize(m_wgpModels[object.geometryIndex].getMesh().getIndexBuffer().getBuffer());

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WIREFRAME_LINE"), 0u);
		bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
		bindGroupDesc.entries = bindGroupEntries.data();
		object.bindGroupWireframe = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

		m_objects.push_back(object);
	}
}

WireframeOverlay::~WireframeOverlay() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();

	for (uint32_t i = 0; i < NUM_OBJECTS; ++i) {
		const Object& object = m_objects[i];
		object.uniformBuffer.markForDelete();
		object.uniformBufferLine.markForDelete();
		if (object.bindGroup) {
			wgpuBindGroupRelease(object.bindGroup);
		}

		if (object.bindGroupWireframe) {
			wgpuBindGroupRelease(object.bindGroupWireframe);
		}
	}
}

void WireframeOverlay::fixedUpdate() {

}

void WireframeOverlay::update() {
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
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	if (m_animate) {
		m_animationTime += m_dt;
		for (uint32_t i = 0; i < NUM_OBJECTS; ++i) {

			Transform transform;
			transform.rotate((m_animationTime * 0.53f + (float)i) * _180_ON_PI, 0.0f, 0.0f, false);
			transform.translate(0.0f, 0.0f, sinf((float)i * 9.721f + m_animationTime * 0.1f) * 200.0f);
			transform.rotate(0.0f, (float)i * 2.967f * _180_ON_PI, 0.0f, false);
			transform.rotate((float)i * 4.567f * _180_ON_PI, 0.0f, 0.0f, false);
			transform.translate(0.0f, 0.0f, sinf((float)i * 3.721f + m_animationTime * 0.1f) * 200.0f);

			//Matrix4f model = Matrix4f::Translate(0.0f, 0.0f, sinf((float)i * 3.721f + m_animationTime * 0.1f) * 200.0f) *
			//                 Matrix4f::Rotate((float)i * 4.567f * _180_ON_PI, 0.0f, 0.0) *
			//                 Matrix4f::Rotate(0.0f, (float)i * 2.967f * _180_ON_PI, 0.0f) *
			//                 Matrix4f::Translate(0.0f, 0.0f, sinf((float)i * 9.721f + m_animationTime * 0.1f) * 200.0f) *
			//                 Matrix4f::Rotate((m_animationTime * 0.53f + (float)i) * _180_ON_PI, 0.0f, 0.0f);

			const Object& object = m_objects[i];
			wgpuQueueWriteBuffer(wgpContext.queue, object.uniformBuffer.getBuffer(), 0u, &transform.getTransformationMatrix(), sizeof(Matrix4f));
		}
	}
}

void WireframeOverlay::render() {
	wgpDraw();
}

void WireframeOverlay::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	if (m_darawModels) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WIREFRAME_BASE"));
		for (uint32_t i = 0; i < NUM_OBJECTS; i++) {
			const WgpModel& model = m_wgpModels[m_objects[i].geometryIndex];

			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, m_objects[i].bindGroup, 0, nullptr);
			model.draw(renderPassEncoder);
		}
	}

	if(m_drawWireframe && !m_barycentric){
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WIREFRAME_LINE"));
		for (uint32_t i = 0; i < NUM_OBJECTS; i++) {
			const WgpModel& model = m_wgpModels[m_objects[i].geometryIndex];
			const WgpMesh& mesh = model.getMeshes().front();

			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, m_objects[i].bindGroupWireframe, 0, nullptr);
			wgpuRenderPassEncoderDraw(renderPassEncoder, mesh.getDrawCount() * 2u, 1u, 0u, 0u);
		}
	}

	if (m_drawWireframe && m_barycentric) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WIREFRAME_BARYCENTRIC"));
		for (uint32_t i = 0; i < NUM_OBJECTS; i++) {
			const WgpModel& model = m_wgpModels[m_objects[i].geometryIndex];
			const WgpMesh& mesh = model.getMeshes().front();

			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, m_objects[i].bindGroupWireframe, 0, nullptr);
			wgpuRenderPassEncoderDraw(renderPassEncoder, mesh.getDrawCount(), 1u, 0u, 0u);
		}
	}

	if(m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);	
}

void WireframeOverlay::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void WireframeOverlay::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void WireframeOverlay::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void WireframeOverlay::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void WireframeOverlay::OnKeyDown(const Event::KeyboardEvent& event) {
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

void WireframeOverlay::OnKeyUp(const Event::KeyboardEvent& event) {

}

void WireframeOverlay::resize(int deltaW, int deltaH) {
	m_camera.perspective(60.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

void WireframeOverlay::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	ImGui::Checkbox("Model", &m_darawModels);
	ImGui::Checkbox("Lines", &m_drawWireframe);
	ImGui::Checkbox("Barycentric", &m_barycentric);
	if (ImGui::SliderInt("Depth Bias", &m_depthbias, -3, 3)) {
		rebuildPipeline();
	}

	if (ImGui::SliderFloat("Depth Bias Slope Scale", &m_depthBiasSlopeScale, -1.0f, 1.0f, "%.2f")) {
		rebuildPipeline();
	}
	ImGui::Checkbox("Animate", &m_animate);

	if (m_barycentric) {
		if (ImGui::SliderFloat("Thickness", &m_thickness, 0.0f, 10.0f, "%.1f")) {
			updateUniforms();
		}

		if (ImGui::SliderFloat("Alpha Threshold", &m_alphaTthreshold, 0.0f, 1.0f, "%.2f")) {
			updateUniforms();;
		}
	}

	ImGui::End();	
	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> WireframeOverlay::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Matrix4f) + sizeof(Vector4f);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> WireframeOverlay::OnBindGroupLayoutsWireframe() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Matrix4f) + sizeof(Vector4f);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = sizeof(LineUniforms);

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

float WireframeOverlay::randFloat() {
	return (float)rand() / (float)RAND_MAX;
}

void WireframeOverlay::rebuildPipeline() {
	wgpuRenderPipelineRelease(wgpContext.renderPipelines.at("RP_WIREFRAME_BASE"));
	wgpContext.createRenderPipeline("WIREFRAME_BASE", "RP_WIREFRAME_BASE", VL_PN, std::bind(&WireframeOverlay::OnBindGroupLayouts, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::WRITE, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_Undefined, {}, {m_depthbias, m_depthBiasSlopeScale, 0.0f} }
	);
}

void WireframeOverlay::updateUniforms() {
	for (uint32_t i = 0; i < NUM_OBJECTS; ++i) {
		Object& object = m_objects[i];
		wgpuQueueWriteBuffer(wgpContext.queue, object.uniformBufferLine.getBuffer(), sizeof(uint32_t), &m_thickness, sizeof(float));
		wgpuQueueWriteBuffer(wgpContext.queue, object.uniformBufferLine.getBuffer(), sizeof(uint32_t) + sizeof(float), &m_alphaTthreshold, sizeof(float));
	}
}