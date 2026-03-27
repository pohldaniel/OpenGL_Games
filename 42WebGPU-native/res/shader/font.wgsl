struct VertexInput {
	@location(0) posTex: vec4f,
	@location(1) color: vec4f,
	@location(2) layer: u32
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) uv: vec2f,
	@location(1) color: vec4f,
	@location(2) @interpolate(flat) layer: u32
};

struct Uniforms {
    projectionMatrix: mat4x4f,
    viewMatrix: mat4x4f,
    modelMatrix: mat4x4f,
	normalMatrix: mat4x4f,
    color: vec4f,
	camPosition: vec3f,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var fontSampler: sampler;

@group(1) @binding(0) var fontTexture: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	out.position = uniforms.projectionMatrix * vec4<f32>(in.posTex.xy, 0.0, 1.0);
	out.uv = in.posTex.zw;
	out.color = in.color;
	out.layer = in.layer;
	return out;
}

fn sampleMsdf(texcoord: vec2f) -> f32 {
  let c = textureSample(fontTexture, fontSampler, texcoord);
  return max(min(c.r, c.g), min(max(c.r, c.g), c.b));
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {

	//let sample = textureSample(fontTexture, fontSampler, in.uv);
	//return sample;
	
	let pxRange = 4.0;
	let sz = vec2f(textureDimensions(fontTexture, 0));
	let dx = sz.x*length(vec2f(dpdx(in.uv.x), dpdy(in.uv.x)));
	let dy = sz.y*length(vec2f(dpdx(in.uv.y), dpdy(in.uv.y)));
	let toPixels = pxRange * inverseSqrt(dx * dx + dy * dy);
	let sigDist = sampleMsdf(in.uv) - 0.5;
	let pxDist = sigDist * toPixels;

	let edgeWidth = 0.5;

	let alpha = smoothstep(-edgeWidth, edgeWidth, pxDist);

	if (alpha < 0.001) {
	  discard;
	}

	return vec4f(in.color.rgb, in.color.a * alpha);
}