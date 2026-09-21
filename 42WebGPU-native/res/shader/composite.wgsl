struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texcoord: vec2f,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    // Generiert ein Fullscreen-Quad ohne Vertex-Buffer
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

@group(0) @binding(0) var textureSampler: sampler;
@group(0) @binding(1) var baseTexture: texture_multisampled_2d<f32>;
@group(0) @binding(2) var emissionTexture: texture_2d<f32>;         // VertBlur (1x)
@group(0) @binding(3) var brightTexture: texture_2d<f32>; 

fn CalcBrightness(col: vec3f) -> f32 {
    return (col.x + col.y + col.z) / 3.0;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let textureDims = textureDimensions(baseTexture);
    let texelCoords = vec2i(in.texcoord * vec2f(textureDims));
    
    // 1. Texturen auslesen
    let baseColor = textureLoad(baseTexture, texelCoords, 0).rgb;
    let emissionColor = textureSample(emissionTexture, textureSampler, in.texcoord).rgb;
    let rawBright = textureSample(brightTexture, textureSampler, in.texcoord).rgb;
    
    // 2. Initiale Zuweisung: Nur auf den RGB-Kanälen rechnen!
    var finalRGB = baseColor + (emissionColor * 2.9);
    
    // 3. Scharfer Bright-Core-Zusatz
    let brightness = CalcBrightness(rawBright);
    if (brightness > 0.05) {
        let mult = 1.5;
        var additive = 0.4;
        if (brightness > 0.3) {
            additive = 1.8;
        }
        
        let dynamicOffset = vec3f(2.0 * additive, 0.6 * additive, 0.6 * additive);
        

        finalRGB += (mult * rawBright) + dynamicOffset;
    }
    
    // 4. Clamping (Optional): Verhindert, dass Werte ins Unendliche schießen, 
    // falls kein HDR-Format für die Surface aktiv ist
    //finalRGB = clamp(finalRGB, vec3f(0.0), vec3f(1.0));
    
    // 5. Erst hier den Vektor mit einem sauberen Alpha von 1.0 zusammenbauen
    return vec4f(finalRGB, 1.0);
}