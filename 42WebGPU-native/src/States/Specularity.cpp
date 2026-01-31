#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "Specularity.h"
#include "Application.h"
#include "Globals.h"

Specularity::Specularity(StateMachine& machine) : State(machine, States::SPECULARITY) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpContext.addSampler(wgpCreateSampler());
	m_texture = wgpCreateTexture(512, 512, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
	m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);

	wgpContext.addSahderModule("BOAT", "res/shader/shader_new.wgsl");
	wgpContext.createRenderPipeline("BOAT", "RP_PTNC", VL_PTNC, std::bind(&Specularity::OnBindGroupLayout, this));

	m_boat.loadModel("res/models/fourareen.obj", false, false, false, false, false, true);
	m_boat.generateColors();
	m_wgpBoat.create(m_boat, m_textureView, m_uniformBuffer);
	m_wgpBoat.createBindGroup("RP_PTNC");

	wgpContext.OnDraw = std::bind(&Specularity::OnDraw, this, std::placeholders::_1);

	float cx = cos(m_cameraState.angles[0]);
	float sx = sin(m_cameraState.angles[0]);
	float cy = cos(m_cameraState.angles[1]);
	float sy = sin(m_cameraState.angles[1]);
	Vector3f position = Vector3f(cx * cy, sx * cy, sy) * std::expf(-m_cameraState.zoom);

	m_uniforms.modelMatrix = Matrix4f::IDENTITY;
	m_uniforms.viewMatrix = Matrix4f::LookAt(position, Vector3f(0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	m_uniforms.projectionMatrix = Matrix4f::Perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.01f, 100.0f);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));
}

Specularity::~Specularity() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();

	wgpuTextureDestroy(m_texture);
	wgpuTextureRelease(m_texture);
	m_texture = NULL;

	wgpuTextureViewRelease(m_textureView);
	m_textureView = NULL;
}

void Specularity::fixedUpdate() {

}

void Specularity::update() {

}

void Specularity::render() {
	wgpDraw();
}

void Specularity::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projectionMatrix), &m_uniforms.projectionMatrix, sizeof(Uniforms::projectionMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, modelMatrix), &m_uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));

	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelinesC.at("RP_PTNC"));

	m_wgpBoat.drawRaw(renderPassEncoder);

	if (m_drawUi)
		renderUi(renderPassEncoder);
}

void Specularity::OnMouseMotion(const Event::MouseMoveEvent& event) {
	if (m_drag.active) {
		Vector2f currentMouse = Vector2f(-static_cast<float>(event.x), static_cast<float>(event.y));
		Vector2f delta = (currentMouse - m_drag.startMouse) * m_drag.sensitivity;
		m_cameraState.angles = m_drag.startCameraState.angles + delta;
		m_cameraState.angles[1] = Math::Clamp(m_cameraState.angles[1], -HALF_PI + 1e-5f, HALF_PI - 1e-5f);
		updateViewMatrix();
		m_drag.velocity = delta - m_drag.previousDelta;
		m_drag.previousDelta = delta;
	}
}

void Specularity::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}else if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_drag.active = true;
		m_drag.startMouse = Vector2f(-static_cast<float>(event.x), static_cast<float>(event.y));
		m_drag.startCameraState = m_cameraState;
	}
}

void Specularity::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT){
		Mouse::instance().detach();
	}else if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_drag.active = false;
	}
}

void Specularity::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void Specularity::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Specularity::OnKeyUp(const Event::KeyboardEvent& event) {

}

void Specularity::resize(int deltaW, int deltaH) {
	
}

void Specularity::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {

}

WGPUBindGroupLayout Specularity::OnBindGroupLayout() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[0];
	uniformLayout.binding = 0;
	uniformLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	uniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries[1];
	textureBindingLayout.binding = 1;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries[2];
	samplerBindingLayout.binding = 2;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	return wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
}

void Specularity::updateViewMatrix() {
	float cx = cos(m_cameraState.angles[0]);
	float sx = sin(m_cameraState.angles[0]);
	float cy = cos(m_cameraState.angles[1]);
	float sy = sin(m_cameraState.angles[1]);
	Vector3f position = Vector3f(cx * cy, sx * cy, sy) * std::expf(-m_cameraState.zoom);
	m_uniforms.viewMatrix = Matrix4f::LookAt(position, Vector3f(0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
}

void Specularity::updateDragInertia() {
	constexpr float eps = 1e-4f;
	if (!m_drag.active) {
		if (std::abs(m_drag.velocity[0]) < eps && std::abs(m_drag.velocity[1]) < eps) {
			return;
		}
		m_cameraState.angles += m_drag.velocity;
		m_cameraState.angles[1] = Math::Clamp(m_cameraState.angles[1], -HALF_PI + 1e-5f, HALF_PI - 1e-5f);
		m_drag.velocity *= m_drag.intertia;
		updateViewMatrix();
	}
}