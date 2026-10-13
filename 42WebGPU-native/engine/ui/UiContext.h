#pragma once

#include <vector>
#include <webgpu.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpTexture.h>

#define UI_WGSL              "struct SpriteInstance {                                                                                            \n \
                                  transform: mat4x4<f32>,                                                                                        \n \
                                  color : vec4<f32>,                                                                                             \n \
                                  textureRect : vec4<f32>,                                                                                       \n \
                                  textureLayer : u32,                                                                                            \n \
                                  padding : vec3<u32>,                                                                                           \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              struct Uniforms {                                                                                                  \n \
                                  projection: mat4x4<f32>                                                                                        \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @group(0) @binding(0) var<uniform> uniforms: Uniforms;                                                             \n \
                              @group(0) @binding(1) var<storage, read> instances: array<SpriteInstance>;                                         \n \
                              @group(0) @binding(2) var smplr : sampler;                                                                         \n \
                              @group(0) @binding(3) var uiTextures : texture_2d_array<f32>;                                                      \n \
                                                                                                                                                 \n \
                              struct VertexOutput {                                                                                              \n \
                                  @builtin(position) position: vec4<f32>,                                                                        \n \
                                  @location(0) uv: vec2<f32>,                                                                                    \n \
                                  @location(1) color: vec4<f32>,                                                                                 \n \
                                  @location(2) @interpolate(flat) textureLayer: u32,                                                             \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @vertex                                                                                                            \n \
                              fn vs_main(@builtin(vertex_index) vertexIndex: u32, @builtin(instance_index) instanceIndex: u32) -> VertexOutput { \n \
                                  let instance = instances[instanceIndex];                                                                       \n \
                                  var localPos = vec2<f32>(0.0, 0.0);                                                                            \n \
                                  var uv = vec2<f32>(0.0, 0.0);                                                                                  \n \
                                                                                                                                                 \n \
                                  switch (vertexIndex) {                                                                                         \n \
                                      case 0u: { localPos = vec2<f32>(0.0, 0.0); uv = vec2<f32>(0.0, 0.0); }                                     \n \
                                      case 1u: { localPos = vec2<f32>(1.0, 0.0); uv = vec2<f32>(1.0, 0.0); }                                     \n \
                                      case 2u: { localPos = vec2<f32>(0.0, 1.0); uv = vec2<f32>(0.0, 1.0); }                                     \n \
                                      case 3u: { localPos = vec2<f32>(0.0, 1.0); uv = vec2<f32>(0.0, 1.0); }                                     \n \
                                      case 4u: { localPos = vec2<f32>(1.0, 0.0); uv = vec2<f32>(1.0, 0.0); }                                     \n \
                                      case 5u: { localPos = vec2<f32>(1.0, 1.0); uv = vec2<f32>(1.0, 1.0); }                                     \n \
                                      default: {}                                                                                                \n \
                                  }                                                                                                              \n \
                                                                                                                                                 \n \
                                  let finalUV = instance.textureRect.xy + (uv * instance.textureRect.zw);                                        \n \
                                                                                                                                                 \n \
                                  var output : VertexOutput;                                                                                     \n \
                                  output.position = uniforms.projection * instance.transform * vec4<f32>(localPos, 0.0, 1.0);                    \n \
                                  output.uv = finalUV;                                                                                           \n \
                                  output.color = instance.color;                                                                                 \n \
                                  output.textureLayer = instance.textureLayer;                                                                   \n \
                                                                                                                                                 \n \
                                  return output;                                                                                                 \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @fragment                                                                                                          \n \
                              fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {                                                           \n \
                                  return textureSample(uiTextures, smplr, in.uv, in.textureLayer) * in.color;                                    \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @fragment                                                                                                          \n \
                              fn fs_main_clear(in: VertexOutput) {                                                                               \n \
                                                                                                                                                 \n \
                              }"

#define UI_TEXT_WGSL         "struct SpriteInstance {                                                                                            \n \
                                  transform: mat4x4<f32>,                                                                                        \n \
                                  color : vec4<f32>,                                                                                             \n \
                                  textureRect : vec4<f32>,                                                                                       \n \
                                  textureLayer : u32,                                                                                            \n \
                                  padding : vec3<u32>,                                                                                           \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              struct Uniforms {                                                                                                  \n \
                                  projection: mat4x4<f32>                                                                                        \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @group(0) @binding(0) var<uniform> uniforms: Uniforms;                                                             \n \
                              @group(0) @binding(1) var<storage, read> instances: array<SpriteInstance>;                                         \n \
                              @group(0) @binding(2) var smplr : sampler;                                                                         \n \
                              @group(0) @binding(3) var uiTextures : texture_2d_array<f32>;                                                      \n \
                                                                                                                                                 \n \
                              struct VertexOutput {                                                                                              \n \
                                  @builtin(position) position: vec4<f32>,                                                                        \n \
                                  @location(0) uv: vec2<f32>,                                                                                    \n \
                                  @location(1) color: vec4<f32>,                                                                                 \n \
                                  @location(2) @interpolate(flat) textureLayer: u32,                                                             \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @vertex                                                                                                            \n \
                              fn vs_main(@builtin(vertex_index) vertexIndex: u32, @builtin(instance_index) instanceIndex: u32) -> VertexOutput { \n \
                                  let instance = instances[instanceIndex];                                                                       \n \
                                  var localPos = vec2<f32>(0.0, 0.0);                                                                            \n \
                                  var uv = vec2<f32>(0.0, 0.0);                                                                                  \n \
                                                                                                                                                 \n \
                                  switch (vertexIndex) {                                                                                         \n \
                                      case 0u: { localPos = vec2<f32>(0.0, 0.0); uv = vec2<f32>(0.0, 0.0); }                                     \n \
                                      case 1u: { localPos = vec2<f32>(1.0, 0.0); uv = vec2<f32>(1.0, 0.0); }                                     \n \
                                      case 2u: { localPos = vec2<f32>(0.0, 1.0); uv = vec2<f32>(0.0, 1.0); }                                     \n \
                                      case 3u: { localPos = vec2<f32>(0.0, 1.0); uv = vec2<f32>(0.0, 1.0); }                                     \n \
                                      case 4u: { localPos = vec2<f32>(1.0, 0.0); uv = vec2<f32>(1.0, 0.0); }                                     \n \
                                      case 5u: { localPos = vec2<f32>(1.0, 1.0); uv = vec2<f32>(1.0, 1.0); }                                     \n \
                                      default: {}                                                                                                \n \
                                  }                                                                                                              \n \
                                                                                                                                                 \n \
                                  let finalUV = instance.textureRect.xy + (uv * instance.textureRect.zw);                                        \n \
                                                                                                                                                 \n \
                                  var output : VertexOutput;                                                                                     \n \
                                  output.position = uniforms.projection * instance.transform * vec4<f32>(localPos, 0.0, 1.0);                    \n \
                                  output.uv = finalUV;                                                                                           \n \
                                  output.color = instance.color;                                                                                 \n \
                                  output.textureLayer = instance.textureLayer;                                                                   \n \
                                                                                                                                                 \n \
                                  return output;                                                                                                 \n \
                              }                                                                                                                  \n \
                                                                                                                                                 \n \
                              @fragment                                                                                                          \n \
                              fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {                                                           \n \
                                   let sampledColor = textureSample(uiTextures, smplr, in.uv, in.textureLayer);                                  \n \
                                   let alphaMask = sampledColor.r;                                                                               \n \
                                   let textAlpha = in.color.a * alphaMask;                                                                       \n \
                                   return vec4<f32>(in.color.rgb, textAlpha);                                                                    \n \
                              }"

struct UiContext;              
extern UiContext uiContext;

extern "C" {
	void uiInit(float width, float height);
	void uiInitTextureArray(uint32_t width = 512u, uint32_t height = 512u, uint32_t layers = 16u);

	void uiResize(float width, float height);
	void uiShutDown();
	void uiDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);

	void uiCreateRenderPipeline(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineMask(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineRead(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineClear(WGPURenderPipeline& renderPipeline);
	void uiCreateRenderPipelineText(WGPURenderPipeline& renderPipeline);
	void uiCreateBindGroup(WGPUBindGroup& bindgroup);
	void uiCreateBindGroupText(WGPUBindGroup& bindgroup);
}
	
struct UiInstance {
	float transform[16];
	float color[4];
	float textureRect[4];
	uint32_t textureLayer;
	uint32_t padding[7];
};

enum class UiPipelineType {
	Standard,
	Clear,
	MaskWrite,
	OutlineRead,
	Text
};

struct UiBatch {
	UiPipelineType pipelineType;
	uint32_t startIndex;
	uint32_t instanceCount;
	uint32_t scissorX = 0;
	uint32_t scissorY = 0;
	uint32_t scissorWidth = 0;
	uint32_t scissorHeight = 0;
};

struct UiLayer {
	std::vector<UiBatch> batches;
};

struct UiContext {
	float width;
	float height;

	WgpTexture texture;

	WGPUBindGroup bindgroup = nullptr, bindgroupText = nullptr;
	WGPUBindGroupLayout bindgroupLayout = nullptr, bindgroupLayoutText = nullptr;
	WGPURenderPipeline renderPipeline = nullptr, renderPipelineMask = nullptr, renderPipelineRead = nullptr, renderPipelineClear = nullptr, renderPipelineText = nullptr;
	//WGPUTextureView textureView = nullptr;
	WGPUTextureView textureViewFont = nullptr;

	WgpBuffer wgpStorageBuffer, wgpUniformBuffer;

	std::vector<UiInstance> uiInstances;
	std::vector<UiLayer> uiLayers;
	UiLayer* currentActiveLayer = nullptr;

	uint32_t activeScissorX = 0u;
	uint32_t activeScissorY = 0u;
	uint32_t activeScissorWidth = 0u;
	uint32_t activeScissorHeight = 0u;
};