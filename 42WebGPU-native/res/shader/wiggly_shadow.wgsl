struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f
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

struct Wiggly {
	nosePos: vec3f,
	time: f32
};


struct InstanceData {
    instances : array<mat4x4<f32>>,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> wiggly: Wiggly;
@group(0) @binding(2) var<storage, read> instanceStorage : InstanceData;

const wiggleMagnitude: f32 = 0.03;
const wiggleDistModifier: f32 = 0.12;
const wiggleTimeModifier: f32 = 9.4;

@vertex
fn vs_main(in : VertexInput, @builtin(instance_index) instanceIndex : u32) -> @builtin(position) vec4f  {
	let model = instanceStorage.instances[instanceIndex];
	let xOffset = sin(wiggleTimeModifier * wiggly.time + wiggleDistModifier * distance(wiggly.nosePos, in.position * 100.0)) * wiggleMagnitude;
	
	return uniforms.lightVP * model * vec4(in.position.x + xOffset, in.position.y, in.position.z, 1.0);
}