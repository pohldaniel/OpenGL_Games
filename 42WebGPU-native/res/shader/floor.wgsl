struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f
};


struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) color: vec4f,
	@location(4) shadowPos: vec4f,
	@location(5) worldPos: vec3f
};

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>,
	lightVP: mat4x4<f32>,
	shadow: mat4x4<f32>,
	lightPos: vec3<f32>
};

struct DirectionalLight {
	dir: vec3f,
	color: vec4f
};

struct PointLight {
	pos: vec3f,
	color: vec4f,
	actve: u32
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var smplr: sampler;
@group(0) @binding(2) var diffuseMap: texture_2d<f32>;
@group(0) @binding(3) var normalMap: texture_2d<f32>;
@group(0) @binding(4) var specularityMap: texture_2d<f32>;
@group(0) @binding(5) var shadowSampler : sampler_comparison; 
@group(0) @binding(6) var shadowMap : texture_depth_2d;          
@group(0) @binding(7) var<uniform> directional: DirectionalLight;
@group(0) @binding(8) var<uniform> point: PointLight;
 
@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * vec4f(in.position, 1.0);
	out.shadowPos = uniforms.shadow * vec4f(in.position, 1.0); 
	out.normal = in.normal;
	out.texcoord = in.texcoord;
	out.color = uniforms.color;
	out.worldPos = in.position;
	return out;
}

const ambient : vec3f = vec3f(0.35 * 0.5 * 0.7 * 0.7, 0.35 * 0.5 * 0.7 * 0.7, 0.35 * 0.5 * 0.7);

fn shadowCalculation(bias: f32, shadowPos: vec4<f32>, offset: vec2<f32>) -> f32 {
	return textureSampleCompare(shadowMap, shadowSampler, shadowPos.xy + offset, shadowPos.z - bias );     
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	var color: vec4f = textureSample(diffuseMap, smplr, in.texcoord * 100.0);
	let diffTexColor = color.rgb;
	
	let lightDir = normalize(-directional.dir);

    var normal: vec3<f32> = textureSample(normalMap, smplr, in.texcoord * 100.0).xyz;
    normal = normalize(normal * 2.0 - 1.0);
	
	let diff = max(dot(normal, lightDir), 0.0);
    let amb = ambient * color.xyz;

	let texelSize = vec2f(1.0, 1.0) / vec2f(textureDimensions(shadowMap));
	var shadow = 0.0;
	for (var y = 0; y <= 1; y++) {
		for (var x = 0; x <= 1; x++) {
			//let offset = vec2f(vec2(x, y)) * texelSize;
			let offset = (vec2f(vec2(x, y)) - 0.5) * texelSize;
			shadow += shadowCalculation(0.001, in.shadowPos, offset);						
		}
	}      
	shadow /= 4.0;
	shadow *= 0.7;
	color = 0.7 * (1.0 - shadow) * directional.color * color * diff + vec4f(amb, 1.0);
	
	
	let specNormal = vec3f(0.0, 1.0, 0.0);
    let specLightDir = normalize(vec3f(-3.0, -4.0, 1.0));
    let reflectDir = reflect(specLightDir, specNormal);
    let viewDir = normalize(uniforms.camPos - in.worldPos);
	
	let shininess: f32 = 1.0;
    let str: f32 = 1.0;
	
	let spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    let specTexColor = textureSample(specularityMap, smplr, in.texcoord * 100.0);
	color += str * spec * specTexColor * directional.color;
	if (point.actve == 1u){
		let pointLightDir = normalize(point.pos - in.worldPos);
		let pointNormal = vec3f(0.0, 1.0, 0.0);
		let pointDiff = max(dot(pointNormal, pointLightDir), 0.0);
		
		let distance = length(point.pos - in.worldPos);
		let linear: f32 = 0.5;
		let constant: f32 = 0.0;
		let quadratic: f32 = 3.0;
		let attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));
		
		var diffuse: vec3f = point.color.rgb * pointDiff * diffTexColor;
		diffuse *= attenuation;
		color += vec4f(diffuse, 1.0);
	}	
	return color;
}