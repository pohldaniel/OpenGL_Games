struct Uniforms {
	projection: mat4x4<f32>,
	view: mat4x4<f32>,
    model: mat4x4<f32>,    
	color: vec4f
}

struct U32s {
	values : array<u32>
}

struct F32s {
	values : array<f32>
}

@group(0) @binding(0) var<uniform> uniforms : Uniforms;
@group(0) @binding(1) var<storage, read> indices : U32s;
@group(0) @binding(2) var<storage, read> positions : F32s;
@group(0) @binding(3) var texture: texture_2d<f32>;
@group(0) @binding(4) var textureSampler: sampler;
//@group(0) @binding(5) var<storage, read> colors : U32s;

struct VertexInput {
	@builtin(instance_index) instanceID : u32,
	@builtin(vertex_index) vertexID : u32
}

struct VertexOutput {
    @builtin(position) position : vec4<f32>,   
	@location(1) texcoord: vec2<f32>,
	@location(2) normal: vec3<f32>,
	@location(3) color : vec4<f32>,
}

@vertex
fn vs_main(in : VertexInput) -> VertexOutput {
    var localToElement = array<u32, 6>(0u, 1u, 1u, 2u, 2u, 0u);

    var triangleIndex = in.vertexID / 6u;
    var localVertexIndex = in.vertexID % 6u;

    var elementIndexIndex = 3u * triangleIndex + localToElement[localVertexIndex];
    var elementIndex = indices.values[elementIndexIndex];

    var position = vec4<f32>(
        positions.values[8u * elementIndex + 0u],
        positions.values[8u * elementIndex + 1u],
        positions.values[8u * elementIndex + 2u],
        1.0
    );
	
	var texcoord = vec2<f32>(
        positions.values[8u * elementIndex + 3u],
        positions.values[8u * elementIndex + 4u]
    );
	
	var normal = vec4<f32>(
        positions.values[8u * elementIndex + 5u],
        positions.values[8u * elementIndex + 6u],
        positions.values[8u * elementIndex + 7u],
        0.0
    );
	
    position = uniforms.projection * uniforms.view * uniforms.model * position;
	normal = (uniforms.model * normal);

    var output : VertexOutput;
    output.position = position;
	output.texcoord = texcoord;
	output.normal = normal.xyz;
    output.color = uniforms.color;

    return output;
}

struct FragmentInput {
    @location(0) color : vec4<f32>
}

struct FragmentOutput {
    @location(0) color : vec4<f32>
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let diffuse_color = textureSample(texture, textureSampler, in.texcoord);
	let normal = normalize(in.normal);

	let lightColor1 = vec3f(1.0, 0.9, 0.6);
	let lightColor2 = vec3f(0.6, 0.9, 1.0);
	let lightDirection1 = vec3f(0.5, -0.9, 0.1);
	let lightDirection2 = vec3f(0.2, 0.4, 0.3);
	let shading1 = max(0.0, dot(lightDirection1, normal));
	let shading2 = max(0.0, dot(lightDirection2, normal));
	let shading = shading1 * lightColor1 + shading2 * lightColor2;
	let color = in.color.xyz * shading;

	// Gamma-correction
	let corrected_color = pow(color.xyz, vec3f(2.2));
	return vec4f(corrected_color, in.color.a) * diffuse_color;
}