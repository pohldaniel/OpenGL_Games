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

struct Wiggly {
	nosePos: vec3f,
	time: f32
};

struct Instance {
    modelMatrix : mat4x4<f32>
}

struct InstanceData {
    instances : array<Instance>,
}

struct PointLight {
	pos: vec3f,
	color: vec4f,
	actve: u32
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> wiggly: Wiggly;
@group(0) @binding(2) var<storage, read> instanceStorage : InstanceData;
@group(0) @binding(3) var smplr: sampler;
@group(0) @binding(4) var diffuseMap: texture_2d<f32>;
@group(0) @binding(5) var normalMap: texture_2d<f32>;
@group(0) @binding(6) var specularityMap: texture_2d<f32>;
@group(0) @binding(7) var shadowSampler : sampler_comparison; 
@group(0) @binding(8) var shadowMap : texture_depth_2d;
@group(0) @binding(9) var<uniform> point: PointLight;

const wiggleMagnitude: f32 = 0.03;
const wiggleDistModifier: f32 = 0.12;
const wiggleTimeModifier: f32 = 9.4;

@vertex
fn vs_main(in : VertexInput, @builtin(instance_index) instanceIndex : u32) -> VertexOutput {
	var out: VertexOutput;
	let instance = instanceStorage.instances[instanceIndex];
	let xOffset = sin(wiggleTimeModifier * wiggly.time + wiggleDistModifier * distance(wiggly.nosePos, in.position * 100.0)) * wiggleMagnitude;
	
	out.worldPos = (instance.modelMatrix * vec4(in.position.x + xOffset, in.position.y, in.position.z, 1.0)).xyz;
	out.position = uniforms.projection * uniforms.view * vec4f(out.worldPos, 1.0);
	out.shadowPos = uniforms.shadow * vec4f(out.worldPos, 1.0);
	out.normal = (instance.modelMatrix * vec4f(in.normal, 0.0)).xyz;
	out.texcoord = in.texcoord;
	out.color = uniforms.color;
	return out;
}

const ambient : vec3f = vec3f(0.8 * 0.1 * 0.9 * 0.7, 0.8 * 0.1 * 0.9 * 0.7, 0.8 * 0.1 * 0.7);
const lightDirection : vec3f =  normalize(vec3f(-1.0, -1.0, 1.0));
const lightColor : vec4f =  vec4f(0.8  * 0.9 * 0.406, 0.8  * 0.9 * 0.723, 1.0, 1.0);

fn shadowCalculation(bias: f32, shadowPos: vec4<f32>) -> f32 {
	return textureSampleCompare(shadowMap, shadowSampler, shadowPos.xy, shadowPos.z - bias );     
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {

	var color: vec4<f32> = textureSample(diffuseMap, smplr, in.texcoord);
	let diffTexColor = color.rgb;
	
	let normal = normalize(in.normal);
	let lightDir = -lightDirection;
	let diff = max(dot(normal, lightDir), 0.0);
    let amb = ambient * diffTexColor;

	var shadow = shadowCalculation(0.001, in.shadowPos);	
	
	color = (1.0 - shadow) * lightColor * color * diff + vec4f(amb, 1.0);
	
	if (point.actve == 1u){
		let lightDir = normalize(point.pos - in.worldPos);
        let diff = max(dot(normal, lightDir), 0.0);
		
		//let diffTexColor = textureSample(texture_diffuse, default_sampler, input.TexCoord).xyz;
        let diffuse = 0.7 * point.color.rgb * diff * diffTexColor;          
        color += vec4f(diffuse, 1.0);
	}
	
	let specTexColor = textureSample(specularityMap, smplr, in.texcoord);
	if (shadow < 0.1) {
		let reflectDir = reflect(lightDir, normal);
		let viewDir = normalize(uniforms.camPos - in.worldPos);
		
		let shininess: f32 = 24.0;
        let str: f32 = 1.0;
            
        let spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
       
            
        color += str * spec * specTexColor * lightColor;
        color += spec * 0.1 * vec4f(1.0, 1.0, 1.0, 1.0);
	}
	return color;
}