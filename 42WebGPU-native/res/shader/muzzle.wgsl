struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texcoord: vec2f,
};

struct SpriteInstance {
    position: vec3f,
    scale: vec2f,
    currentFrame: f32
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

struct FrameInfo {
    colRow: vec2u,
    frameSize: vec2f,
};

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> info: FrameInfo;
@group(0) @binding(2) var<storage, read> instances: array<SpriteInstance>;
@group(0) @binding(3) var smplr: sampler;
@group(0) @binding(4) var texture: texture_2d<f32>;

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {

	var out: VertexOutput;

    var positions = array<vec2f, 4>(
        vec2f(-0.5,  0.5),
        vec2f(-0.5, -0.5),
        vec2f( 0.5,  0.5),
        vec2f( 0.5, -0.5)
    );
    
    var uvs = array<vec2f, 4>(
        vec2f(0.0, 0.0),
        vec2f(0.0, 1.0),
        vec2f(1.0, 0.0),
        vec2f(1.0, 1.0)
    );
	
	out.position = uniforms.projection * uniforms.view * uniforms.model * vec4f(positions[vertexIndex], 0.0, 1.0);
	
	let instance = instances[0];
	
	let frameIndex = u32(instance.currentFrame);
    //let sizePerFrame = vec2f(1.0 / 6.0, 1.0); 
	let cols = u32(info.colRow.x);
    let uvOffset = vec2f(f32(frameIndex % cols), f32(frameIndex / cols)) * info.frameSize;
    out.texcoord = uvOffset + (uvs[vertexIndex] * info.frameSize);
	
	 return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	//return vec4f(in.texcoord, 0.0, 1.0);
	return textureSample(texture, smplr, in.texcoord);
}