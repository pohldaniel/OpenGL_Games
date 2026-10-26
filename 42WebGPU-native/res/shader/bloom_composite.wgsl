struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texcoord: vec2f,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    var positions = array<vec2f, 6>(
        vec2f(-1.0, -1.0), vec2f( 1.0, -1.0), vec2f(-1.0,  1.0),
        vec2f(-1.0,  1.0), vec2f( 1.0, -1.0), vec2f( 1.0,  1.0)
    );
    var texcoords = array<vec2f, 6>(
        vec2f(0.0, 1.0), vec2f(1.0, 1.0), vec2f(0.0, 0.0),
        vec2f(0.0, 0.0), vec2f(1.0, 1.0), vec2f(1.0, 0.0)
    );

    var out: VertexOutput;
    out.position = vec4f(positions[vertexIndex], 0.0, 1.0);
    out.texcoord = texcoords[vertexIndex];
    return out;
}

struct CompositeUniforms {
    exposure: f32,
    bloomStrength: f32,
};

@group(0) @binding(0) var smplr: sampler;
@group(0) @binding(1) var sceneTexture: texture_2d<f32>;
@group(0) @binding(2) var bloomTexture: texture_2d<f32>;
@group(0) @binding(3) var<uniform> uniforms: CompositeUniforms;

fn bloom_new(texcoord: vec2f) -> vec3f {
    let hdrColor = textureSample(sceneTexture, smplr, texcoord).rgb;
    let bloomColor = textureSample(bloomTexture, smplr, texcoord).rgb;
    return mix(hdrColor, bloomColor, uniforms.bloomStrength);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    var result = bloom_new(in.texcoord);
    result = vec3f(1.0) - exp(-result * uniforms.exposure);
    const gamma = 2.2;
    result = pow(result, vec3f(1.0 / gamma)); 
    return vec4f(result, 1.0);
}