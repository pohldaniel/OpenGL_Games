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
@group(0) @binding(0) var<uniform> commonUniforms : Uniforms;
@group(0) @binding(1) var<uniform> light : LightUniforms;
@group(0) @binding(2) var<uniform> sphere : SphereUniforms;
@group(0) @binding(3) var tileSampler : sampler;
@group(0) @binding(4) var tileTexture : texture_2d<f32>;
@group(0) @binding(5) var waterSampler : sampler;
@group(0) @binding(6) var waterTexture : texture_2d<f32>;
@group(0) @binding(7) var causticTexture : texture_2d<f32>;
@group(0) @binding(8) var<uniform> shadows : ShadowUniforms;
@group(0) @binding(9) var skySampler : sampler;
@group(0) @binding(10) var skyTexture : texture_cube<f32>;

const IOR_AIR : f32 = 1.0;
const IOR_WATER : f32 = 1.333;
const poolHeight : f32 = 1.0;

struct VertexOutput {
  @builtin(position) position : vec4f,
  @location(0) worldPos : vec3f,
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

fn intersectSphere(origin: vec3f, ray: vec3f, sphereCenter: vec3f, sphereRadius: f32) -> f32 {
  let toSphere = origin - sphereCenter;
  let a = dot(ray, ray);
  let b = 2.0 * dot(toSphere, ray);
  let c = dot(toSphere, toSphere) - sphereRadius * sphereRadius;
  let discriminant = b*b - 4.0*a*c;
  if (discriminant > 0.0) {
    let t = (-b - sqrt(discriminant)) / (2.0 * a);
    if (t > 0.0) { return t; }
  }
  return 1.0e6;
}

fn getSphereColor(point: vec3f) -> vec3f {
  var color = vec3f(0.5);
  let sphereRadius = sphere.radius;
  color *= 1.0 - 0.9 / pow((1.0 + sphereRadius - abs(point.x)) / sphereRadius, 3.0);
  color *= 1.0 - 0.9 / pow((1.0 + sphereRadius - abs(point.z)) / sphereRadius, 3.0);
  color *= 1.0 - 0.9 / pow((point.y + 1.0 + sphereRadius) / sphereRadius, 3.0);
  let sphereNormal = (point - sphere.center) / sphereRadius;
  let refractedLight = refract(-light.direction, vec3f(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
  var diffuse = max(0.0, dot(-refractedLight, sphereNormal)) * 0.5;
  let info = textureSampleLevel(waterTexture, waterSampler, point.xz * 0.5 + 0.5, 0.0);
  if (point.y < info.r) {
    let causticUV = 0.75 * (point.xz - point.y * refractedLight.xz / refractedLight.y) * 0.5 + 0.5;
    let caustic = textureSampleLevel(causticTexture, waterSampler, causticUV, 0.0);
    diffuse *= caustic.r * 4.0;
  }
  color += diffuse;
  return color;
}

fn getWallColor(point: vec3f) -> vec3f {
  var wallColor : vec3f;
  var normal = vec3f(0.0, 1.0, 0.0);
  if (abs(point.x) > 0.999) {
    wallColor = textureSampleLevel(tileTexture, tileSampler, point.yz * 0.5 + vec2f(1.0, 0.5), 0.0).rgb;
    normal = vec3f(-point.x, 0.0, 0.0);
  } else if (abs(point.z) > 0.999) {
    wallColor = textureSampleLevel(tileTexture, tileSampler, point.yx * 0.5 + vec2f(1.0, 0.5), 0.0).rgb;
    normal = vec3f(0.0, 0.0, -point.z);
  } else {
    wallColor = textureSampleLevel(tileTexture, tileSampler, point.xz * 0.5 + 0.5, 0.0).rgb;
  }
  var scale = 0.5;
  scale /= length(point);
  scale *= mix(1.0, 1.0 - 0.9 / pow(length(point - sphere.center) / sphere.radius, 4.0), shadows.sphere);
  let refractedLight = -refract(-light.direction, vec3f(0.0, 1.0, 0.0), IOR_AIR / IOR_WATER);
  var diffuse = max(0.0, dot(refractedLight, normal));
  let info = textureSampleLevel(waterTexture, waterSampler, point.xz * 0.5 + 0.5, 0.0);
  if (point.y < info.r) {
    let causticUV = 0.75 * (point.xz - point.y * refractedLight.xz / refractedLight.y) * 0.5 + 0.5;
    let caustic = textureSampleLevel(causticTexture, waterSampler, causticUV, 0.0);
    scale += diffuse * caustic.r * 2.0 * caustic.g;
  } else {
    let t = intersectCube(point, refractedLight, vec3f(-1.0, -poolHeight, -1.0), vec3f(1.0, 2.0, 1.0));
    diffuse *= 1.0 / (1.0 + exp(-200.0 / (1.0 + 10.0 * (t.y - t.x)) * (point.y + refractedLight.y * t.y - 2.0 / 12.0)));
    scale += diffuse * 0.5;
  }
  return wallColor * scale;
}

fn getSurfaceRayColor(origin: vec3f, ray: vec3f, waterColor: vec3f) -> vec3f {
  var color : vec3f;
  var q = 1.0e6;
  if (shadows.sphere > 0.5) {
    q = intersectSphere(origin, ray, sphere.center, sphere.radius);
  }
  if (q < 1.0e6) {
    color = getSphereColor(origin + ray * q);
  } else if (ray.y < 0.0) {
    let t = intersectCube(origin, ray, vec3f(-1.0, -poolHeight, -1.0), vec3f(1.0, 2.0, 1.0));
    color = getWallColor(origin + ray * t.y);
  } else {
    let t = intersectCube(origin, ray, vec3f(-1.0, -poolHeight, -1.0), vec3f(1.0, 2.0, 1.0));
    let hit = origin + ray * t.y;
    if (hit.y < 2.0 / 12.0) {
      color = getWallColor(hit);
    } else {
      // Sample skybox for rays going upward
      color = textureSampleLevel(skyTexture, skySampler, ray, 0.0).rgb;
      // Add sun specular highlight
      let sunDir = normalize(light.direction);
      let spec = pow(max(0.0, dot(sunDir, ray)), 5000.0);
      color += vec3f(spec) * vec3f(10.0, 8.0, 6.0);
    }
  }
  if (ray.y < 0.0) {
    color *= waterColor;
  }
  return color;
}

const aboveWaterColor : vec3f = vec3f(0.25, 1.0, 1.25);

@vertex
fn vs_main(@location(0) position : vec3f) -> VertexOutput {
  var out : VertexOutput;
  let uv = position.xy * 0.5 + 0.5;
  let info = textureSampleLevel(waterTexture, waterSampler, uv, 0.0);
  var pos = position.xzy;
  pos.y = info.r;
  out.worldPos = pos;
  out.position = commonUniforms.projection * commonUniforms.view * commonUniforms.model * vec4f(pos, 1.0);
  return out;
}

@fragment
fn fs_main(@location(0) worldPos : vec3f) -> @location(0) vec4f {
  var uv = worldPos.xz * 0.5 + 0.5;
  var info = textureSampleLevel(waterTexture, waterSampler, uv, 0.0);
  for (var i = 0; i < 5; i++) {
    uv += info.ba * 0.005;
    info = textureSampleLevel(waterTexture, waterSampler, uv, 0.0);
  }
  let ba = vec2f(info.b, info.a);
  var normal = vec3f(info.b, sqrt(max(0.0, 1.0 - dot(ba, ba))), info.a);
  let incomingRay = normalize(worldPos - commonUniforms.camPos);
  let reflectedRay = reflect(incomingRay, normal);
  let refractedRay = refract(incomingRay, normal, IOR_AIR / IOR_WATER);
  let fresnel = mix(0.25, 1.0, pow(1.0 - dot(normal, -incomingRay), 3.0));
  let reflectedColor = getSurfaceRayColor(worldPos, reflectedRay, aboveWaterColor);
  let refractedColor = getSurfaceRayColor(worldPos, refractedRay, aboveWaterColor);
  let finalColor = mix(refractedColor, reflectedColor, fresnel);
  return vec4f(finalColor, 1.0);
}