#include <iostream>
#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpBuffer.h>
#include "Widget.h"

Vector2f Widget::WorldPosition;
Vector2f Widget::WorldScale;
float Widget::WorldOrientation;
std::vector<UiInstance> Widget::Instances;
std::vector<UiBatch> Widget::Batches;

Widget::Widget() : Node(), Object2D(), m_isDirty(true), m_draw(nullptr) {

}

Widget::Widget(const Widget& rhs) : Node(rhs), Object2D(rhs), m_draw(rhs.m_draw) {
	m_isDirty = rhs.m_isDirty;
}

Widget::Widget(Widget&& rhs) noexcept : Node(rhs), Object2D(rhs), m_draw(std::move(rhs.m_draw)) {
	m_isDirty = rhs.m_isDirty;
}

Widget::~Widget() {

}

void Widget::draw() {
	if (m_draw) {
		return m_draw();
	}
	drawDefault();
	drawTree();
		
}

void Widget::drawTree() {
	if (m_children.size() > 0) {
		for (std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = getChildren().begin(); it != getChildren().end(); ++it) {
			static_cast<Widget*>((*it).get())->draw();
		}
	}
}

void Widget::OnTransformChanged() {

	if (m_isDirty) {
		return;
	}

	for (auto&& child : m_children) {
		static_cast<Widget*>(child.get())->OnTransformChanged();
	}

	m_isDirty = true;
}

const Matrix4f& Widget::getWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = static_cast<Widget*>(m_parent)->getWorldTransformation() * m_modelMatrix;

		m_isDirty = false;
	}

	return m_modelMatrix;
}

void Widget::updateWorldTransformation() const {
	if (m_isDirty) {
		m_modelMatrix = getTransformationSOP();
		if (m_parent)
			m_modelMatrix = static_cast<Widget*>(m_parent)->getWorldTransformation() * m_modelMatrix;

		m_isDirty = false;
	}
}

const Vector2f& Widget::getWorldPosition(bool update) const {
	if (update)
		WorldPosition = getWorldTransformation().getTranslation2D();
	return WorldPosition;
}

const Vector2f& Widget::getWorldScale(bool update) const {
	if (update)
		WorldScale = getWorldTransformation().getScale2D();
	return WorldScale;
}

const float Widget::getWorldOrientation(bool update) const {
	if (update)
		WorldOrientation = getWorldTransformation().getRotation2D().getRoll2D();
	return WorldOrientation;
}

void Widget::setScale(float sx, float sy) {
	Object2D::setScale(sx, sy);
	OnTransformChanged();
}

void Widget::setScale(const Vector2f& scale) {
	Object2D::setScale(scale);
	OnTransformChanged();
}

void Widget::setScale(float s) {
	Object2D::setScale(s);
	OnTransformChanged();
}

void Widget::setPosition(float x, float y) {
	Object2D::setPosition(x, y);
	OnTransformChanged();
}

void Widget::setPosition(const Vector2f& position) {
	Object2D::setPosition(position);
	OnTransformChanged();
}

void Widget::setOrigin(float x, float y) {
	Object2D::setOrigin(x, y);
	OnTransformChanged();
}

void Widget::setOrigin(const Vector2f& origin) {
	Object2D::setOrigin(origin);
	OnTransformChanged();
}

void Widget::setOrientation(float degrees) {
	Object2D::setOrientation(degrees);
	OnTransformChanged();
}

void Widget::translate(const Vector2f& trans) {
	Object2D::translate(trans);
	OnTransformChanged();
}

void Widget::translate(const float dx, const float dy) {
	Object2D::translate(dx, dy);
	OnTransformChanged();
}

void Widget::translateRelative(const Vector2f& trans) {
	translateRelative(trans[0], trans[1]);
}

void Widget::translateRelative(const float dx, const float dy) {
	Object2D::translateRelative(dx, dy);
	OnTransformChanged();
}

void Widget::scale(const Vector2f& scale) {
	Object2D::scale(scale);
	OnTransformChanged();
}

