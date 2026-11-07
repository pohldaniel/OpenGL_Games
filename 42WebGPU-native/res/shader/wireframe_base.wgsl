struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f
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

struct Instance {
    model : mat4x4<f32>,
	color: vec4<f32>
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> instance : Instance;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * instance.model * vec4f(in.position, 1.0);
	out.color = instance.color;
	out.normal = in.normal;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    let lightDirection = normalize(vec3f(4.0, 10.0, 6.0));
    let light = dot(normalize(in.normal), lightDirection) * 0.5 + 0.5;
    return vec4f(in.color.rgb * light, in.color.a);
}