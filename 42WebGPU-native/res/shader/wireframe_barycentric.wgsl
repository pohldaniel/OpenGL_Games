struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) color: vec4f,
	@location(2) barycenticCoord: vec3f
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

	let vertNdx = vertexIndex % 3;
	let index = indices[vertexIndex];

	let pNdx = index * line.stride;
	let position = vec4f(positions[pNdx], positions[pNdx + 1], positions[pNdx + 2], 1);

	var out: VertexOutput;
	out.position = uniforms.projection * uniforms.view * instance.model * position;
	out.color = instance.color;

	// emit a barycentric coordinate
	out.barycenticCoord = vec3f(0.0);
	out.barycenticCoord[vertNdx] = 1.0;
	return out;
}

fn edgeFactor(bary: vec3f) -> f32 {
  let d = fwidth(bary);
  let a3 = smoothstep(vec3f(0.0), d * line.thickness, bary);
  return min(min(a3.x, a3.y), a3.z);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	let a = 1.0 - edgeFactor(in.barycenticCoord);
	if (a < line.alphaThreshold) {
	  discard;
	}

	return vec4((in.color.rgb + 0.5) * a, a);
}