#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "Compute.h"
#include "Application.h"
#include "Globals.h"

Compute::Compute(StateMachine& machine) : State(machine, States::COMPUTE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_uniformBuffer.createBuffer(sizeof(UniformsCompute), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpContext.addSampler(wgpCreateSampler());

	wgpContext.addSahderModule("COMPUTE", "res/shader/compute.wgsl");
	wgpContext.createComputePipeline("COMPUTE", "CP_COMPUTE", std::bind(&Compute::OnBindGroupLayout, this));

	uint32_t width, height;
	uint8_t* pixelData = WgpTexture::LoadFromFile("res/textures/input.jpg", width, height, true);


	WGPUExtent3D textureSize = { width, height, 1 };

	// Create texture
	WGPUTextureDescriptor textureDesc = {};
	textureDesc.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;;
	textureDesc.format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
	textureDesc.size = textureSize;
	textureDesc.sampleCount = 1;
	textureDesc.viewFormatCount = 0;
	textureDesc.viewFormats = nullptr;
	textureDesc.mipLevelCount = 1;

	textureDesc.label = WGPU_STR("input");
	textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
	inputTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);

	textureDesc.label = WGPU_STR("output");
	textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_StorageBinding | WGPUTextureUsage_CopySrc;
	outputTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDesc);

	// Upload texture data for MIP level 0 to the GPU
	WGPUTexelCopyTextureInfo destination = {};
	destination.texture = inputTexture;
	destination.origin = { 0, 0, 0 };
	destination.aspect = WGPUTextureAspect_All;
	destination.mipLevel = 0;

	WGPUTexelCopyBufferLayout source = {};
	source.offset = 0;
	source.bytesPerRow = 4 * textureSize.width;
	source.rowsPerImage = textureSize.height;
	wgpuQueueWriteTexture(wgpContext.queue, &destination, pixelData, (size_t)(4 * width * height), &source, &textureSize);
	free(pixelData);

	WGPUTextureViewDescriptor textureViewDesc = {};
	textureViewDesc.aspect = WGPUTextureAspect::WGPUTextureAspect_All;
	textureViewDesc.baseArrayLayer = 0;
	textureViewDesc.arrayLayerCount = 1;
	textureViewDesc.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDesc.format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
	textureViewDesc.mipLevelCount = 1;
	textureViewDesc.baseMipLevel = 0;

	textureViewDesc.label = WGPU_STR("input");
	inputTextureView = wgpuTextureCreateView(inputTexture, &textureViewDesc);

	textureViewDesc.label = WGPU_STR("output");
	outputTextureView = wgpuTextureCreateView(outputTexture, &textureViewDesc);

	m_bindGroup = createBindGroup(inputTextureView, outputTextureView, m_uniformBuffer.getBuffer());

	wgpContext.OnDraw = std::bind(&Compute::OnDraw, this, std::placeholders::_1);
}

Compute::~Compute() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();

}

void Compute::fixedUpdate() {

}

void Compute::update() {
	
}

void Compute::render() {
	wgpDraw();
	OnCompute();
}

void Compute::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {
	renderUi(renderPassEncoder, m_force);
}

void Compute::OnCompute() {
	if (m_shouldCompute) {
		wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(UniformsCompute));

		WGPUCommandEncoderDescriptor commandEncoderDesc = {};
		commandEncoderDesc.label = WGPU_STR("command_encoder");
		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDesc);


		WGPUComputePassDescriptor computePassDesc = {};
		computePassDesc.timestampWrites = NULL;
		WGPUComputePassEncoder computePass = wgpuCommandEncoderBeginComputePass(encoder, &computePassDesc);
		wgpuComputePassEncoderSetPipeline(computePass, wgpContext.computePipelines.at("CP_COMPUTE"));

		for (uint32_t i = 0; i < 1; ++i) {
			wgpuComputePassEncoderSetBindGroup(computePass, 0, m_bindGroup, 0, nullptr);
			uint32_t invocationCountX = wgpuTextureGetWidth(inputTexture);
			uint32_t invocationCountY = wgpuTextureGetHeight(inputTexture);
			uint32_t workgroupSizePerDim = 8;
			uint32_t workgroupCountX = (invocationCountX + workgroupSizePerDim - 1) / workgroupSizePerDim;
			uint32_t workgroupCountY = (invocationCountY + workgroupSizePerDim - 1) / workgroupSizePerDim;
			wgpuComputePassEncoderDispatchWorkgroups(computePass, workgroupCountX, workgroupCountY, 1);
		}

		wgpuComputePassEncoderEnd(computePass);
		WGPUCommandBufferDescriptor commandBufferDescriptor = {};
		commandBufferDescriptor.label = WGPU_STR("command_buffer");
		WGPUCommandBuffer commands = wgpuCommandEncoderFinish(encoder, &commandBufferDescriptor);

		wgpuQueueSubmit(wgpContext.queue, 1, &commands);
		m_shouldCompute = false;
		m_force = false;
	}
}

