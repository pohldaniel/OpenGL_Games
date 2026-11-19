struct LightUniforms {
  direction : vec3f,
  _pad : f32,
}
struct SphereUniforms {
  center : vec3f,
  radius : f32,
}
struct ShadowUniforms {
  rim : f32,
  sphere : f32,
  ao : f32,
  _pad : f32,
}
@group(0) @binding(0) var<uniform> light : LightUniforms;
@group(0) @binding(1) var<uniform> sphere : SphereUniforms;
@group(0) @binding(2) var waterTexture : texture_2d<f32>;
@group(0) @binding(3) var waterSampler : sampler;
@group(0) @binding(4) var<uniform> shadows : ShadowUniforms;

const IOR_AIR : f32 = 1.0;
const IOR_WATER : f32 = 1.333;
const poolHeight : f32 = 1.0;

struct VertexOutput {
  @builtin(position) position : vec4f,
  @location(0) oldPos : vec3f,
  @location(1) newPos : vec3f,
  @location(2) ray : vec3f,
}

fn intersectCube(origin: vec3f, ray: vec3f, cubeMin: vec3f, cubeMax: vec3f) -> vec2f {
  let tMin = (cubeMin - origin) / ray;
  let tMax = (cubeMax - origin) / ray;
  let t1 = min(tMin, tMax);
  let t2 = max(tMin, tMax);
  let tNear = max(max(t1.x, t1.y), t1.z);
  let tFar = min(min(t2.x, t2.y), t2.z);
  return vec2f(tNear, tFar);
}

fn project(origin: vec3f, ray: vec3f, refractedLight: vec3f) -> vec3f {
  var point = origin;
  let tcube = intersectCube(origin, ray, vec3f(-1.0, -poolHeight, -1.0), vec3f(1.0, 2.0, 1.0));
  point += ray * tcube.y;
  let tplane = (-point.y - 1.0) / refractedLight.y;
  return point + refractedLight * tplane;
}

@vertex
fn vs_main(@location(0) position : vec3f) -> VertexOutput {
  var out : VertexOutput;
  let uv = position.xy * 0.5 + 0.5;
  let info = textureSampleLevel(waterTexture, waterSampler, uv, 0.0);
  // Reconstruct normal (scaled down for stability)
  let ba = info.ba * 0.5;
  let normal = vec3f(ba.x, sqrt(max(0.0, 1.0 - dot(ba, ba))), ba.y);
  // Calculate refracted light directions
  let lightDir = normalize(light.direction);
  // Flat water refraction (reference)
  let refractedLight = refract(-lightDir, vec3f(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
  // Displaced water refraction (actual)
  let ray = refract(-lightDir, normal, IOR_AIR / IOR_WATER);
  // Water surface position
  let pos = vec3f(position.x, 0.0, position.y);
  // Project both rays to pool floor
  out.oldPos = project(pos, refractedLight, refractedLight);
  out.newPos = project(pos + vec3f(0.0, info.r, 0.0), ray, refractedLight);
  out.ray = ray;
  // Position in caustics texture space
  let projectedPos = 0.75 * (out.newPos.xz - out.newPos.y * refractedLight.xz / refractedLight.y);
  out.position = vec4f(projectedPos.x, -projectedPos.y, 0.0, 1.0);
  return out;
}

@fragment
fn fs_main(@location(0) oldPos : vec3f, @location(1) newPos : vec3f, @location(2) ray : vec3f) -> @location(0) vec4f {
  // Calculate intensity from area ratio using screen-space derivatives
  let oldArea = length(dpdx(oldPos)) * length(dpdy(oldPos));
  let newArea = length(dpdx(newPos)) * length(dpdy(newPos));
  var intensity = oldArea / newArea * 0.2;

  // Calculate sphere shadow
  let lightDir = normalize(light.direction);
  let refractedLight = refract(-lightDir, vec3f(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
  // Sphere shadow using distance to ray
  let dir = (sphere.center - newPos) / sphere.radius;
  let area = cross(dir, refractedLight);
  var shadow = dot(area, area);
  let dist = dot(dir, -refractedLight);
  shadow = 1.0 + (shadow - 1.0) / (0.05 + dist * 0.025);
  shadow = clamp(1.0 / (1.0 + exp(-shadow)), 0.0, 1.0);
  shadow = mix(1.0, shadow, clamp(dist * 2.0, 0.0, 1.0));
  shadow = mix(1.0, shadow, shadows.sphere);

  // Rim shadow at pool edges
  let t = intersectCube(newPos, -refractedLight, vec3f(-1.0, -poolHeight, -1.0), vec3f(1.0, 2.0, 1.0));
  let rimShadow = 1.0 / (1.0 + exp(-200.0 / (1.0 + 10.0 * (t.y - t.x)) * (newPos.y - refractedLight.y * t.y - 2.0 / 12.0)));
  intensity *= mix(1.0, rimShadow, shadows.rim);

  // R = caustic intensity, G = sphere shadow factor
  return vec4f(intensity, shadow, 0.0, 1.0);
}