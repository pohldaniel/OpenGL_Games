struct SpriteInstance {
    transform: mat4x4<f32>,
    color: vec4<f32>,
    textureRect: vec4<f32>, // [u, v, width, height]
    flipAndTile: vec4<f32>, // [flipped, tileX, tileY]
}

struct Uniforms {
	projection: mat4x4<f32>
}                           

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<storage, read> instances: array<SpriteInstance>;
@group(0) @binding(2) var uiSampler: sampler;
@group(0) @binding(3) var uiTextures: texture_2d_array<f32>;


struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) uv: vec2<f32>,
    @location(1) color: vec4<f32>,
    @location(2) @interpolate(flat) textureLayer: u32,
}

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32, @builtin(instance_index) instanceIndex: u32) -> VertexOutput {
    let instance = instances[instanceIndex];

    // Generiere Quad-Ecken für ein 1x1 Standard-Quad: (0,0) bis (1,1)
    // vertexIndex geht von 0 bis 5 (für 2 Triangles)
    var localPos = vec2<f32>(0.0, 0.0);
    var uv = vec2<f32>(0.0, 0.0);

    switch (vertexIndex) {
        case 0u: { localPos = vec2<f32>(0.0, 0.0); uv = vec2<f32>(0.0, 0.0); }
        case 1u: { localPos = vec2<f32>(1.0, 0.0); uv = vec2<f32>(1.0, 0.0); }
        case 2u: { localPos = vec2<f32>(0.0, 1.0); uv = vec2<f32>(0.0, 1.0); }
        case 3u: { localPos = vec2<f32>(0.0, 1.0); uv = vec2<f32>(0.0, 1.0); }
        case 4u: { localPos = vec2<f32>(1.0, 0.0); uv = vec2<f32>(1.0, 0.0); }
        case 5u: { localPos = vec2<f32>(1.0, 1.0); uv = vec2<f32>(1.0, 1.0); }
        default: {}
    }

    // Transformiere UVs auf den richtigen Ausschnitt im Spritesheet/Sub-Bild
    let finalUV = instance.textureRect.xy + (uv * instance.textureRect.zw);

    var output: VertexOutput;
    output.position = uniforms.projection * instance.transform * vec4<f32>(localPos, 0.0, 1.0);
    output.uv = finalUV;
    output.color = instance.color;
    output.textureLayer = u32(instance.flipAndTile.x);

    return output;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
   // 1. Sample die R8-Textur (Wert landet komplett im .r Kanal)
    let sampledColor = textureSample(uiTextures, uiSampler, in.uv, in.textureLayer);
    
    // 2. Extrahiere die Schrift-Intensität aus dem Rot-Kanal
    let alphaMask = sampledColor.r; 
    
    // 3. Erzeuge die finale Textfarbe: 
    // RGB kommt zu 100% aus der Wunschfarbe (in.color.rgb)
    // Alpha berechnet sich aus Wunsch-Alpha multipliziert mit der Glyphen-Maske
    let textAlpha = in.color.a * alphaMask;
    
    return vec4<f32>(in.color.rgb, textAlpha);
}