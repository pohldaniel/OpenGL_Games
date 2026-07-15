#include <vector>
#include <WebGPU/WgpContext.h>

#define NUKLEAR_IMPLEMENTATION
#include "NkContext.h"
#include "Application.h"

NkContext nkContext = {};

void nkInit() {

	nkContext.wgpVertexBuffer.createBuffer(MAX_VERTEX_MEMORY, WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst);
	nkContext.wgpIndexBuffer.createBuffer(MAX_INDEX_MEMORY, WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst);
	nkContext.wgpUniformBuffer.createBuffer(16 * sizeof(float), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	nkResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 16u * sizeof(float);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.multisampled = WGPU_FALSE;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	nkContext.bindgroupLayout = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	nkCreateRenderPipeline(nkContext.renderPipeline);

	nkContext.wgpTextureNull.createEmpty(1u, 1u, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat_RGBA8Unorm);
	uint8_t white_pixel[4] = { 255, 255, 255, 255 };

	WGPUTexelCopyTextureInfo destination = {};
	destination.texture = nkContext.wgpTextureNull.getTexture();
	destination.mipLevel = 0u;
	destination.origin = { 0u, 0u, 0u };
	destination.aspect = WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout source = {};
	source.offset = 0u;
	source.bytesPerRow = 4u;
	source.rowsPerImage = 1u;

	WGPUExtent3D size = { 1u , 1u , 1u };
	wgpuQueueWriteTexture(wgpContext.queue, &destination, white_pixel, 4u, &source, &size);

	nkCreateBindGroup(nkContext.wgpTextureNull, nkContext.bindgroup);

	nkContext.convertConfig = {};
	nkContext.convertConfig.shape_AA = NK_ANTI_ALIASING_ON;
	nkContext.convertConfig.line_AA = NK_ANTI_ALIASING_ON;
	nkContext.convertConfig.circle_segment_count = 22;
	nkContext.convertConfig.curve_segment_count = 22;
	nkContext.convertConfig.arc_segment_count = 22;
	nkContext.convertConfig.global_alpha = 1.0f;
	nkContext.convertConfig.null.texture.ptr = nkContext.bindgroup;
	nkContext.convertConfig.null.uv = nk_vec2(0.5f, 0.5f);

	nkContext.drawVertexLayoutElements.resize(4);
	nkContext.drawVertexLayoutElements[0] = { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0 };
	nkContext.drawVertexLayoutElements[1] = { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 2 * sizeof(float) };
	nkContext.drawVertexLayoutElements[2] = { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, 4 * sizeof(float) };
	nkContext.drawVertexLayoutElements[3] = { NK_VERTEX_LAYOUT_END };

	
	nkContext.convertConfig.vertex_layout = nkContext.drawVertexLayoutElements.data();
	nkContext.convertConfig.vertex_size = 20;
	nkContext.convertConfig.vertex_alignment = 4;

	nk_init_default(&nkContext.context, NULL);
	nk_buffer_init_default(&nkContext.commandBuffer);

	nk_buffer_init_fixed(&nkContext.vertexBuffer, nkContext.vertexBufferData, MAX_VERTEX_MEMORY);
	nk_buffer_init_fixed(&nkContext.indexBuffer, nkContext.indexBufferData, MAX_INDEX_MEMORY);

	nk_style_default(&nkContext.context);
}

void nkInitFont(const char* path) {
	nk_font_atlas_init_default(&nkContext.fontAtlas);
	nk_font_atlas_begin(&nkContext.fontAtlas);

	if (path) {
		struct nk_font_config config_font = nk_font_config(0.0f);
		config_font.oversample_h = 3;
		config_font.oversample_v = 3;
		nkContext.font = nk_font_atlas_add_from_file(&nkContext.fontAtlas, path, BASE_FONT_SIZE, &config_font);
	}else {
		nkContext.font = nk_font_atlas_add_default(&nkContext.fontAtlas, BASE_FONT_SIZE, NULL);
	}

	const void* image_pixels;
	int atlas_width, atlas_height;
	image_pixels = nk_font_atlas_bake(&nkContext.fontAtlas, &atlas_width, &atlas_height, NK_FONT_ATLAS_RGBA32);

	nkContext.wgpTextureFont.createEmpty(atlas_width, atlas_height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat_RGBA8Unorm);

	WGPUTexelCopyTextureInfo font_destination = {};
	font_destination.texture = nkContext.wgpTextureFont.getTexture();
	font_destination.mipLevel = 0u;
	font_destination.origin = { 0u, 0u, 0u };
	font_destination.aspect = WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout font_source = {};
	font_source.offset = 0u;
	font_source.bytesPerRow = (uint32_t)atlas_width * 4;
	font_source.rowsPerImage = (uint32_t)atlas_height;

	WGPUExtent3D font_size = { (uint32_t)atlas_width, (uint32_t)atlas_height, 1u };
	wgpuQueueWriteTexture(wgpContext.queue, &font_destination, image_pixels, (size_t)(atlas_width * atlas_height * 4), &font_source, &font_size);

	nkCreateBindGroup(nkContext.wgpTextureFont, nkContext.bindgroupFont);

	nk_handle font_handle;
	font_handle.ptr = nkContext.bindgroupFont;

	nk_font_atlas_end(&nkContext.fontAtlas, font_handle, &nkContext.convertConfig.null);
	nk_style_set_font(&nkContext.context, &nkContext.font->handle);
}

void nkInitIcon(const char* path) {
	nkContext.wgpTextureIcon.loadFromFile(path, true);
	nkCreateBindGroup(nkContext.wgpTextureIcon, nkContext.bindgroupIcon);
}

void nkResize(float width, float height) {
	float ortho[16] = { 2.0f / width, 0.0f,   0.0f, 0.0f,
					    0.0f, -2.0f / height, 0.0f, 0.0f,
					    0.0f, 0.0f, -1.0f, 0.0f,
					   -1.0f, 1.0f,  0.0f, 1.0f };

	wgpuQueueWriteBuffer(wgpContext.queue, nkContext.wgpUniformBuffer.getBuffer(), 0u, ortho, 16 * sizeof(float));
}

void nkShutDown() {
	nkContext.wgpVertexBuffer.markForDelete();
	nkContext.wgpVertexBuffer.cleanup();

	nkContext.wgpIndexBuffer.markForDelete();
	nkContext.wgpIndexBuffer.cleanup();

	nkContext.wgpUniformBuffer.markForDelete();
	nkContext.wgpUniformBuffer.cleanup();

	nkContext.wgpTextureNull.markForDelete();
	nkContext.wgpTextureNull.cleanup();

	nkContext.wgpTextureFont.markForDelete();
	nkContext.wgpTextureFont.cleanup();

	nkContext.wgpTextureIcon.markForDelete();
	nkContext.wgpTextureIcon.cleanup();

	if (nkContext.bindgroup) {
		wgpuBindGroupRelease(nkContext.bindgroup);
		nkContext.bindgroup = NULL;
	}

	if (nkContext.bindgroupFont) {
		wgpuBindGroupRelease(nkContext.bindgroupFont);
		nkContext.bindgroupFont = NULL;
	}

	if (nkContext.bindgroupIcon) {
		wgpuBindGroupRelease(nkContext.bindgroupIcon);
		nkContext.bindgroupIcon = NULL;
	}

	if (nkContext.bindgroupLayout) {
		wgpuBindGroupLayoutRelease(nkContext.bindgroupLayout);
		nkContext.bindgroupLayout = NULL;
	}

	if (nkContext.shaderModule) {
		wgpuShaderModuleRelease(nkContext.shaderModule);
		nkContext.shaderModule = NULL;
	}

	if (nkContext.pipelineLayout) {
		wgpuPipelineLayoutRelease(nkContext.pipelineLayout);
		nkContext.pipelineLayout = NULL;
	}

	nk_free(&nkContext.context);
	nk_font_atlas_clear(&nkContext.fontAtlas);
	nk_buffer_free(&nkContext.vertexBuffer);
	nk_buffer_free(&nkContext.indexBuffer);
	nk_buffer_free(&nkContext.commandBuffer);

	nkContext.drawVertexLayoutElements.clear();
	nkContext.drawVertexLayoutElements.shrink_to_fit();
	nkContext.OnFillBuffer = NULL;
}

void nkUpdateInput(int x, int y, bool button, float scrollDelta) {
	nk_input_begin(&nkContext.context);
	nk_input_motion(&nkContext.context, x, y);
	nk_input_button(&nkContext.context, NK_BUTTON_LEFT, x, y, button);
	nk_input_scroll(&nkContext.context, nk_vec2(0.0f, scrollDelta));
	nk_input_end(&nkContext.context);
}

void nkDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	if (nkContext.OnFillBuffer)
		nkContext.OnFillBuffer(nkContext.context);

	nk_buffer_clear(&nkContext.vertexBuffer);
	nk_buffer_clear(&nkContext.indexBuffer);

	nk_convert(&nkContext.context, &nkContext.commandBuffer, &nkContext.vertexBuffer, &nkContext.indexBuffer, &nkContext.convertConfig);
	

	uint32_t vertex_count = nkContext.vertexBuffer.needed / sizeof(nk_webgpu_vertex);
	nk_webgpu_vertex* vertices = (nk_webgpu_vertex*)nkContext.vertexBufferData;
	nkContext.indexBuffer.needed = (nkContext.indexBuffer.needed + 3) & ~3;
	wgpuQueueWriteBuffer(wgpContext.queue, nkContext.wgpVertexBuffer.getBuffer(), 0, nkContext.vertexBufferData, nkContext.vertexBuffer.needed);
	wgpuQueueWriteBuffer(wgpContext.queue, nkContext.wgpIndexBuffer.getBuffer(), 0, nkContext.indexBufferData, nkContext.indexBuffer.needed);

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, nkContext.renderPipeline);

	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, nkContext.wgpVertexBuffer.getBuffer(), 0u, MAX_VERTEX_MEMORY);
	wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, nkContext.wgpIndexBuffer.getBuffer(), WGPUIndexFormat_Uint16, 0u, MAX_INDEX_MEMORY);

	uint32_t index_offset = 0;
	nk_draw_foreach(nkContext.drawCommand, &nkContext.context, &nkContext.commandBuffer) {
		if (!nkContext.drawCommand->elem_count) continue;

		WGPUBindGroup bindGroup = (WGPUBindGroup)nkContext.drawCommand->texture.ptr;

		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, bindGroup, 0u, NULL);

		float scissor_x = nkContext.drawCommand->clip_rect.x < 0.0f ? 0.0f : nkContext.drawCommand->clip_rect.x;
		float scissor_y = nkContext.drawCommand->clip_rect.y < 0.0f ? 0.0f : nkContext.drawCommand->clip_rect.y;
		float scissor_w = nkContext.drawCommand->clip_rect.w;
		float scissor_h = nkContext.drawCommand->clip_rect.h;

		if (scissor_x + scissor_w > static_cast<float>(Application::Width)) {
			scissor_w = static_cast<float>(Application::Width) - scissor_x;
		}

		if (scissor_y + scissor_h > static_cast<float>(Application::Height)) {
			scissor_h = static_cast<float>(Application::Height) - scissor_y;
		}

		if (scissor_w <= 0.0f || scissor_h <= 0.0f) {
			continue;
		}

		wgpuRenderPassEncoderSetScissorRect(renderPassEncoder, (uint32_t)scissor_x, (uint32_t)scissor_y, (uint32_t)scissor_w, (uint32_t)scissor_h);
		wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, nkContext.drawCommand->elem_count, 1, index_offset, 0, 0);
		index_offset += nkContext.drawCommand->elem_count;
	}
	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
	nk_clear(&nkContext.context);
	nk_buffer_clear(&nkContext.commandBuffer);
}

