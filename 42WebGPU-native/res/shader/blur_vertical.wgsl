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
@group(0) @binding(1) var texture: texture_2d<f32>;

const weight = array<f32, 28>(
    0.049835, 0.049448, 0.048304, 0.046456, 0.043987, 0.041004, 0.037631, 
    0.034002, 0.030246, 0.026489, 0.022839, 0.019388, 0.016203, 0.013331, 
    0.010799, 0.008612, 0.006762, 0.005227, 0.003978, 0.00298,  0.002199, 
    0.001597, 0.01142,  0.000804, 0.000557, 0.00038,  0.000255, 0.000169
);

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {

    let baseSample = textureSample(texture, smplr, in.texcoord).rgb;
    let tex_offset = vec2f(1.0) / vec2f(textureDimensions(texture));
    
    var result = baseSample * weight[0];
    
    for (var i = 1; i < 28; i++) {
        let offset = vec2f(0.0, tex_offset.y * f32(i));
        result += textureSample(texture, smplr, in.texcoord + offset).rgb * weight[i];
        result += textureSample(texture, smplr, in.texcoord - offset).rgb * weight[i];
    }
	return vec4f(result, 1.0);
}