struct VertexIn {
   @location(0) pos: vec3f,
   @location(1) uv: vec2f,
}

struct VertexOut {
   @builtin(position) pos: vec4f,
   @location(0) uv: vec2f,
}

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>,
	lightVP:mat4x4<f32>,
	shadow: mat4x4<f32>,
	lightPos: vec3<f32>
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;

@vertex
fn vs_main(in: VertexIn) -> VertexOut {
   var out: VertexOut;
   out.pos = uniforms.projection * uniforms.view * vec4f(in.pos, 1.0);
   out.uv = in.uv * 200.0;
   return out;
}

struct GridArgs {
   lineColor: vec4f,
   baseColor: vec4f,
   lineWidth: vec2f
};

@group(1) @binding(0) var<uniform> gridArgs: GridArgs;

fn PristineGrid(uv: vec2f, lineWidth: vec2f) -> f32 {
   let uvDDXY = vec4f(dpdx(uv), dpdy(uv));
   let uvDeriv = vec2f(length(uvDDXY.xz), length(uvDDXY.yw));
   let invertLine: vec2<bool> = lineWidth > vec2f(0.5);
   let targetWidth: vec2f = select(lineWidth, 1 - lineWidth, invertLine);
   let drawWidth: vec2f = clamp(targetWidth, uvDeriv, vec2f(0.5));
   let lineAA: vec2f = uvDeriv * 1.5;
   var gridUV: vec2f = abs(fract(uv) * 2.0 - 1.0);
   gridUV = select(1 - gridUV, gridUV, invertLine);
   var grid2: vec2f = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
   grid2 *= saturate(targetWidth / drawWidth);
   grid2 = mix(grid2, targetWidth, saturate(uvDeriv * 2.0 - 1.0));
   grid2 = select(grid2, 1.0 - grid2, invertLine);
   return mix(grid2.x, 1.0, grid2.y);
}

 
@fragment
fn fs_main(in: VertexOut) -> @location(0) vec4f {
   var grid = PristineGrid(in.uv, gridArgs.lineWidth);
   return mix(gridArgs.baseColor, gridArgs.lineColor, grid * gridArgs.lineColor.a);
}