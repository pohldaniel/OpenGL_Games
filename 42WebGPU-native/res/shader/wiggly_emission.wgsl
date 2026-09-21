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
	@location(4) shadowPos: vec4f
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

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> wiggly: Wiggly;
@group(0) @binding(2) var<storage, read> instanceStorage : InstanceData;
@group(0) @binding(3) var smplr: sampler;
@group(0) @binding(4) var texture: texture_2d<f32>;
@group(0) @binding(5) var shadowSampler : sampler_comparison; 
@group(0) @binding(6) var shadowMap : texture_depth_2d; 

const wiggleMagnitude: f32 = 0.03;
const wiggleDistModifier: f32 = 0.12;
const wiggleTimeModifier: f32 = 9.4;

@vertex
fn vs_main(in : VertexInput, @builtin(instance_index) instanceIndex : u32) -> VertexOutput {
	var out: VertexOutput;
	let instance = instanceStorage.instances[instanceIndex];
	
	
	let xOffset = sin(wiggleTimeModifier * wiggly.time + wiggleDistModifier * distance(wiggly.nosePos, in.position * 100.0)) * wiggleMagnitude;
	
	out.position = uniforms.projection * uniforms.view * instance.modelMatrix * vec4(in.position.x + xOffset, in.position.y, in.position.z, 1.0);
	out.shadowPos = uniforms.shadow * instance.modelMatrix * vec4(in.position.x + xOffset, in.position.y, in.position.z, 1.0);
	out.normal = in.normal;
	out.texcoord = in.texcoord;
	out.color = uniforms.color;
	return out;
}

fn shadowCalculation(bias: f32, shadowPos: vec4<f32>, offset: vec2<f32>) -> f32 {
	return textureSampleCompare(shadowMap, shadowSampler, shadowPos.xy + offset, shadowPos.z - bias );     
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
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
    let shadowIntensity = shadow * 0.7;
    let lightFactor = 1.0 - shadowIntensity; 
    
	let texColor = textureSample(texture, smplr, in.texcoord);
    return vec4<f32>(texColor.rgb * lightFactor, texColor.a);
}