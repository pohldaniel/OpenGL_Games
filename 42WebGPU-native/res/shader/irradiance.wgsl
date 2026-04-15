struct VertexInput {
	@location(0) position: vec3f
};

struct VertexOutput {
	@builtin(position) position : vec4<f32>,
	@location(0) local_pos : vec3<f32>,
};

@group(0) @binding(0) var<uniform> view_projection : mat4x4<f32>;
@group(0) @binding(1) var smplr : sampler;
@group(0) @binding(2) var texture : texture_cube<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var output : VertexOutput;
	output.position = view_projection * vec4f(in.position, 1.0);
	output.local_pos = in.position;
	return output;
}

const PI : f32 = 3.1415926535897932384626433832795;
const SAMPLE_DELTA : f32 = 0.025;

@fragment
fn fs_main(input : VertexOutput) -> @location(0) vec4<f32> {
	let normal = normalize(input.local_pos);
	var irradiance = vec3<f32>(0.0);

	let up = vec3<f32>(0.0, 1.0, 0.0);
	let right = normalize(cross(up, normal));
	let up_tangent = normalize(cross(normal, right));

	var num_samples = 0.0;
	var phi = 0.0;
	while (phi < 2.0 * PI) {
		var theta = 0.0;
		while (theta < 0.5 * PI) {
			let tangent_sample = vec3<f32>(
			sin(theta) * cos(phi),
			sin(theta) * sin(phi),
			cos(theta));
			
			let sample_vec = tangent_sample.x * right + tangent_sample.y * up_tangent + tangent_sample.z * normal;

			irradiance += textureSample(texture, smplr, sample_vec).rgb * cos(theta) * sin(theta);
			num_samples += 1.0;

			theta += SAMPLE_DELTA;
		}
		phi += SAMPLE_DELTA;
	}

	irradiance = PI * irradiance / num_samples;
	return vec4<f32>(irradiance, 1.0);
}