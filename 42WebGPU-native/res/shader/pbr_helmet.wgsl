struct VertexInput {
	@location(0) position: vec3f,
	@location(1) uv: vec2f,
	@location(2) normal: vec3f,
	@location(3) tangent: vec3f,
	@location(4) bitangent: vec3f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) normal: vec3f,
	@location(1) uv: vec2f,
	@location(2) worldPosition: vec3f,
	@location(3) shadowCoord: vec4f,
	@location(4) tangent: vec3f
};

struct Material {
  baseColorFactor: vec4f,
  alphaCutoff: f32,
};

struct Uniforms {
    projection: mat4x4f,
	view: mat4x4f,
	env: mat4x4f,
    model:mat4x4f,    
	normal: mat4x4f,
	color: vec4f,
	camPos: vec3f,
	lightVP: mat4x4f,
	shadow: mat4x4f,
	lightPos: vec3f
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(1) @binding(0) var<storage> models: array<mat4x4f>;

// Material
@group(2) @binding(0) var<uniform> material: Material;
@group(2) @binding(1) var albedoSampler: sampler;
@group(2) @binding(2) var albedoTexture: texture_2d<f32>;
@group(2) @binding(3) var normalSampler: sampler;
@group(2) @binding(4) var normalTexture: texture_2d<f32>;
@group(2) @binding(5) var roughnessMetallicSampler: sampler;
@group(2) @binding(6) var roughnessMetallicTexture: texture_2d<f32>;
@group(2) @binding(7) var aoSampler: sampler;
@group(2) @binding(8) var aoTexture: texture_2d<f32>;
@group(2) @binding(9) var emissiveSampler: sampler;
@group(2) @binding(10) var emissiveTexture: texture_2d<f32>;

// PBR textures
@group(3) @binding(0) var samplerBRDF: sampler;
@group(3) @binding(1) var samplerGeneral: sampler;
@group(3) @binding(2) var brdfLUT: texture_2d<f32>;
@group(3) @binding(3) var irradianceMap: texture_cube<f32>;
@group(3) @binding(4) var prefilterMap: texture_cube<f32>;
@group(3) @binding(5) var shadowMap: texture_depth_2d;
@group(3) @binding(6) var shadowSampler: sampler_comparison;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput{
	var out: VertexOutput;
	let worldPosition = uniforms.model * vec4f(in.position, 1.0);
	out.position = uniforms.projection * uniforms.view * worldPosition;
	out.uv = in.uv;
	out.normal = (uniforms.model * vec4f(in.normal, 0.0)).xyz;
	out.tangent = (uniforms.model * vec4f(in.tangent, 0.0)).xyz;
	out.shadowCoord = uniforms.shadow * worldPosition;	
	out.worldPosition = worldPosition.xyz;
	return out;
}

fn distributionGGX(n : vec3f, h : vec3f, roughness : f32)->f32 {
	let a      = roughness * roughness;
	let a2     = a * a;
	let nDotH  = max(dot(n, h), 0.0);
	let nDotH2 = nDotH * nDotH;
	var denom  = (nDotH2 * (a2 - 1.0) + 1.0);
	denom      = PI * denom * denom;
	return a2 / denom;
}
	
fn geometrySchlickGGX(nDotV : f32, roughness : f32)->f32 {
	let r = (roughness + 1.0);
	let k = (r * r) / 8.0;
	return nDotV / (nDotV * (1.0 - k) + k);
}
	
fn geometrySmith(n : vec3f, v : vec3f, l : vec3f, roughness : f32)->f32 {
	let nDotV = max(dot(n, v), 0.0);
	let nDotL = max(dot(n, l), 0.0);
	let ggx2  = geometrySchlickGGX(nDotV, roughness);
	let ggx1  = geometrySchlickGGX(nDotL, roughness);
	return ggx1 * ggx2;
}
fn fresnelSchlick(cosTheta : f32, f0 : vec3f)->vec3f {
	return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
	
fn fresnelSchlickRoughness(cosTheta : f32, f0 : vec3f, roughness : f32)->vec3f {
	return f0 + (max(vec3f(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
		   
//Tone mapping - ACES		   
fn toneMapping(color : vec3f) -> vec3f {
	let a = 2.51;
	let b = 0.03;
	let c = 2.43;
	let d = 0.59;
	let e = 0.14;

	return (color * (a * color + b)) / (color * (c * color + d) + e);
}

//Tone mapping - Reinhard (currently unused, ACES is used by default)
/*fn toneMapping(color : vec3f)->vec3f {
      return color / (color + vec3f(1.0));
}*/

//Tone mapping - Uncharted 2 (currently unused)
/*fn uncharted2Helper(x : vec3f) -> vec3f {
    let a = 0.15;
    let b = 0.50;
    let c = 0.10;
    let d = 0.20;
    let e = 0.02;
    let f = 0.30;

    return (x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f) - e / f;
}

fn toneMapping(color : vec3f) -> vec3f {
    let w            = 11.2;
    let exposureBias = 2.0;
    let current      = uncharted2Helper(exposureBias * color);
    let whiteScale   = 1 / uncharted2Helper(vec3f(w));
    return current * whiteScale;
}*/

//Tone mapping - Lottes (currently unused)
/*fn toneMapping(color : vec3f) -> vec3f {
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
}*/

const SHADOW_MAP_SIZE = 4096.0;
	
fn calculateShadows(shadowPosition: vec3f) -> f32 {
  var visibility = 0.0;
  let oneOverShadowDepthTextureSize = 1.0 / SHADOW_MAP_SIZE;
  for (var y = -1; y <= 1; y++) {
    for (var x = -1; x <= 1; x++) {
      let offset = vec2f(vec2(x, y)) * oneOverShadowDepthTextureSize;
      visibility += textureSampleCompare(
        shadowMap,
        shadowSampler,
        shadowPosition.xy + offset,
        shadowPosition.z - 0.005
      );
    }
  }
  visibility /= 9.0;
  return visibility;
}

const MAX_REFLECTION_LOD = 4.0;
const PI = 3.14159265359;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	var visibility = calculateShadows(in.shadowCoord.xyz);
	visibility = 1.0;
	
	let baseColor = textureSample(albedoTexture, albedoSampler, in.uv) * material.baseColorFactor;
	
	if (baseColor.a < material.alphaCutoff) { 
		discard; 
	}
	
	let ao = textureSample(aoTexture, aoSampler, in.uv).r;
	let albedo = baseColor.rgb;

	let roughnessMetallic = textureSample(roughnessMetallicTexture, roughnessMetallicSampler, in.uv);
	let metallic = roughnessMetallic.b;
	let roughness = roughnessMetallic.g;
	let emissive = textureSample(emissiveTexture, emissiveSampler, in.uv).rgb;
	
	var normalSample = textureSample(normalTexture, normalSampler, in.uv).rgb;
	normalSample = normalize(normalSample * 2.0 - 1.0);
	var n = normalize(in.normal);
	let t = normalize(in.tangent);
	let b = cross(n, t);
	let tbn = mat3x3f(t, b, n);
	n = normalize(tbn * normalSample);
	
	let v = normalize(uniforms.camPos - in.worldPosition);
	let r = reflect(-v, n);
	let f0 = mix(vec3f(0.04), albedo, metallic);

	var lo = vec3f(0.0);
	
	{
	   let l = normalize(uniforms.lightPos - in.worldPosition);
	   let h = normalize(v + l);
	   let distance = length(l);
	   let attenuation = 1.0 / (distance * distance);
	   let radiance = uniforms.color.xyz * attenuation;

	   let d = distributionGGX(n, h, roughness);
	   let g = geometrySmith(n, v, l, roughness);
	   let f = fresnelSchlick(max(dot(h, v), 0.0), f0);

	   let numerator = d * g * f;
	   let denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.00001;
	   let specular = numerator / denominator;

	   let kS = f;
	   var kD = vec3f(1.0) - kS;
	   kD *= 1.0 - metallic;

	   let nDotL = max(dot(n, l), 0.00001);
	   lo += (kD * albedo / PI + specular) * radiance * nDotL * visibility;
	}
	
	let f = fresnelSchlickRoughness(max(dot(n, v), 0.00001), f0, roughness);
	let kS = f;
	var kD = vec3f(1.0) - kS;
	kD *= 1.0 - metallic;

	// Negate X component for correct cubemap reflection orientation
	let nFlipped = vec3f(-n.x, n.y, n.z);
	let rFlipped = vec3f(-r.x, r.y, r.z);
	let irradiance = textureSample(irradianceMap, samplerGeneral, nFlipped).rgb;
	let diffuse = irradiance * albedo;

	let prefilteredColor = textureSampleLevel(prefilterMap, samplerGeneral, rFlipped, roughness * MAX_REFLECTION_LOD).rgb;
	let brdf = textureSample(brdfLUT, samplerBRDF, vec2f(max(dot(n, v), 0.0), roughness)).rg;
	let specular = prefilteredColor * (f * brdf.x + brdf.y);

	let ambient = (kD * diffuse + specular) * ao;
	
	var color = ambient + lo + emissive;
	color = toneMapping(color);
	color = pow(color, vec3f(1.0 / 2.2));
	return vec4(color, 1.0);
}