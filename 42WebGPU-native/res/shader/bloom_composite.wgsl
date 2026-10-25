struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texcoord: vec2f,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    // Beibehaltung deines bewährten 6-Vertex (Quad) Setups
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

// Uniform-Struktur für die Steuerung (16-Byte Aligned)
struct CompositeUniforms {
    exposure: f32,
    bloomStrength: f32,
    padding0: f32, // Padding zum Auffüllen auf 16 Byte
    padding1: f32,
};

@group(0) @binding(0) var smplr: sampler;
@group(0) @binding(1) var sceneTexture: texture_2d<f32>;
@group(0) @binding(2) var bloomTexture: texture_2d<f32>;
@group(0) @binding(3) var<uniform> uniforms: CompositeUniforms;

fn bloom_new(texcoord: vec2f) -> vec3f {
    let hdrColor = textureSample(sceneTexture, smplr, texcoord).rgb;
    let bloomColor = textureSample(bloomTexture, smplr, texcoord).rgb;
    
    // mix() entspricht in GLSL dem linearen Interpolieren (lerp)
    return mix(hdrColor, bloomColor, uniforms.bloomStrength);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    // 1. Bloom & Szene linear interpolieren
    var result = bloom_new(in.texcoord);
    
    // 2. Exposure-basiertes Tonemapping: result = 1.0 - exp(-result * exposure)
    result = vec3f(1.0) - exp(-result * uniforms.exposure);
    
    // 3. Gamma-Korrektur (Gamma = 2.2)
    const gamma = 2.2;
    result = pow(result, vec3f(1.0 / gamma));
    
    return vec4f(result, 1.0);
}