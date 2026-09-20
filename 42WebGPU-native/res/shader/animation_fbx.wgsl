struct VertexInput {
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) weight: vec4f,
	@location(4) joint: vec4u
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

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> skin: array<mat4x4f>;
@group(0) @binding(2) var shadowSampler : sampler_comparison; 
@group(0) @binding(3) var shadowMap : texture_depth_2d; 

fn get_world_matrix(weight : vec4f, joint : vec4u) -> mat4x4f {
	return skin[joint.x] * weight.x + skin[joint.y] * weight.y +
           skin[joint.z] * weight.z + skin[joint.w] * weight.w;
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	let world = get_world_matrix(in.weight, in.joint);  
	
	out.position = uniforms.projection * uniforms.view * world * vec4f(in.position, 1.0);
	out.shadowPos = uniforms.shadow * world * vec4f(in.position, 1.0);
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
	
	return vec4f(in.normal * lightFactor, 1.0);
}