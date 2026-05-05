#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

#define SHADOW_WGSL             "struct VertexInput {                                                        \n \
                                     @location(0) position: vec3f,                                           \n \
							         @location(1) normal: vec3f                                              \n \
                                 };                                                                          \n \
								 	 												                         \n \
                                 struct Uniforms {                                                           \n \
                                     projection: mat4x4f,                                                    \n \
                                     view: mat4x4f,                                                          \n \
                                     env: mat4x4f,                                                           \n \
                                     model:mat4x4f,                                                          \n \
                                     normal: mat4x4f,                                                        \n \
                                     color: vec4f,                                                           \n \
                                     camPos: vec3f,                                                          \n \
                                     lightVP: mat4x4f,                                                       \n \
                                     shadow: mat4x4f,                                                        \n \
                                     lightPos: vec3f                                                         \n \
							     };                                                                          \n \
                                                                                                             \n \
                                 @group(0) @binding(0) var<uniform> uniforms: Uniforms;                      \n \
                                                                                                             \n \
                                 @vertex                                                                     \n \
                                 fn vs_main(in: VertexInput) -> @builtin(position) vec4f {                   \n \
                                    return uniforms.lightVP * uniforms.model * vec4(in.position, 1.0);       \n \
							     }"


#define SHADOW_MAPPING_WGSL     "struct VertexInput {                                                                                       \n \
                                     @location(0) position: vec3f,                                                                          \n \
							         @location(1) normal: vec3f                                                                             \n \
                                 };                                                                                                         \n \
                                                                                                                                            \n \
							     struct VertexOutput {                                                                                      \n \
                                      @builtin(position) position: vec4f,                                                                   \n \
                                      @location(0) shadowPos: vec4f,                                                                        \n \
                                      @location(1) fragPos: vec3f,                                                                          \n \
                                      @location(2) fragNorm: vec3f                                                                          \n \
                                 };														                                                    \n \
                                                                                                                                            \n \
                                 struct Uniforms {                                                                                          \n \
                                     projection: mat4x4f,                                                                                   \n \
                                     view: mat4x4f,                                                                                         \n \
                                     env: mat4x4f,                                                                                          \n \
                                     model:mat4x4f,                                                                                         \n \
                                     normal: mat4x4f,                                                                                       \n \
                                     color: vec4f,                                                                                          \n \
                                     camPos: vec3f,                                                                                         \n \
                                     lightVP: mat4x4f,                                                                                      \n \
                                     shadow: mat4x4f,                                                                                       \n \
                                     lightPos: vec3f                                                                                        \n \
							     };                                                                                                         \n \
                                                                                                                                            \n \
                                 const shadowDepthTextureSize : f32 = 1024.0;                                                               \n \
                                 const albedo = vec3f(0.9);                                                                                 \n \
                                 const ambientFactor = 0.2;                                                                                 \n \
                                                                                                                                            \n \
                                 @group(0) @binding(0) var<uniform> uniforms: Uniforms;                                                     \n \
                                 @group(0) @binding(1) var shadowMap : texture_depth_2d;                                                    \n \
                                 @group(0) @binding(2) var shadowSampler : sampler_comparison;                                              \n \
                                                                                                                                            \n \
                                 @vertex                                                                                                    \n \
                                 fn vs_main(in: VertexInput) -> VertexOutput {                                                              \n \
                                     var out : VertexOutput;                                                                                \n \
                                     out.position = uniforms.projection * uniforms.view * uniforms.model * vec4f(in.position, 1.0);         \n \
                                     out.shadowPos = uniforms.shadow * uniforms.model * vec4f(in.position, 1.0);                            \n \
                                     out.fragPos = out.position.xyz;                                                                        \n \
                                     out.fragNorm = in.normal;                                                                              \n \
                                     return out;                                                                                            \n \
                                 }                                                                                                          \n \
                                                                                                                                            \n \
                                 @fragment                                                                                                  \n \
                                 fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {                                                   \n \
                                     var visibility = 0.0;                                                                                  \n \
                                     let oneOverShadowDepthTextureSize = 1.0 / shadowDepthTextureSize;                                      \n \
                                     for (var y = -1; y <= 1; y++) {                                                                        \n \
                                         for (var x = -1; x <= 1; x++) {                                                                    \n \
                                             let offset = vec2f(vec2(x, y)) * oneOverShadowDepthTextureSize;                                \n \
                                                                                                                                            \n \
                                             visibility += textureSampleCompare(                                                            \n \
                                                 shadowMap, shadowSampler,                                                                  \n \
                                                 in.shadowPos.xy + offset, in.shadowPos.z - 0.007                                           \n \
                                             );                                                                                             \n \
                                         }                                                                                                  \n \
                                     }                                                                                                      \n \
                                     visibility /= 9.0;                                                                                     \n \
                                                                                                                                            \n \
                                     let lambertFactor = max(dot(normalize(uniforms.lightPos - in.fragPos), normalize(in.fragNorm)), 0.0);  \n \
                                     let lightingFactor = min(ambientFactor + visibility * lambertFactor, 1.0);                             \n \
                                                                                                                                            \n \
                                     return vec4(lightingFactor * albedo, 1.0);                                                             \n \
                                }"

class ShadowMapping : public State, public MouseEventListener, public KeyboardEventListener {

public:

	ShadowMapping(StateMachine& machine);
	~ShadowMapping();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsShadow();
	std::vector<WGPUBindGroup> OnBindGroupsShadow();
	void OnDrawShadow(const WGPURenderPassEncoder& renderPassEncoder);

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	Uniforms m_uniforms;
	AssimpModel m_dragon;
	Shape m_quad;
	Matrix4f m_lightProjection, m_lightView, m_shadow;

	WgpModel m_wgpDragon, m_wgpQuad;
	WgpBuffer m_uniformBuffer;
	WgpTexture m_wgpTextureShadow;
};