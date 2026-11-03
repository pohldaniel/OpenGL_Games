 struct VertexOutput {
   @builtin(position) pos: vec4<f32>,
   @location(0) tUV: vec2<f32>,
   @location(1) vAlpha: f32,
 };

 @vertex
 fn vs_main(
   @location(0) position: vec3<f32>,
   @location(1) scale: vec3<f32>,
   @location(2) alpha: f32,
   @location(3) a_pos: vec4<f32>,
   @location(4) a_uv: vec2<f32>,
 ) -> VertexOutput {
   var output: VertexOutput;

   let ratio: f32 = 976.0 / 1920.0;
   let scaleMTX: mat4x4<f32> = mat4x4<f32>(
     vec4<f32>(scale.x, 0.0, 0.0, 0.0),
     vec4<f32>(0.0, scale.y, 0.0, 0.0),
     vec4<f32>(0.0, 0.0, scale.z, 0.0),
     vec4<f32>(position, 1.0)
   );
   output.pos = scaleMTX * vec4<f32>(a_pos.x, a_pos.y / ratio, a_pos.z, 1.0);
   output.tUV = a_uv;
   output.vAlpha = alpha;

   return output;
 }
 
@group(0) @binding(0) var uSampler: sampler;
@group(0) @binding(1) var uTexture: texture_2d<f32>;

struct FragmentOutput {
  @location(0) outColor: vec4<f32>,
};

@fragment
fn fs_main(
  @location(0) tUV: vec2<f32>,
  @location(1) vAlpha: f32,
) -> FragmentOutput {
  var output: FragmentOutput;

  output.outColor = textureSample(uTexture, uSampler, tUV);
  output.outColor = vec4<f32>(mix(output.outColor.rgb, vec3<f32>(vAlpha, 0.0, 0.0), 1.0 - vAlpha), output.outColor.a);
  output.outColor.a *= vAlpha;

  return output;
}