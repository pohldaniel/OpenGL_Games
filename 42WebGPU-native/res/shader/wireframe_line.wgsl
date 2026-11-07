struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) color: vec4f
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

struct Instance {
    model : mat4x4<f32>,
	color: vec4<f32>
}

struct LineUniforms {
    stride: u32,
    thickness: f32,
    alphaThreshold: f32,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> instance : Instance;
@group(0) @binding(2) var<uniform> line : LineUniforms;
@group(0) @binding(3) var<storage, read> positions: array<f32>;
@group(0) @binding(4) var<storage, read> indices: array<u32>;

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {

	let triNdx = vertexIndex / 6;
	// 0 1 0 1 0 1  0 1 0 1 0 1  vNdx % 2
	// 0 0 1 1 2 2  3 3 4 4 5 5  vNdx / 2
	// 0 1 1 2 2 3  3 4 4 5 5 6  vNdx % 2 + vNdx / 2
	// 0 1 1 2 2 0  0 1 1 2 2 0  (vNdx % 2 + vNdx / 2) % 3
	let vertNdx = (vertexIndex % 2 + vertexIndex / 2) % 3;
	let index = indices[triNdx * 3 + vertNdx];

	let pNdx = index * line.stride;
	let position = vec4f(positions[pNdx], positions[pNdx + 1], positions[pNdx + 2], 1.0);

	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * instance.model * position;
	out.color = instance.color;
	return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	return in.color + vec4f(0.5);
}