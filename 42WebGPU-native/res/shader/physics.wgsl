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

struct Instance {
    modelMatrix : mat4x4<f32>,
    color       : vec4<f32>,
}

struct InstanceData {
    instances : array<Instance>,
}
@group(0) @binding(1) var<storage, read> instanceStorage : InstanceData;

struct VertexInput {
    @location(0) position : vec3<f32>
}

struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) color : vec4<f32>,
}

@vertex
fn vs_main(in : VertexInput, @builtin(instance_index) instanceIdx : u32) -> VertexOutput {
    var out : VertexOutput;
    
    // Hole die Weltmatrix für DIESEN spezifischen Cube
     let instance = instanceStorage.instances[instanceIdx];
    
    // Transformation: Local -> World -> Clip Space
    out.position = uniforms.projection * uniforms.view * instance.modelMatrix * vec4<f32>(in.position, 1.0);
    out.color = instance.color;
    return out;
}

@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4<f32> {
    return in.color;
}