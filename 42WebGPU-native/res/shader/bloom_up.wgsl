struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) texCoord: vec2<f32>,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    var out: VertexOutput;
    if (vertexIndex == 0u) {
        out.position = vec4<f32>(-1.0, -1.0, 0.0, 1.0);
        out.texCoord = vec2<f32>(0.0, 1.0); 
    } else if (vertexIndex == 1u) {
        out.position = vec4<f32>(3.0, -1.0, 0.0, 1.0);
        out.texCoord = vec2<f32>(2.0, 1.0);
    } else {
        out.position = vec4<f32>(-1.0, 3.0, 0.0, 1.0);
        out.texCoord = vec2<f32>(0.0, -1.0);
    }
    return out;
}

struct UpsampleUniforms {
    filterRadius: f32,
};

@group(0) @binding(0) var srcSampler: sampler;
@group(0) @binding(1) var srcTexture: texture_2d<f32>;
@group(0) @binding(2) var<uniform> uniforms: UpsampleUniforms;

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4<f32> {
    let x = uniforms.filterRadius;
    let y = uniforms.filterRadius;
    let uv = input.texCoord;

    let a = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - x, uv.y + y)).rgb;
    let b = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x,     uv.y + y)).rgb;
    let c = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + x, uv.y + y)).rgb;

    let d = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - x, uv.y)).rgb;
    let e = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x,     uv.y)).rgb;
    let f = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + x, uv.y)).rgb;

    let g = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - x, uv.y - y)).rgb;
    let h = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x,     uv.y - y)).rgb;
    let i = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + x, uv.y - y)).rgb;

    var upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;

    return vec4<f32>(upsample, 1.0);
}