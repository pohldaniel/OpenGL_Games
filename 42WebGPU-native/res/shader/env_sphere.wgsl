struct VertexInput {
	@location(0) position: vec3f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec3f
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
@group(0) @binding(1) var smplr: sampler;
@group(1) @binding(0) var texture: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.env * vec4f(in.position, 1.0);
	out.position.z = out.position.w - 0.001;
	out.texcoord = in.position;
	return out;
}

const inverseAtan = vec2f(0.1591, 0.3183);

fn sampleSphericalMap(v: vec3f) -> vec2f {
	var uv = vec2f(atan2(v.z, v.x), asin(v.y));
	uv *= inverseAtan;
	uv += 0.5;
	return uv;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {	
	return textureSample(texture, smplr, sampleSphericalMap(in.texcoord));
}