struct VertexInput {
  @location(0) position : vec3f,
  @location(1) uv : vec2f,
  @location(2) normal : vec3f
}

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

@group(0) @binding(0) var<uniform> uniforms : Uniforms;
@group(1) @binding(0) var<uniform> model : mat4x4f;


struct VertexOutput {
  @builtin(position) position : vec4f,
  @location(0) uv : vec2f,
  @location(1) normal: vec3f
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out : VertexOutput;
  out.position = uniforms.projection * uniforms.view * model * vec4(in.position, 1.0);
  out.normal = normalize((model * vec4(in.normal, 0)).xyz);
  out.uv = in.uv;
  return out;
}

@group(1) @binding(1) var meshSampler: sampler;
@group(1) @binding(2) var meshTexture: texture_2d<f32>;

// Static directional lighting
const lightDir = vec3f(1, 1, 1);
const dirColor = vec3(1);
const ambientColor = vec3f(0.05);

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  let textureColor = textureSample(meshTexture, meshSampler, in.uv);

  // Very simplified lighting algorithm.
  let lightColor = saturate(ambientColor + max(dot(in.normal, lightDir), 0.0) * dirColor);

   return vec4f(textureColor.rgb * lightColor, textureColor.a);
}