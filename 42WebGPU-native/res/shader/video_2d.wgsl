const position = array(
    vec2(-1.0, -1.0), vec2(3.0f,  -1.0), vec2(-1.0, 3.0)
);

struct VertexOutput {
  @builtin(position) position : vec4f, 
  @location(0) uv : vec2f,
};

@vertex 
fn vs_main(@builtin(vertex_index) vertexIndex : u32) -> VertexOutput {
  var output : VertexOutput;
  output.position = vec4f(position[vertexIndex], 0.0, 1.0);
  output.uv       = position[vertexIndex] * vec2f(0.5, -0.5) + vec2f(0.5);
  return output;
}

@group(0) @binding(0) var smplr : sampler;
@group(0) @binding(1) var texture : texture_2d<f32>;

@fragment 
fn fs_main(in: VertexOutput)->@location(0) vec4f {
  return textureSample(texture, smplr, in.uv);
  //return vec4f(in.uv, 0.0, 1.0);
}