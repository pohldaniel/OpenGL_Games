struct UpdateUniforms {
  delta : vec2f,
}
@group(0) @binding(0) var waterTexture : texture_2d<f32>;
@group(0) @binding(1) var waterSampler : sampler;
@group(0) @binding(2) var<uniform> u : UpdateUniforms;

struct VertexOutput {
  @builtin(position) position : vec4f,
  @location(0) coord : vec2f,
}

@vertex
fn vs_main(@builtin(vertex_index) idx : u32) -> VertexOutput {
  var pos = array<vec2f, 6>(
    vec2f(-1.0, -1.0), vec2f(1.0, -1.0), vec2f(-1.0, 1.0),
    vec2f(-1.0, 1.0), vec2f(1.0, -1.0), vec2f(1.0, 1.0)
  );
  var out : VertexOutput;
  out.position = vec4f(pos[idx], 0.0, 1.0);
  out.coord = (pos[idx] + 1.0) * 0.5;
  return out;
}

@fragment
fn fs_main(@location(0) coord : vec2f) -> @location(0) vec4f {
  var info = textureSample(waterTexture, waterSampler, coord);
  let dx = vec2f(u.delta.x, 0.0);
  let dy = vec2f(0.0, u.delta.y);
  let avg = (
    textureSample(waterTexture, waterSampler, coord - dx).r +
    textureSample(waterTexture, waterSampler, coord - dy).r +
    textureSample(waterTexture, waterSampler, coord + dx).r +
    textureSample(waterTexture, waterSampler, coord + dy).r
  ) * 0.25;
  info.g += (avg - info.r) * 2.0;
  info.g *= 0.995;
  info.r += info.g;
  return info;
}