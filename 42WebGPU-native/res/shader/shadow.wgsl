struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f
};

struct Uniforms {
    projection: mat4x4f,
	view: mat4x4f,
	env: mat4x4f,
    model:mat4x4f,    
	normal: mat4x4f,
	color: vec4f,
	camPos: vec3f,
	lightVP: mat4x4f,
	shadow: mat4x4f,
	lightPos: vec3f
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex
fn vs_main(in: VertexInput) -> @builtin(position) vec4f {	
	return uniforms.lightVP * uniforms.model * vec4(in.position, 1.0);
}