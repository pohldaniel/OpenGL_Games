#pragma once

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_STANDARD_VARARGS

#ifdef NUKLEAR_IMPLEMENTATION
	#define NK_IMPLEMENTATION
#endif

#include <nuklear.h>
#include <webgpu.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>

#define NUKLEAR_WGSL       "struct Uniforms {                                                           \n \
                                projection: mat4x4<f32>                                                 \n \
                            }                                                                           \n \
                                                                                                        \n \
                            @group(0) @binding(0) var<uniform> uniforms: Uniforms;                      \n \
                            struct VertexInput {                                                        \n \
                                @location(0) position: vec2<f32>,                                       \n \
                                @location(1) uv: vec2<f32>,                                             \n \
                                @location(2) color: vec4<f32>                                           \n \
                            }                                                                           \n \
                                                                                                        \n \
                            struct VertexOutput {                                                       \n \
                                @builtin(position) position: vec4<f32>,                                 \n \
                                @location(0) uv: vec2<f32>,                                             \n \
                                @location(1) color: vec4<f32>                                           \n \
                            };                                                                          \n \
                                                                                                        \n \
                            @vertex                                                                     \n \
                            fn vs_main(in: VertexInput) -> VertexOutput {                               \n \
                                var out : VertexOutput;                                                 \n \
                                out.position = uniforms.projection * vec4<f32>(in.position, 0.0, 1.0);  \n \
                                out.uv = in.uv;                                                         \n \
                                out.color = in.color;                                                   \n \
                                return out;                                                             \n \
                            }                                                                           \n \
                                                                                                        \n \
                            @group(0) @binding(1) var s : sampler;                                      \n \
                            @group(0) @binding(2) var t : texture_2d<f32>;                              \n \
                                                                                                        \n \
                            @fragment                                                                   \n \
                            fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {                    \n \
                                return in.color * textureSample(t, s, in.uv);                           \n \
                            }"

#define MAX_VERTEX_MEMORY (1024u * 1024u)
#define MAX_INDEX_MEMORY (256u * 1024u)
#define BASE_FONT_SIZE 16.0f

struct NkContext;
extern NkContext nkContext;

extern "C" {
	void nkInit();
	void nkInitFont(const char* path = nullptr);
	void nkInitIcon(const char* path);	
	void nkResize(float width, float height);
	void nkShutDown();
	void nkUpdateInput(int x, int y, bool button, float scrollDelta);
	void nkDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	
	void nkCreateBindGroup(const WgpTexture& texture, WGPUBindGroup& bindgroup);
	void nkCreateRenderPipeline(WGPURenderPipeline& renderPipeline);
}

struct nk_webgpu_vertex {
	float position[2];
	float uv[2];
	uint8_t col[4];
};

struct NkContext {

	struct nk_context context;
	struct nk_buffer commandBuffer, vertexBuffer, indexBuffer;
	struct nk_convert_config convertConfig;
	std::vector<nk_draw_vertex_layout_element> drawVertexLayoutElements;
	struct nk_font_atlas fontAtlas;
	struct nk_font* font = nullptr;
	const struct nk_draw_command* drawCommand = nullptr;

	uint8_t vertexBufferData[MAX_VERTEX_MEMORY];
	uint8_t indexBufferData[MAX_INDEX_MEMORY];

	WGPUBindGroup bindgroup = NULL, bindgroupFont = NULL, bindgroupIcon = NULL;
	WGPUBindGroupLayout bindgroupLayout = NULL;

	WGPURenderPipeline renderPipeline = NULL;
	WGPUPipelineLayout pipelineLayout = NULL;
	WGPUShaderModule shaderModule = NULL;

	WgpBuffer wgpVertexBuffer, wgpIndexBuffer, wgpUniformBuffer;
	WgpTexture wgpTextureNull, wgpTextureFont, wgpTextureIcon;

	std::function<void(nk_context& nkCntxt)> OnFillBuffer = NULL;
};