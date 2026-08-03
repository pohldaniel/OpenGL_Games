struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f
};


struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) color: vec4f
};

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>
};

struct Wiggly {
	nosePos: vec3f,
	time: f32
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> wiggly: Wiggly;
@group(0) @binding(2) var smplr: sampler;
@group(0) @binding(3) var texture: texture_2d<f32>;

const wiggleMagnitude: f32 = 0.03;
const wiggleDistModifier: f32 = 0.12;
const wiggleTimeModifier: f32 = 9.4;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	
	let xOffset = sin(wiggleTimeModifier * wiggly.time + wiggleDistModifier * distance(wiggly.nosePos, in.position * 100.0)) * wiggleMagnitude;
	
	out.position = uniforms.projection * uniforms.view * uniforms.model * vec4(in.position.x + xOffset, in.position.y, in.position.z, 1.0);
	out.normal = in.normal;
	out.texcoord = in.texcoord;
	out.color = uniforms.color;
	return out;
}


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	return textureSample(texture, smplr, in.texcoord);
}