struct VertexInput {
	@location(0) position: vec3f,
};

struct VertexOutput {
	@builtin(position) position : vec4<f32>,
	@location(0) local_pos : vec3<f32>,
}
 
@group(0) @binding(0) var<uniform> view_projection : mat4x4<f32>;
@group(0) @binding(1) var environment_sampler : sampler;
@group(0) @binding(2) var environment_map : texture_cube<f32>;
@group(0) @binding(3) var<uniform> roughness : f32;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var output : VertexOutput;
	output.position = view_projection * vec4f(in.position, 1.0);
	output.local_pos = in.position;
	return output;
}

const PI : f32 = 3.1415926535897932384626433832795;

fn radical_inverse_vdc(bits_in : u32) -> f32 {
	var bits = bits_in;
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return f32(bits) * 2.3283064365386963e-10;
}

fn hammersley(i : u32, n : u32) -> vec2<f32> {
   return vec2<f32>(f32(i) / f32(n), radical_inverse_vdc(i));
}

fn importance_sample_ggx(xi : vec2<f32>, n : vec3<f32>, roughness_val : f32) -> vec3<f32> {
	let a = roughness_val * roughness_val;
	let phi = 2.0 * PI * xi.x;
	let cos_theta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
	let sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	let h = vec3<f32>(
		cos(phi) * sin_theta,
		sin(phi) * sin_theta,
		cos_theta
	);

	let up = select(vec3<f32>(1.0, 0.0, 0.0), vec3<f32>(0.0, 0.0, 1.0), abs(n.z) < 0.999);
	let tangent = normalize(cross(up, n));
	let bitangent = cross(n, tangent);

	return normalize(tangent * h.x + bitangent * h.y + n * h.z);
}

@fragment
fn fs_main(input : VertexOutput) -> @location(0) vec4<f32> {
	let n = normalize(input.local_pos);
	let r = n;
	let v = r;

	const SAMPLE_COUNT = 1024u;
	var prefiltered_color = vec3<f32>(0.0);
	var total_weight = 0.0;

	for (var i = 0u; i < SAMPLE_COUNT; i++) {
		let xi = hammersley(i, SAMPLE_COUNT);
		let h = importance_sample_ggx(xi, n, roughness);
		let l = normalize(2.0 * dot(v, h) * h - v);

		let n_dot_l = max(dot(n, l), 0.0);
		if (n_dot_l > 0.0) {
			prefiltered_color += textureSampleLevel(environment_map, environment_sampler, l, 0.0).rgb * n_dot_l;
			total_weight += n_dot_l;
		}
	}

	prefiltered_color = prefiltered_color / total_weight;
	return vec4<f32>(prefiltered_color, 1.0);
}