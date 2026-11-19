struct DropUniforms {
  center : vec2f,
  radius : f32,
  strength : f32,
}
@group(0) @binding(0) var waterTexture : texture_2d<f32>;
@group(0) @binding(1) var waterSampler : sampler;
@group(0) @binding(2) var<uniform> u : DropUniforms;

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
  let drop = max(0.0, 1.0 - length((u.center * 0.5 + 0.5) - coord) / u.radius);
  let dropVal = 0.5 - cos(drop * 3.14159265) * 0.5;
  info.r += dropVal * u.strength;
  return info;
}