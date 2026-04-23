struct VertexInput {
	@location(0) position: vec3f,
	@location(1) uv: vec2f,
	@location(2) normal: vec3f
};

struct VertexOutput {
  @builtin(position) position: vec4f,
  @location(0) uv: vec2f,
  @location(1) normal: vec3f,
  @location(2) worldPosition: vec3f,
  @location(3) @interpolate(flat) instanceIndex: u32,
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
@group(0) @binding(1) var<uniform> modelMatrices: array<mat4x4f, 12>;

@vertex
fn vs_main(@builtin(instance_index) instanceIndex: u32, in: VertexInput) -> VertexOutput {
  var out: VertexOutput;
  out.position = uniforms.projection * uniforms.view * uniforms.model * modelMatrices[instanceIndex] * vec4f(in.position, 1);
  out.normal = in.normal;
  out.uv = in.uv;
  out.worldPosition = (modelMatrices[instanceIndex] * vec4f(in.position, 1)).xyz;
  out.instanceIndex = instanceIndex;
  return out;
}

struct Light {
    position : vec3f,
    padding1 : f32,
    color : vec3f,
    padding2 : f32,
}

@group(0) @binding(2) var<uniform> lights: array<Light, 4>;

@group(1) @binding(0) var smplr: sampler;
@group(1) @binding(1) var smplrBRDF: sampler;
@group(1) @binding(2) var brdfLUT: texture_2d<f32>;
@group(1) @binding(3) var irradianceMap: texture_cube<f32>;
@group(1) @binding(4) var prefilterMap: texture_cube<f32>;

const PI                 = 3.14159265359;
const MAX_REFLECTION_LOD = 4.0;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {

	let ao       = 1.0;
	let albedo   = select(vec3f(0.957, 0.792, 0.407), vec3f(1, 0, 0), in.instanceIndex < 6);
	let metallic = select(1.0, 0.0, in.instanceIndex < 6);
	let roughness = select(f32(in.instanceIndex) % 6 / 6, 1.0, in.instanceIndex > 5) ;

	let n = normalize(in.normal);
	let v = normalize(uniforms.camPos - in.worldPosition);
	let r = reflect(-v, n);

	let f0 = mix(vec3f(0.04), albedo, metallic);

	var lo = vec3f(0.0);
	
	for (var i = 0; i < 4; i++) {
	   let l = normalize(lights[i].position - in.worldPosition);
	   let h = normalize(v + l);

	   let distance    = length(lights[i].position - in.worldPosition);
	   let attenuation = 1.0 / (distance * distance);
	   let radiance    = lights[i].color * attenuation;

	   let d = distributionGGX(n, h, roughness);
	   let g = geometrySmith(n, v, l, roughness);
	   let f = fresnelSchlick(max(dot(h, v), 0.0), f0);

	   let numerator = d * g * f;
	   let denominator =
		   4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.00001;
	   let specular = numerator / denominator;

	   let kS = f;
	   var kD = vec3f(1.0) - kS;
	   kD *= 1.0 - metallic;

	   let nDotL = max(dot(n, l), 0.00001);
	   lo += (kD * albedo / PI + specular) * radiance * nDotL;
	}
	
	let f = fresnelSchlickRoughness(max(dot(n, v), 0.00001), f0, roughness);
	let kS = f;
	var kD = vec3f(1.0) - kS;
	kD *= 1.0 - metallic;

	let irradiance = textureSample(irradianceMap, smplr, n).rgb;
	let diffuse    = irradiance * albedo;

	let prefilteredColor =
		textureSampleLevel(prefilterMap, smplr, r, roughness * MAX_REFLECTION_LOD)
			.rgb;
	let brdf =
		textureSample(brdfLUT, smplrBRDF,
					  vec2f(max(dot(n, v), 0.0), roughness))
			.rg;
	let specular = prefilteredColor * (f * brdf.x + brdf.y);

	let ambient = (kD * diffuse + specular) * ao;

	var color = ambient + lo;
	color     = toneMapping(color);
	color     = pow(color, vec3f(1.0 / 2.2));
	return vec4f(color, 1.0);
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

fn fresnelSchlickRoughness(cosTheta : f32, f0 : vec3f, roughness : f32) ->vec3f {
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

fn toneMapping(color : vec3f)->vec3f {
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