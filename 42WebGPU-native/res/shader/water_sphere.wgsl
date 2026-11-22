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
@group(0) @binding(0) var<uniform> camera : Uniforms;
@group(0) @binding(1) var<uniform> light : LightUniforms;
@group(0) @binding(2) var<uniform> sphere : SphereUniforms;
@group(0) @binding(3) var waterSampler : sampler;
@group(0) @binding(4) var waterTexture : texture_2d<f32>;
@group(0) @binding(5) var causticTexture : texture_2d<f32>;

const IOR_AIR : f32 = 1.0;
const IOR_WATER : f32 = 1.333;
const underwaterColor : vec3f = vec3f(0.4, 0.9, 1.0);

struct VertexOutput {
  @builtin(position) position : vec4f,
  @location(0) localPos : vec3f,
  @location(1) worldPos : vec3f,
}

@vertex
fn vs_main(@location(0) position : vec3f) -> VertexOutput {
  var out : VertexOutput;
  let worldPos = sphere.center + position * sphere.radius;
  out.position = camera.projection * camera.view * camera.model * vec4f(worldPos, 1.0);
  out.localPos = position;
  out.worldPos = worldPos;
  return out;
}

@fragment
fn fs_main(@location(0) localPos : vec3f, @location(1) worldPos : vec3f) -> @location(0) vec4f {
  var color = vec3f(0.5);

  // Distance-based darkening near pool walls
  color *= 1.0 - 0.9 / pow((1.0 + sphere.radius - abs(worldPos.x)) / sphere.radius, 3.0);
  color *= 1.0 - 0.9 / pow((1.0 + sphere.radius - abs(worldPos.z)) / sphere.radius, 3.0);
  color *= 1.0 - 0.9 / pow((worldPos.y + 1.0 + sphere.radius) / sphere.radius, 3.0);

  // Diffuse lighting with caustics
  let sphereNormal = normalize(localPos);
  let refractedLight = refract(-light.direction, vec3f(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
  var diffuse = max(0.0, dot(-refractedLight, sphereNormal)) * 0.5;

  let info = textureSampleLevel(waterTexture, waterSampler, worldPos.xz * 0.5 + 0.5, 0.0);
  if (worldPos.y < info.r) {
    // Underwater: apply caustics
    let causticUV = 0.75 * (worldPos.xz - worldPos.y * refractedLight.xz / refractedLight.y) * 0.5 + 0.5;
    let caustic = textureSampleLevel(causticTexture, waterSampler, causticUV, 0.0);
    diffuse *= caustic.r * 4.0;
  }
  color += diffuse;

  // Apply underwater tint
  if (worldPos.y < info.r) {
    color *= underwaterColor;
  }

  return vec4f(color, 1.0);
}