void nkCreateBindGroup(const WgpTexture& texture, WGPUBindGroup& bindgroup) {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = nkContext.wgpUniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = 16u * sizeof(float);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = texture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = nkContext.bindgroupLayout;
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindgroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void nkCreateRenderPipeline(WGPURenderPipeline& renderPipeline) {
	std::vector<WGPUVertexAttribute> vertexAttribute = std::vector<WGPUVertexAttribute>(3);

	vertexAttribute[0].shaderLocation = 0u;
	vertexAttribute[0].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
	vertexAttribute[0].offset = 0u;

	vertexAttribute[1].shaderLocation = 1u;
	vertexAttribute[1].format = WGPUVertexFormat::WGPUVertexFormat_Float32x2;
	vertexAttribute[1].offset = 2 * sizeof(float);

	vertexAttribute[2].shaderLocation = 2u;
	vertexAttribute[2].format = WGPUVertexFormat::WGPUVertexFormat_Unorm8x4;
	vertexAttribute[2].offset = 4 * sizeof(float);

	WGPUVertexBufferLayout vertexBufferLayout = {};
	vertexBufferLayout.attributeCount = (uint32_t)vertexAttribute.size();
	vertexBufferLayout.attributes = vertexAttribute.data();
	vertexBufferLayout.arrayStride = 20u;
	vertexBufferLayout.stepMode = WGPUVertexStepMode::WGPUVertexStepMode_Vertex;

	nkContext.shaderModule = wgpCreateShaderFromString(NUKLEAR_WGSL);

	WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
	pipelineLayoutDescriptor.bindGroupLayoutCount = 1u;
	pipelineLayoutDescriptor.bindGroupLayouts = &nkContext.bindgroupLayout;
	nkContext.pipelineLayout = wgpuDeviceCreatePipelineLayout(wgpContext.device, &pipelineLayoutDescriptor);

	WGPUVertexState vertexState = {};
	vertexState.module = nkContext.shaderModule;
	vertexState.entryPoint = WGPU_STR("vs_main");
	vertexState.constantCount = 0u;
	vertexState.constants = NULL;
	vertexState.bufferCount = 1u;
	vertexState.buffers = &vertexBufferLayout;

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
	fragmentState.module = nkContext.shaderModule;
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
	depthStencilState.stencilFront.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.compare = WGPUCompareFunction::WGPUCompareFunction_Always;
	depthStencilState.stencilBack.failOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.depthFailOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilBack.passOp = WGPUStencilOperation::WGPUStencilOperation_Keep;
	depthStencilState.stencilReadMask = 0u;
	depthStencilState.stencilWriteMask = 0u;
	depthStencilState.depthBias = 0;
	depthStencilState.depthBiasSlopeScale = 0.0f;
	depthStencilState.depthBiasClamp = 0.0f;

	WGPURenderPipelineDescriptor renderPipelineDescriptor = {};
	renderPipelineDescriptor.layout = nkContext.pipelineLayout;
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