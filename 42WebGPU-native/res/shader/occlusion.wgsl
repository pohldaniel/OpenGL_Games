struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) color: vec4f,
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

struct UniformsScene {
	model: mat4x4f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) normal: vec3f,
  @location(1) color: vec4f,
};

@group(0) @binding(0) var<uniform> uniforms : Uniforms;
@group(0) @binding(1) var<uniform> uniformsScene: UniformsScene;

@vertex 
fn vs_main(in: VertexInput) -> VertexOutput {
  var out : VertexOutput;
  out.position = uniforms.projection * uniforms.view * uniformsScene.model * vec4f(in.position, 1.0);
  out.normal = (uniformsScene.model * vec4f(in.normal, 0)).xyz;
  out.color = in.color;
  return out;
}

@fragment 
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  let lightDirection = normalize(vec3f(4, 10, 6));
  let light = dot(normalize(in.normal), lightDirection) * 0.5 + 0.5;
  return vec4f(in.color.rgb * light, in.color.a);
}