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

@group(0) @binding(0) var smplr: sampler;
@group(0) @binding(1) var sceneTexture: texture_2d<f32>;
@group(0) @binding(2) var bloomTexture: texture_2d<f32>;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let sceneColor = textureSample(sceneTexture, smplr, in.texcoord).rgb;
    let bloomColor = textureSample(bloomTexture, smplr, in.texcoord).rgb;

    // Lineare Addition (Mischung aus scharfer Szene und weichem Bloom)
    let intensity = 0.04; // Stellschraube für die Stärke des Effekts
    var mixedColor = sceneColor + bloomColor * intensity;

    // Wichtig bei HDR: Tonemapping & Gamma-Korrektur
    mixedColor = mixedColor / (mixedColor + vec3<f32>(1.0)); // Einfaches Reinhard-Tonemapping
    let finalColor = pow(mixedColor, vec3<f32>(1.0 / 2.2)); // Gamma-Korrektur

    return vec4<f32>(finalColor, 1.0);
}