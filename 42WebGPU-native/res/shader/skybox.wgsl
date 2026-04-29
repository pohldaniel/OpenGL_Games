struct VertexInput {
	@location(0) position: vec3f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) fragmentPosition: vec4f
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
@group(0) @binding(2) var texture: texture_cube<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out: VertexOutput;

  var copy = uniforms.view;
  copy[3][0] = 0.0;
  copy[3][1] = 0.0;
  copy[3][2] = 0.0;

  out.position = (uniforms.projection * uniforms.env * vec4(in.position, 1.0));
  out.position.z = out.position.w;
  out.fragmentPosition = 0.5 * (vec4(in.position, 1.0) + vec4(1.0, 1.0, 1.0, 1.0));
  return out;
}

fn toneMapping(color: vec3f) -> vec3f {
  let a      = vec3f(1.6);
  let d      = vec3f(0.977);
  let hdrMax = vec3f(8.0);
  let midIn  = vec3f(0.18);
  let midOut = vec3f(0.267);

  let b = (-pow(midIn, a) + pow(hdrMax, a) * midOut)
          / ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
  let c = (pow(hdrMax, a * d) * pow(midIn, a)
           - pow(hdrMax, a) * pow(midIn, a * d) * midOut)
          / ((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

  return pow(color, a) / (pow(color, a * d) * b + c);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  var cubemapVec = in.fragmentPosition.xyz - vec3(0.5);
  var color = textureSample(texture, smplr, cubemapVec).rgb;
  color = toneMapping(color);
  color = pow(color, vec3f(1.0 / 2.2));
  return vec4f(color, 1);
}