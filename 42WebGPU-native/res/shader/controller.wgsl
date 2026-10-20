struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec2f,
	@location(2) color: vec4f
};

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>,
	lightVP:mat4x4<f32>,
	shadow: mat4x4<f32>,
	lightPos: vec3<f32>
};

struct Instance {
    model : mat4x4<f32>,
    color : vec4<f32>,
}

struct Instances {
    instances : array<Instance>,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> strg : Instances;

@vertex
fn vs_main(in: VertexInput, @builtin(instance_index) instanceIndex : u32) -> VertexOutput {
	let instance = strg.instances[instanceIndex];
	
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * instance.model * vec4f(in.position, 1.0);
	out.texcoord = in.texcoord;
	out.color = instance.color;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	return in.color;
}