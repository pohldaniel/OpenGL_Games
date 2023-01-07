#version 430

in vec2 texCoord;
in vec4 sc;
in vec3 toCameraVector;
in vec3 fromLightVector;

out vec4 outColor;

uniform sampler2D reflection;
uniform sampler2D refraction;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform vec3 lightColor;
uniform float move;
uniform float flipReflection;

const float waveStrength = 0.04;
const float shineDamper = 20.0;
const float reflectivity = 0.5;

float getDepthPassSpaceZ(float zWC, float near, float far){

	// Assume standard opengl depth range we have to map it (screen -> ndc <=> [0..1] -> [-1..1])
    float z_n = 2.0 * zWC - 1.0;
    float z_e = (2.0 * near * far) / (far + near + z_n * (near - far));	//[near, far]

	// direct substitution
	//float z_e = (near * far) / (far + zWC * (near - far));	//[near, far]

	//divided by far to get the range [near/far, 1.0] just for visualisation
	//float z_e = (2.0 * near) / (far + near + z_n * (near - far));	

	return z_e;
}

void main(void) {
	vec2 ndc = (sc.xy/sc.w);
	
	float depth = texture(depthMap, ndc).r;
	float floorDistance = getDepthPassSpaceZ(depth, 1.0, 5000.0);
	
	depth = gl_FragCoord.z;
	float waterDistance = getDepthPassSpaceZ(depth, 1.0, 5000.0);
	float waterDepth = (floorDistance - waterDistance);

	vec2 distortionTexCoords = texture(dudvMap,vec2(texCoord.x + move, texCoord.y)).rg * 0.1;
	distortionTexCoords = texCoord + vec2(distortionTexCoords.x, distortionTexCoords.y + move);
	vec2 distortion = (texture(dudvMap,distortionTexCoords).rg * 2.0 - 1.0)  * waveStrength * clamp(waterDepth/100.0, 0.0, 1.0);

	//vec2 distortion1 = (texture(dudvMap, vec2(texCoord.x + move, texCoord.y)).rg * 2.0 - 1.0) * waveStrength;
	//vec2 distortion2 = (texture(dudvMap, vec2(-texCoord.x + move, texCoord.y + move)).rg * 2.0 - 1.0) * waveStrength;
	//vec2 distortion = distortion1 + distortion2;
	//vec4 reflectionColor = texture(reflection, texCoord);
	//vec4 refractionColor = texture(refraction, texCoord);
	
	
	
	
	
	//float depth =_depth;
	vec4 reflectionColor = texture(reflection, ndc + distortion);
	vec4 refractionColor = texture(refraction, ndc + distortion);
	
	//vec4 reflectionColor = texture(reflection, clamp(ndc + distortion, 0.001, 0.999));
	//vec4 refractionColor = texture(refraction, clamp(ndc + distortion, 0.001, 0.999));
	
	//in case the rendering to the framebuffers is properly there is no need for flipping the texCoords see
	//see the video of thinmatrix https://www.youtube.com/watch?v=GADTasvDOX4&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=4
	//vec4 reflectionColor = textureProj(reflection, sc);
	//vec4 refractionColor = textureProj(refraction, sc);
	
	vec4 normalMapColor = texture(normalMap, distortionTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);
	
	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, normal);
	refractiveFactor = pow(refractiveFactor, 1);
	
	
	
	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = lightColor * specular * reflectivity * clamp(waterDepth/5.0, 0.0, 1.0);;
	
	outColor = mix(reflectionColor, refractionColor, refractiveFactor);
	outColor = mix(outColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0.0);
	outColor.a = clamp(waterDepth/50.0, 0.0, 1.0);
	
	
	//outColor = vec4(waterDepth, waterDepth, waterDepth, waterDepth)/500;
}