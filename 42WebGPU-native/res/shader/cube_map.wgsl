struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f
};


struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) color: vec4f,
	@location(4) texcoord_env: vec3f
};

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var textureSampler: sampler;
@group(1) @binding(0) var texture: texture_cube<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * uniforms.model * vec4f(in.position, 1.0);
	out.texcoord_env = in.position;
	return out;
}


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	let sample = textureSample(texture, textureSampler, in.texcoord_env);
	let mappedColor = sample.rgb / (sample.rgb + vec3f(1.0));
	
	return vec4(sample.rgb, 1.0);
}