void Widget::scale(float sx, float sy) {
	Object2D::scale(sx, sy);
	OnTransformChanged();
}

void Widget::scale(float s) {
	Object2D::scale(s);
	OnTransformChanged();
}

void Widget::rotate(float degrees) {
	Object2D::rotate(degrees);
	OnTransformChanged();
}

void Widget::setDrawFunction(std::function<void()> fun) {
	m_draw = fun;
}

WGPUBindGroup bindgroup = nullptr, bindgroupFont = nullptr, bindgroupIcon = nullptr;
WGPUBindGroupLayout bindgroupLayout = nullptr;
WGPURenderPipeline renderPipeline = nullptr, renderPipelineMask = nullptr, renderPipelineRead = nullptr;
WGPUPipelineLayout pipelineLayout = nullptr;
WGPUShaderModule shaderModule = nullptr;
WgpBuffer storageBuffer, uniformBuffer;

void Widget::Draw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {		
	/*wgpuQueueWriteBuffer(wgpContext.queue, storageBuffer.getBuffer(), 0u, Instances.data(), Instances.size() * sizeof(UiInstance));
	uint32_t firstBatchSize = Instances.size() - 1;

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = wgpCopyDepthStencilAttachment(renderPassDescriptor.depthStencilAttachment);
	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;


	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;
	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, bindgroup, 0u, nullptr);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipeline);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);

	wgpuRenderPassEncoderSetStencilReference(renderPassEncoder, 1);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipelineMask);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 1u);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipelineRead);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 2u);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
	
	Instances.clear();*/

	wgpuQueueWriteBuffer(wgpContext.queue, storageBuffer.getBuffer(), 0u, Instances.data(), Instances.size() * sizeof(UiInstance));

	WGPURenderPassDepthStencilAttachment depthStencilAttachment = wgpCopyDepthStencilAttachment(renderPassDescriptor.depthStencilAttachment);
	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
	depthStencilAttachment.stencilClearValue = 0;

	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, bindgroup, 0u, nullptr);

	wgpuRenderPassEncoderSetStencilReference(renderPassEncoder, 1);

	for (const auto& batch : Batches) {
		if (batch.instanceCount == 0) continue;

		switch (batch.pipelineType) {
		case UiPipelineType::Standard:
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipeline);
			break;
		case UiPipelineType::MaskWrite:
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipelineMask);
			break;
		case UiPipelineType::OutlineRead:
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipelineRead);
			break;
		}

		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, batch.instanceCount, 0u, batch.startIndex);
	}

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);

	Instances.clear();
	Batches.clear();
}

 
void Widget::Init(float width, float height) {

	storageBuffer.createBuffer(400u * sizeof(UiInstance), WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst);
	uniformBuffer.createBuffer(16 * sizeof(float), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	Resize(width, height);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 16u * sizeof(float);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 400u * sizeof(UiInstance);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindgroupLayout = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	CreateRenderPipeline(renderPipeline);
	CreateRenderPipelineMask(renderPipelineMask);
	CreateRenderPipelineRead(renderPipelineRead);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = storageBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(storageBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = bindgroupLayout;
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindgroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void Widget::Resize(float width, float height) {
	

	float ortho[16] = { 2.0f / width, 0.0f,   0.0f, 0.0f,
						0.0f, -2.0f / height, 0.0f, 0.0f,
						0.0f, 0.0f, -1.0f, 0.0f,
					   -1.0f, 1.0f,  0.0f, 1.0f };

	wgpuQueueWriteBuffer(wgpContext.queue, uniformBuffer.getBuffer(), 0u, ortho, 16 * sizeof(float));
}

void Widget::CreateRenderPipeline(WGPURenderPipeline& renderPipeline) {
	shaderModule = wgpCreateShaderFromFile("res/shader/ui.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindgroupLayout;
	pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 0u;
	vertexState.buffers = nullptr;

	WGPUBlendState blendState = {};
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.nextInChain = NULL;
	colorTargetState.format = wgpContext.colorFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0u;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1u;
	fragmentState.targets = &colorTargetState;

	WGPUDepthStencilState depthStencilState = {};
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = wgpContext.depthFormat;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;

	depthStencilState.stencilReadMask = 0u;
	depthStencilState.stencilWriteMask = 0u;

	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayout;
	renderPipelineDescriptor.multisample.count = wgpContext.msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	renderPipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);
}

void Widget::CreateRenderPipelineMask(WGPURenderPipeline& renderPipeline) {
	shaderModule = wgpCreateShaderFromFile("res/shader/ui.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindgroupLayout;
	pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 0u;
	vertexState.buffers = nullptr;

	WGPUBlendState blendState = {};
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.nextInChain = NULL;
	colorTargetState.format = wgpContext.colorFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0u;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1u;
	fragmentState.targets = &colorTargetState;

	WGPUDepthStencilState depthStencilState = {};
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = wgpContext.depthFormat;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Replace;

	depthStencilState.stencilReadMask = 0xFFFFFFFF;
	depthStencilState.stencilWriteMask = 0xFFFFFFFF;

	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayout;
	renderPipelineDescriptor.multisample.count = wgpContext.msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	renderPipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);
}

void Widget::CreateRenderPipelineRead(WGPURenderPipeline& renderPipeline) {
	shaderModule = wgpCreateShaderFromFile("res/shader/ui.wgsl");

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &bindgroupLayout;
	pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = nullptr;
	vertexState.bufferCount = 0u;
	vertexState.buffers = nullptr;

	WGPUBlendState blendState = {};
	blendState.color.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.color.srcFactor = WGPUBlendFactor::WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor::WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.alpha.operation = WGPUBlendOperation::WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor::WGPUBlendFactor_One;
	blendState.alpha.dstFactor = WGPUBlendFactor::WGPUBlendFactor_Zero;

	WGPUColorTargetState colorTargetState = {};
	colorTargetState.nextInChain = NULL;
	colorTargetState.format = wgpContext.colorFormat;
	colorTargetState.blend = &blendState;
	colorTargetState.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentState = {};
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = WGPU_STR("fs_main");
	fragmentState.constantCount = 0u;
	fragmentState.constants = NULL;
	fragmentState.targetCount = 1u;
	fragmentState.targets = &colorTargetState;

	WGPUDepthStencilState depthStencilState = {};
	depthStencilState.nextInChain = NULL;
	depthStencilState.format = wgpContext.depthFormat;
	depthStencilState.depthWriteEnabled = WGPUOptionalBool::WGPUOptionalBool_False;
	depthStencilState.depthCompare = WGPUCompareFunction::WGPUCompareFunction_Less;

	depthStencilState.stencilFront.compare = WGPUCompareFunction::WGPUCompareFunction_NotEqual;
	depthStencilState.stencilFront.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_NotEqual;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;

	depthStencilState.stencilReadMask = 0xFFFFFFFF;
	depthStencilState.stencilWriteMask = 0xFFFFFFFF;
	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = pipelineLayout;
	renderPipelineDescriptor.multisample.count = wgpContext.msaaSampleCount;
	renderPipelineDescriptor.multisample.mask = ~0u;
	renderPipelineDescriptor.multisample.alphaToCoverageEnabled = WGPUOptionalBool::WGPUOptionalBool_False;

	renderPipelineDescriptor.vertex = vertexState;
	renderPipelineDescriptor.fragment = &fragmentState;
	renderPipelineDescriptor.depthStencil = &depthStencilState;

	renderPipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	renderPipelineDescriptor.primitive.stripIndexFormat = WGPUIndexFormat::WGPUIndexFormat_Undefined;
	renderPipelineDescriptor.primitive.frontFace = WGPUFrontFace::WGPUFrontFace_CCW;
	renderPipelineDescriptor.primitive.cullMode = WGPUCullMode_None;

	renderPipeline = wgpuDeviceCreateRenderPipeline(wgpContext.device, &renderPipelineDescriptor);
}