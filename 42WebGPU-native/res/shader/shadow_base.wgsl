struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) shadowPos: vec4f,
	@location(1) fragPos: vec3f,
	@location(2) fragNorm: vec3f,
};

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>,
	lightVP: mat4x4<f32>,
	shadow: mat4x4<f32>,
	lightPos: vec3<f32>
};

const shadowDepthTextureSize: f32 = 1024.0;
const albedo = vec3f(0.9);
const ambientFactor = 0.2;
  
@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var shadowMap: texture_depth_2d;
@group(0) @binding(2) var shadowSampler: sampler_comparison;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * uniforms.model * vec4f(in.position, 1.0);
	out.shadowPos = uniforms.shadow * uniforms.model * vec4f(in.position, 1.0);
	out.fragPos = out.position.xyz;
	out.fragNorm = in.normal;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {	
var visibility = 0.0;
	let oneOverShadowDepthTextureSize = 1.0 / shadowDepthTextureSize;
	for (var y = -1; y <= 1; y++) {
	  for (var x = -1; x <= 1; x++) {
		let offset = vec2f(vec2(x, y)) * oneOverShadowDepthTextureSize;
    
		visibility += textureSampleCompare(
		  shadowMap, shadowSampler,
		  in.shadowPos.xy + offset, in.shadowPos.z - 0.007
		);
	  }
	}
	visibility /= 9.0;
	
	let lambertFactor = max(dot(normalize(uniforms.lightPos - in.fragPos), normalize(in.fragNorm)), 0.0);
	let lightingFactor = min(ambientFactor + visibility * lambertFactor, 1.0);

	return vec4(lightingFactor * albedo, 1.0);
}