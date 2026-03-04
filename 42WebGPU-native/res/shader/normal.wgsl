struct VertexInput {
	@location(0) position: vec3f,
	@location(1) uv: vec2f,
	@location(2) normal: vec3f,
	@location(3) tangent: vec3f,
	@location(4) bitangent: vec3f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
	@location(1) normal: vec3f,	
	@location(2) color: vec4f,
	@location(3) viewDirection: vec3<f32>
};

/**
 * A structure holding the value of our uniforms
 */
struct Uniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
    color: vec4f,
	camPosition: vec3f,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var textureSampler: sampler;
@group(0) @binding(2) var albedoTexture: texture_2d<f32>;
@group(0) @binding(3) var normalTexture: texture_2d<f32>;
@group(0) @binding(4) var heightTexture: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	let worldPosition = uniforms.modelMatrix * vec4<f32>(in.position, 1.0);
	out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPosition;
	out.uv = in.uv;
    out.normal = (uniforms.modelMatrix * vec4f(in.normal, 0.0)).xyz;
	out.color = uniforms.color;
	
	out.viewDirection = uniforms.camPosition - worldPosition.xyz;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	return uniforms.color;
}
