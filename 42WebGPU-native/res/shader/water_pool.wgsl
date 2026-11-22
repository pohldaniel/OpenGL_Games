struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>,
	lightVP:mat4x4<f32>,
	shadow: mat4x4<f32>,
	lightPos: vec3<f32>
};
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
@group(0) @binding(0) var<uniform> uniforms : Uniforms;
@group(0) @binding(1) var tileSampler : sampler;
@group(0) @binding(2) var tileTexture : texture_2d<f32>;
@group(0) @binding(3) var<uniform> light : LightUniforms;
@group(0) @binding(4) var<uniform> sphere : SphereUniforms;
@group(0) @binding(5) var waterSampler : sampler;
@group(0) @binding(6) var waterTexture : texture_2d<f32>;
@group(0) @binding(7) var causticTexture : texture_2d<f32>;
@group(0) @binding(8) var<uniform> shadows : ShadowUniforms;

const IOR_AIR : f32 = 1.0;
const IOR_WATER : f32 = 1.333;
const poolHeight : f32 = 1.0;

struct VertexOutput {
  @builtin(position) position : vec4f,
  @location(0) localPos : vec3f,
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

@vertex
fn vs_main(@location(0) position : vec3f) -> VertexOutput {
  var out : VertexOutput;
  // Transform Y coordinate to create pool depth
  var transformedPos = position;
  transformedPos.y = ((1.0 - position.y) * (7.0 / 12.0) - 1.0);
  out.position = uniforms.projection * uniforms.view * uniforms.model * vec4f(transformedPos, 1.0);
  out.localPos = transformedPos;
  return out;
}

@fragment
fn fs_main(@location(0) localPos : vec3f) -> @location(0) vec4f {
  var wallColor : vec3f;
  let point = localPos;

  // Sample tile texture based on which face we're rendering
  if (abs(point.x) > 0.999) {
    wallColor = textureSampleLevel(tileTexture, tileSampler, point.yz * 0.5 + vec2f(1.0, 0.5), 0.0).rgb;
  } else if (abs(point.z) > 0.999) {
    wallColor = textureSampleLevel(tileTexture, tileSampler, point.yx * 0.5 + vec2f(1.0, 0.5), 0.0).rgb;
  } else {
    wallColor = textureSampleLevel(tileTexture, tileSampler, point.xz * 0.5 + 0.5, 0.0).rgb;
  }

  // Determine surface normal based on face
  var normal = vec3f(0.0, 1.0, 0.0);
  if (abs(point.x) > 0.999) { normal = vec3f(-point.x, 0.0, 0.0); }
  else if (abs(point.z) > 0.999) { normal = vec3f(0.0, 0.0, -point.z); }

  // Ambient occlusion
  var scale = 0.5;
  scale /= length(point);
  scale *= mix(1.0, 1.0 - 0.9 / pow(length(point - sphere.center) / sphere.radius, 4.0), shadows.sphere);

  // Lighting with caustics or rim shadow
  let refractedLight = -refract(-light.direction, vec3f(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
  var diffuse = max(0.0, dot(refractedLight, normal));

  let info = textureSampleLevel(waterTexture, waterSampler, point.xz * 0.5 + 0.5, 0.0);
  if (point.y < info.r) {
    // Underwater: sample caustics
    let causticUV = 0.75 * (point.xz - point.y * refractedLight.xz / refractedLight.y) * 0.5 + 0.5;
    let caustic = textureSampleLevel(causticTexture, waterSampler, causticUV, 0.0);
    scale += diffuse * caustic.r * 2.0 * caustic.g;
  } else {
    // Above water: apply rim shadow
    let t = intersectCube(point, refractedLight, vec3f(-1.0, -poolHeight, -1.0), vec3f(1.0, 2.0, 1.0));
    diffuse *= 1.0 / (1.0 + exp(-200.0 / (1.0 + 10.0 * (t.y - t.x)) * (point.y + refractedLight.y * t.y - 2.0 / 12.0)));
    scale += diffuse * 0.5;
  }

  var finalColor = wallColor * scale;

  // Apply underwater color tint (matches TypeScript version)
  if (point.y < info.r) {
    let underwaterColor = vec3f(0.4, 0.9, 1.0);
    finalColor *= underwaterColor * 1.2;
  }

  return vec4f(finalColor, 1.0);
}