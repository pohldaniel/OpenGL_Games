struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) weight: vec4f,
	@location(4) joint: vec4u
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) color: vec4f,
	@location(4) weight: vec4f,
	@location(5) joint: vec4f
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

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> skin: array<mat4x4f>;

fn get_world_matrix(weight : vec4f, joint : vec4u) -> mat4x4f {
	return skin[joint.x] * weight.x + skin[joint.y] * weight.y +
           skin[joint.z] * weight.z + skin[joint.w] * weight.w;
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	let world = get_world_matrix(in.weight, in.joint);  
	
	out.position = uniforms.projection * uniforms.view * world * vec4f(in.position, 1.0);
	out.normal = in.normal;
	out.texcoord = in.texcoord;
	out.color = uniforms.color;
	
	out.weight = in.weight;
	out.joint = vec4f(f32(in.joint[0]), f32(in.joint[1]), f32(in.joint[2]), f32(in.joint[3]));
	
	return out;
}


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	return vec4f(in.normal, 1.0);
}