struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) texCoord: vec2<f32>,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    var out: VertexOutput;
    // Generiert ein bildschirmfüllendes Dreieck aus dem Index (0, 1, 2)
    let uv = vec2<f32>(f32((vertexIndex << 1u) & 2u), f32(vertexIndex & 2u));
    out.position = vec4<f32>(uv * 2.0 - 1.0, 0.0, 1.0);
    out.texCoord = vec2<f32>(uv.x, 1.0 - uv.y); // Y-Achse für WebGPU flippen
    return out;
}

struct DownsampleUniforms {
    srcResolution: vec2<f32>,
    mipLevel: u32,
};

@group(0) @binding(0) var srcSampler: sampler;
@group(0) @binding(1) var srcTexture: texture_2d<f32>;
@group(0) @binding(2) var<uniform> uniforms: DownsampleUniforms;

fn ToSRGB(v: vec3<f32>) -> vec3<f32> {
    return pow(v, vec3<f32>(1.0 / 2.2));
}

fn sRGBToLuma(col: vec3<f32>) -> f32 {
    return dot(col, vec3<f32>(0.299, 0.587, 0.114));
}

fn KarisAverage(col: vec3<f32>) -> f32 {
    let luma = sRGBToLuma(ToSRGB(col)) * 0.25;
    return 1.0 / (1.0 + luma);
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4<f32> {
    let texDims = textureDimensions(srcTexture, 0); 
    
    //let srcTexelSize = vec2<f32>(1.0 / f32(texDims.x), 1.0 / f32(texDims.y));	
	let srcTexelSize = 1.0 / uniforms.srcResolution;
    let x = srcTexelSize.x;
    let y = srcTexelSize.y;
    let uv = input.texCoord;

    let a = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - 2.0*x, uv.y + 2.0*y)).rgb;
    let b = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x,       uv.y + 2.0*y)).rgb;
    let c = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + 2.0*x, uv.y + 2.0*y)).rgb;

    let d = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - 2.0*x, uv.y)).rgb;
    let e = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x,       uv.y)).rgb;
    let f = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + 2.0*x, uv.y)).rgb;

    let g = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - 2.0*x, uv.y - 2.0*y)).rgb;
    let h = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x,       uv.y - 2.0*y)).rgb;
    let i = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + 2.0*x, uv.y - 2.0*y)).rgb;

    let j = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - x, uv.y + y)).rgb;
    let k = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + x, uv.y + y)).rgb;
    let l = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x - x, uv.y - y)).rgb;
    let m = textureSample(srcTexture, srcSampler, vec2<f32>(uv.x + x, uv.y - y)).rgb;

    var downsample: vec3<f32>;

    if (uniforms.mipLevel == 0u) {
        var groups: array<vec3<f32>, 5>;
        groups[0] = (a + b + d + e) * (0.125 / 4.0);
        groups[1] = (b + c + e + f) * (0.125 / 4.0);
        groups[2] = (d + e + g + h) * (0.125 / 4.0);
        groups[3] = (e + f + h + i) * (0.125 / 4.0);
        groups[4] = (j + k + l + m) * (0.5 / 4.0);

        groups[0] *= KarisAverage(groups[0]);
        groups[1] *= KarisAverage(groups[1]);
        groups[2] *= KarisAverage(groups[2]);
        groups[3] *= KarisAverage(groups[3]);
        groups[4] *= KarisAverage(groups[4]);

        downsample = groups[0] + groups[1] + groups[2] + groups[3] + groups[4];
        downsample = max(downsample, vec3<f32>(0.0001));
    } else {
        downsample = e * 0.125;
        downsample += (a + c + g + i) * 0.03125;
        downsample += (b + d + f + h) * 0.0625;
        downsample += (j + k + l + m) * 0.125;
    }

    return vec4<f32>(downsample, 1.0);
}