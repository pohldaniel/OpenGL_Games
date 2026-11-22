struct SphereMoveUniforms {
  oldCenter : vec3f,
  radius : f32,
  newCenter : vec3f,
  _pad : f32,
}
@group(0) @binding(0) var waterTexture : texture_2d<f32>;
@group(0) @binding(1) var waterSampler : sampler;
@group(0) @binding(2) var<uniform> u : SphereMoveUniforms;

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
  out.coord = vec2f((pos[idx].x + 1.0) * 0.5, (1.0 - pos[idx].y) * 0.5);
  return out;
}

// Calculates the volume of sphere intersecting the water at a UV position
fn volumeInSphere(center : vec3f, uv : vec2f, radius : f32) -> f32 {
  let p = vec3f(uv.x * 2.0 - 1.0, 0.0, uv.y * 2.0 - 1.0);
  let dist = length(p - center);
  let t = dist / radius;
  // Gaussian-like falloff for smooth interaction
  let dy = exp(-pow(t * 1.5, 6.0));
  let ymin = min(0.0, center.y - dy);
  let ymax = min(max(0.0, center.y + dy), ymin + 2.0 * dy);
  return (ymax - ymin) * 0.1;
}

@fragment
fn fs_main(@location(0) coord : vec2f) -> @location(0) vec4f {
  var info = textureSample(waterTexture, waterSampler, coord);
  // Water rises where sphere was, falls where sphere is now
  info.r += volumeInSphere(u.oldCenter, coord, u.radius);
  info.r -= volumeInSphere(u.newCenter, coord, u.radius);
  return info;
}