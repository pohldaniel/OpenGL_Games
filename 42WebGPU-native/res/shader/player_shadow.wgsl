struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) weight: vec4f,
	@location(4) joint: vec4u
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

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> skin: array<mat4x4f>;

fn get_world_matrix(weight : vec4f, joint : vec4u) -> mat4x4f {
	return skin[joint.x] * weight.x + skin[joint.y] * weight.y +
           skin[joint.z] * weight.z + skin[joint.w] * weight.w;
}

@vertex
fn vs_main(in: VertexInput) -> @builtin(position) vec4f  {	
	let world = get_world_matrix(in.weight, in.joint);  	
	return uniforms.lightVP * world * vec4f(in.position, 1.0);	
}