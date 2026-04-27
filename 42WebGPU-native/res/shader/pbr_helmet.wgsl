struct VertexInput {
	@location(0) position: vec3f,
	@location(1) uv: vec2f,
	@location(2) normal: vec3f,
	@location(3) tangent: vec3f,
	@location(4) bitangent: vec3f
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) normal: vec3f,
	@location(1) uv: vec2f,
	@location(2) worldPosition: vec3f,
	@location(3) shadowPosition: vec3f,
	@location(4) tangent: vec4f
}

struct Scene {
  cameraProjection: mat4x4f,
  cameraView: mat4x4f,
  cameraPosition: vec3f,
  lightPosition: vec3f,
  lightColor: vec3f,
  lightViewProjection: mat4x4f,
};

struct Material {
  baseColorFactor: vec4f,
  alphaCutoff: f32,
};

struct Uniforms {
    projection: mat4x4f,
	view: mat4x4f,
	env: mat4x4f,
    model:mat4x4f,    
	normal: mat4x4f,
	color:vec4f,
	camPos: vec3f,
	lightVPS: mat4x4f,
	shadow: mat4x4f
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> scene: Scene;
@group(1) @binding(0) var<storage> models: array<mat4x4f>;

// Material
@group(2) @binding(0) var<uniform> material: Material;
@group(2) @binding(1) var albedoSampler: sampler;
@group(2) @binding(2) var albedoTexture: texture_2d<f32>;
@group(2) @binding(3) var normalSampler: sampler;
@group(2) @binding(4) var normalTexture: texture_2d<f32>;
@group(2) @binding(5) var roughnessMetallicSampler: sampler;
@group(2) @binding(6) var roughnessMetallicTexture: texture_2d<f32>;
@group(2) @binding(7) var aoSampler: sampler;
@group(2) @binding(8) var aoTexture: texture_2d<f32>;
@group(2) @binding(9) var emissiveSampler: sampler;
@group(2) @binding(10) var emissiveTexture: texture_2d<f32>;

// PBR textures
@group(3) @binding(0) var samplerBRDF: sampler;
@group(3) @binding(1) var samplerGeneral: sampler;
@group(3) @binding(2) var brdfLUT: texture_2d<f32>;
@group(3) @binding(3) var irradianceMap: texture_cube<f32>;
@group(3) @binding(4) var prefilterMap: texture_cube<f32>;
@group(3) @binding(5) var shadowMap: texture_depth_2d;
@group(3) @binding(6) var shadowSampler: sampler_comparison;

@vertex
fn vs_main(){

}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	return vec4(1.0, 0.0, 0.0, 1.0);
}