void Compute::OnMouseMotion(Event::MouseMoveEvent& event) {
	
}

void Compute::OnMouseButtonDown(Event::MouseButtonEvent& event) {
    if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Compute::OnMouseButtonUp(Event::MouseButtonEvent& event) {
    if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Compute::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Compute::OnKeyDown(Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Compute::OnKeyUp(Event::KeyboardEvent& event) {

}

void Compute::resize(int deltaW, int deltaH) {

}

void Compute::renderUi(const WGPURenderPassEncoder& renderPassEncoder, bool force) {
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

	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		float offset = 0.0f;
		float width = 0.0f;

		// Input image
		width = wgpuTextureGetWidth(inputTexture) * m_settings.scale;
		drawList->AddImage((ImTextureID)inputTextureView, { offset, 0 }, { offset + width, wgpuTextureGetHeight(inputTexture) * m_settings.scale});
		offset += width;

		// Output image
		width = wgpuTextureGetWidth(outputTexture) * m_settings.scale;
		drawList->AddImage((ImTextureID)outputTextureView, { offset, 0 }, {offset + width, wgpuTextureGetHeight(outputTexture) * m_settings.scale});
		offset += width;
	}

	bool changed = false;
	ImGui::Begin("Parameters");
	float minimum = m_parameters.normalize ? 0.0f : -2.0f;
	float maximum = m_parameters.normalize ? 4.0f : 2.0f;
	changed = ImGui::Combo("Filter Type", (int*)&m_parameters.filterType, "Sum\0Maximum\0Minimum\0") || changed;
	changed = ImGui::SliderFloat3("Kernel X", &m_parameters.kernel[0][0], minimum, maximum) || changed;
	changed = ImGui::SliderFloat3("Kernel Y", &m_parameters.kernel[1][0], minimum, maximum) || changed;
	changed = ImGui::SliderFloat3("Kernel Z", &m_parameters.kernel[2][0], minimum, maximum) || changed;
	changed = ImGui::Checkbox("Normalize", &m_parameters.normalize) || changed;
	ImGui::End();

	if (changed) {
		float sum = Vector3f::Dot(Vector3f(1.0, 1.0, 1.0), m_parameters.kernel * Vector3f(1.0));
		m_uniforms.kernel = m_parameters.normalize && std::abs(sum) > 1e-6
			? m_parameters.kernel / sum
			: m_parameters.kernel;
		m_uniforms.filterType = (uint32_t)m_parameters.filterType;
	}
	m_shouldCompute = changed || force;

	ImGui::Begin("Settings");
	ImGui::SliderFloat("Scale", &m_settings.scale, 0.0f, 2.0f);
	ImGui::End();
	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

WGPUBindGroupLayout Compute::OnBindGroupLayout() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	WGPUBindGroupLayoutEntry& inputImageLayout = bindingLayoutEntries[0];
	inputImageLayout.binding = 0;
	inputImageLayout.texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	inputImageLayout.texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	inputImageLayout.visibility = WGPUShaderStage_Compute;

	WGPUBindGroupLayoutEntry& outputImageLayout = bindingLayoutEntries[1];
	outputImageLayout.binding = 1;
	outputImageLayout.storageTexture.access = WGPUStorageTextureAccess::WGPUStorageTextureAccess_WriteOnly;
	outputImageLayout.storageTexture.format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
	outputImageLayout.storageTexture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	outputImageLayout.visibility = WGPUShaderStage_Compute;

	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[2];
	uniformLayout.binding = 2;
	uniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(UniformsCompute);
	uniformLayout.visibility = WGPUShaderStage_Compute;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();
	return wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
}

WGPUBindGroup Compute::createBindGroup(const WGPUTextureView& inputTextureView, const WGPUTextureView& outputTextureView, const WGPUBuffer& uniformBuffer) {
	std::vector<WGPUBindGroupEntry> entries(3);

	entries[0].binding = 0;
	entries[0].textureView = inputTextureView;

	entries[1].binding = 1;
	entries[1].textureView = outputTextureView;

	entries[2].binding = 2;
	entries[2].buffer = uniformBuffer;
	entries[2].offset = 0;
	entries[2].size = sizeof(UniformsCompute);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuComputePipelineGetBindGroupLayout(wgpContext.computePipelines.at("CP_COMPUTE"), 0);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}