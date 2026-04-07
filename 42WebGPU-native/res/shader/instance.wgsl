struct VertexInput {
	@builtin(instance_index) instanceIndex : u32,
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
  mvp : array<mat4x4f, 16>
}

@binding(0) @group(0) var<uniform> uniforms : Uniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.mvp[in.instanceIndex] * vec4f(in.position, 1.0);
	out.normal = in.normal;
	out.texcoord = in.texcoord;
	out.color = 0.5 * (vec4f(in.position, 1.0) + vec4(1.0));
	return out;
}


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	return in.color;
}