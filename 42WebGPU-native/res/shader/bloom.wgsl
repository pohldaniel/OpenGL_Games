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
	@location(4) fragPos: vec3f
};

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

struct Instance {
    model : mat4x4<f32>,
	color : vec4<f32>,
}

struct Instances {
    instances : array<Instance>,
}

struct Light {
    position: vec3f,
    color: vec4f,
};

struct Lights {
    lights : array<Light>,
}

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> strg : Instances;
@group(0) @binding(2) var smplr: sampler;
@group(0) @binding(3) var texture: texture_2d<f32>;
@group(0) @binding(4) var<storage, read> lights: Lights;

@vertex
fn vs_main(in: VertexInput, @builtin(instance_index) instanceIndex : u32) -> VertexOutput {
	let instance = strg.instances[instanceIndex];
	
	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * instance.model * vec4f(in.position, 1.0);
	out.normal = (instance.model * vec4f(in.normal, 0.0)).xyz;
	out.texcoord = in.texcoord;
	out.color = instance.color;
	out.fragPos = (instance.model * vec4f(in.position, 1.0)).xyz;
	return out;
}

struct PassOutput {
  @location(0) brightness : vec4f,
  @location(1) color : vec4f, 
}

@fragment
fn fs_main(in: VertexOutput) -> PassOutput {

	let color = textureSample(texture, smplr, in.texcoord).rgb * in.color.rgb;
    let normal = normalize(in.normal);
	
	let ambient = 0.0 * color;
    var lighting = vec3f(0.0);
    let viewDir = normalize(uniforms.camPos - in.fragPos);
	
	for(var i = 0; i < 4; i++){
		let light = lights.lights[i];
		
		let lightDir = normalize(light.position - in.fragPos);
		let diff = max(dot(lightDir, normal), 0.0);
        var result = light.color.rgb * diff * color;  
		
		let distance = length(in.fragPos - light.position);
        result *= 1.0 / (distance * distance);
        lighting += result;
	}
	
	let result = ambient + lighting;
    let brightness = dot(result, vec3f(0.2126, 0.7152, 0.0722));

	var output : PassOutput;
	
	output.color = vec4f(result, 1.0);
	output.brightness = select(vec4f(0.0, 0.0, 0.0, 1.0), vec4f(result, 1.0), brightness > 1.0);
	
	return output;
}