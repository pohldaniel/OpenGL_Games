struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) normal: vec3f,
	@location(3) color: vec4f
};

struct UniformsScene {
  world: mat4x4f,
  color: vec4f
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

@group(0) @binding(0) var<uniform> uniformsScene: UniformsScene;
@group(0) @binding(1) var<uniform> uniforms: Uniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * uniformsScene.world * vec4f(in.position, 1.0);
	out.color = uniformsScene.color;
	out.normal = (uniformsScene.world * vec4f(in.normal, 0.0)).xyz;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	let diffuseColor = uniformsScene.color;
	let a_normal = normalize(in.normal);
	let l = dot(a_normal, uniforms.lightPos) * 0.5 + 0.5;
	return vec4f(diffuseColor.rgb * l, diffuseColor.a);
}