struct VertexInput {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@builtin(vertex_index) vertexIndex : u32
};

struct Uniforms {
    projection: mat4x4<f32>,
	view: mat4x4<f32>,
	env: mat4x4<f32>,
    model: mat4x4<f32>,    
	normal: mat4x4<f32>,
	color: vec4<f32>,
	camPos: vec3<f32>,
	lightVP: mat4x4<f32>,
	shadow: mat4x4<f32>,
	lightPos: vec3<f32>
};

struct Frame {
   viewProjectionMatrix : mat4x4f,
   invViewProjectionMatrix : mat4x4f,
   pickCoord : vec2u,
   pickedPrimitive : u32,
}

@group(0) @binding(0) var<uniform> uniforms : Uniforms;
@group(0) @binding(1) var<uniform> frame : Frame;

struct VertexOutput {
   @builtin(position) position : vec4f,
   @location(0) normal : vec3f,
   @location(1) @interpolate(flat) vertexIndex : u32,
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
   var out : VertexOutput;
   let worldPosition = (uniforms.model * vec4(in.position, 1.0)).xyz;
   out.position = frame.viewProjectionMatrix * vec4(worldPosition, 1.0);
   out.normal = normalize((uniforms.normal * vec4(in.normal, 1.0)).xyz);
   out.vertexIndex = in.vertexIndex;
   return out;
}

struct PassOutput {
  @location(0) primitive : u32,
  @location(1) color : vec4f, 
}

@fragment 
fn fs_main(in: VertexOutput) -> PassOutput {
  // Compute primitive index from vertex index (3 vertices per triangle)
  let primIndex = in.vertexIndex / 3u;

  // Very simple N-dot-L lighting model
  let lightDirection = normalize(vec3f(4, 10, 6));
  let light        = dot(normalize(in.normal), lightDirection) * 0.5 + 0.5;
  let surfaceColor = vec4f(0.8, 0.8, 0.8, 1.0);

  var output : PassOutput;

  // Highlight the primitive if it's the selected one, otherwise shade
  // normally.
  if (primIndex + 1 == frame.pickedPrimitive) {
    output.color = vec4f(1.0, 1.0, 0.0, 1.0);
  }
  else {
    output.color = vec4f(surfaceColor.xyz * light, surfaceColor.a);
  }

  // Adding one to each primitive index so that 0 can mean "nothing picked"
  output.primitive = primIndex + 1;
  return output;
}