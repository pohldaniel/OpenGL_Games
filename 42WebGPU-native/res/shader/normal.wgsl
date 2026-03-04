const modeAlbedoTexture = 0;
const modeNormalTexture = 1;
const modeDepthTexture  = 2;
const modeNormalMap     = 3;
const modeParallaxScale = 4;
const modeSteepParallax = 5;

struct VertexInput {
	@location(0) position: vec3f,
	@location(1) uv: vec2f,
	@location(2) normal: vec3f,
	@location(3) tangent: vec3f,
	@location(4) bitangent: vec3f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
	@location(1) normal: vec3f,	
	@location(2) color: vec4f,
	@location(3) viewDirection: vec3f,
	@location(4) posVS : vec3f,       // vertex position in view space
    @location(5) tangentVS: vec3f,    // vertex tangent in view space
    @location(6) bitangentVS: vec3f,  // vertex tangent in view space
    @location(7) normalVS: vec3f      // vertex normal in view space
};

/**
 * A structure holding the value of our uniforms
 */
struct Uniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
	normalMatrix: mat4x4f,
    color: vec4f,
	camPosition: vec3f,
};

struct NormalUniform {
  lightPosVS: vec3f,
  mode: u32,
  lightIntensity: f32,
  depthScale: f32,
  depthLayers: f32,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var textureSampler: sampler;
@group(0) @binding(2) var albedoTexture: texture_2d<f32>;
@group(0) @binding(3) var normalTexture: texture_2d<f32>;
@group(0) @binding(4) var heightTexture: texture_2d<f32>;
@group(0) @binding(5) var<uniform> normalUniforms: NormalUniform;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	let worldPosition = uniforms.modelMatrix * vec4<f32>(in.position, 1.0);
	out.position = uniforms.projectionMatrix * uniforms.viewMatrix * worldPosition;
	out.uv = in.uv;
    out.normal = (uniforms.modelMatrix * vec4f(in.normal, 0.0)).xyz;
	out.color = uniforms.color;
	
	out.posVS = (uniforms.viewMatrix * uniforms.modelMatrix * vec4(in.position, 1.0)).xyz;
    out.tangentVS = (uniforms.viewMatrix * uniforms.modelMatrix * vec4(in.tangent, 0.0)).xyz;
    out.bitangentVS = (uniforms.viewMatrix * uniforms.modelMatrix * vec4(in.bitangent, 0.0)).xyz;
    out.normalVS = (uniforms.viewMatrix * uniforms.modelMatrix * vec4(in.normal, 0.0)).xyz;
	
	out.viewDirection = uniforms.camPosition - worldPosition.xyz;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
	let tangentToView = mat3x3f(
		in.tangentVS,
		in.bitangentVS,
		in.normalVS,
	);
	
	// The inverse of a non-scaling affine 3x3 matrix is it's transpose
	let viewToTangent = transpose(tangentToView);

	// Calculate the normalized vector in tangent space from the camera to the fragment
	let viewDirTS = normalize(viewToTangent * in.posVS);
	
	// Apply parallax to the texture coordinate, if parallax is enabled
	var uv : vec2f;
	switch (normalUniforms.mode) {
	  case modeParallaxScale: {
		uv = parallaxScale(in.uv, viewDirTS);
		break;
	  }
	  case modeSteepParallax: {
		uv = parallaxSteep(in.uv, viewDirTS);
		break;
	  }
	  default: {
		uv = in.uv;
		break;
	  }
	}
	
	// Sample the albedo texture
	let albedoSample = textureSample(albedoTexture, textureSampler, uv);

	// Sample the normal texture
	let normalSample = textureSample(normalTexture, textureSampler, uv);
	
	switch (normalUniforms.mode) {
	   case modeAlbedoTexture: { // Output the albedo sample
		 return albedoSample;
	   }
	   case modeNormalTexture: { // Output the normal sample
		 return normalSample;
	   }
	   case modeDepthTexture: { // Output the depth map
		 return textureSample(heightTexture, textureSampler, in.uv);
	   }
	   default: {
		 // Transform the normal sample to a tangent space normal
		 let normalTS = normalSample.xyz * 2 - 1;

		 // Convert normal from tangent space to view space, and normalize
		 let normalVS = normalize(tangentToView * normalTS);

		 // Calculate the vector in view space from the light position to the fragment
		 let fragToLightVS = normalUniforms.lightPosVS - in.posVS;

		 // Calculate the square distance from the light to the fragment
		 let lightSqrDist = dot(fragToLightVS, fragToLightVS);

		 // Calculate the normalized vector in view space from the fragment to the light
		 let lightDirVS = fragToLightVS * inverseSqrt(lightSqrDist);

		 // Light strength is inversely proportional to square of distance from light
		 let diffuseLight = normalUniforms.lightIntensity * max(dot(lightDirVS, normalVS), 0) / lightSqrDist;

		 // The diffuse is the albedo color multiplied by the diffuseLight
		 let diffuse = albedoSample.rgb * diffuseLight;

		 return vec4f(diffuse, 1.0);
		 //return textureSample(heightTexture, textureSampler, in.uv);
	   }		
	}
	//return vec4f(in.uv, 0.0, 1.0);
}
// Returns the uv coordinate displaced in the view direction by a magnitude calculated by the depth
// sampled from the depthTexture and the angle between the surface normal and view direction.
fn parallaxScale(uv: vec2f, viewDirTS: vec3f) -> vec2f {
  let depthSample = textureSample(heightTexture, textureSampler, uv).r;
  return uv + viewDirTS.xy * (depthSample * normalUniforms.depthScale) / -viewDirTS.z;
}

// Returns the uv coordinates displaced in the view direction by ray-tracing the depth map.
fn parallaxSteep(startUV: vec2f, viewDirTS: vec3f) -> vec2f {
  // Calculate derivatives of the texture coordinate, so we can sample the texture with non-uniform
  // control flow.
  let ddx = dpdx(startUV);
  let ddy = dpdy(startUV);

  // Calculate the delta step in UV and depth per iteration
  let uvDelta = viewDirTS.xy * normalUniforms.depthScale / (-viewDirTS.z * normalUniforms.depthLayers);
  let depthDelta = 1.0 / f32(normalUniforms.depthLayers);
  let posDelta = vec3(uvDelta, depthDelta);

  // Walk the depth texture, and stop when the ray intersects the depth map
  var pos = vec3(startUV, 0);
  for (var i = 0; i < 32; i++) {
    if (pos.z >= textureSampleGrad(heightTexture, textureSampler, pos.xy, ddx, ddy).r) {
      break; // Hit the surface
    }
    pos += posDelta;
  }

  return pos.xy;
}