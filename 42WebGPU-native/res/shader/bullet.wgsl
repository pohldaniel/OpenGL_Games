struct VertexInput {
	@builtin(instance_index) instanceIndex : u32,
	@location(0) position: vec3f,
	@location(1) texcoord: vec2f
};

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(1) texcoord: vec2f,
	@location(2) normal: vec3f,
	@location(3) color: vec4f
};

struct Rot {
	rot : array<vec4f, 4000>,
};

struct Offset {
	offset : array<vec4f, 4000>
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

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> rots : Rot;
@group(0) @binding(2) var<uniform> offsets : Offset;
@group(0) @binding(3) var smplr: sampler;
@group(0) @binding(4) var texture: texture_2d<f32>;

fn hamiltonProduct(q1: vec4f,  q2: vec4f) -> vec4f {
  return vec4f(
    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

fn rotateByQuat(v: vec3f , q: vec4f ) -> vec3f {
  let qPrime = vec4f(-q.x, -q.y, -q.z, q.w);
  let vPrime = hamiltonProduct(hamiltonProduct(q, vec4f(v.x, v.y, v.z, 0.0)), qPrime);
  return vec3f(vPrime.x, vPrime.y, vPrime.z);
}

fn rotateByQuat2(v: vec3f , q: vec4f ) -> vec3f {
  let u = vec3f(q.x, q.y, q.z);
  let s = q.w;
  
  return 2.0f * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0f * s *  cross(v, u);
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
	var out: VertexOutput;
	let rotatedInPos = rotateByQuat(in.position, rots.rot[in.instanceIndex]);
	
	let posOffset = offsets.offset[in.instanceIndex].xyz;
	
	out.position = uniforms.projection * uniforms.view * vec4f(rotatedInPos + posOffset, 1.0);	
	out.texcoord = in.texcoord;
	return out;
}


@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	//return vec4f(in.texcoord, 0.0, 1.0);
	return textureSample(texture, smplr, in.texcoord);
}