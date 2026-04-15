struct VertexInput {
	@location(0) position: vec3f,
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) local_pos: vec3f
};

@group(0) @binding(0) var<uniform> view_projection : mat4x4<f32>;
@group(0) @binding(1) var smplr: sampler;
@group(0) @binding(2) var texture: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var output : VertexOutput;
  output.position = view_projection * vec4f(in.position, 1.0);
  output.local_pos = in.position;
  return output;
}

const INV_ATAN : vec2<f32> = vec2<f32>(0.1591, 0.3183);

fn sample_spherical_map(v : vec3<f32>) -> vec2<f32> {
  var uv : vec2<f32> = vec2<f32>(atan2(v.z, v.x), asin(v.y));
  uv = uv * INV_ATAN + 0.5;
  return uv;
}

@fragment
fn fs_main(input : VertexOutput) -> @location(0) vec4<f32> {
  let uv = sample_spherical_map(normalize(input.local_pos));
  return textureSample(texture, smplr, uv);
}