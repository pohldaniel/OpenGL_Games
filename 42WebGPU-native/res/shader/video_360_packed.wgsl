
const position = array(
    vec2(-1.0, -1.0), vec2(3.0f,  -1.0), vec2(-1.0, 3.0)
);

struct CameraUniforms {
    viewMatrix: mat4x4<f32>,
    fovAndAspect: vec2<f32>,    
};

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(0) @binding(1) var videoSampler: sampler;
@group(0) @binding(2) var videoTextureY: texture_2d<f32>;

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
	@location(0) uv : vec2f,
    @location(1) rayDirection: vec3<f32>
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
  var output : VertexOutput;
  var pos = vec4f(position[vertexIndex], 0.0, 1.0);
  output.uv       = position[vertexIndex] * vec2f(0.5, -0.5) + vec2f(0.5);
  output.position = vec4<f32>(pos.x, pos.y, 0.0, 1.0);
  
  var rawRay = vec3<f32>(pos.x * camera.fovAndAspect.y * camera.fovAndAspect.x, pos.y * camera.fovAndAspect.x, 1.0);
  var rotatedRay = camera.viewMatrix * vec4<f32>(rawRay, 0.0);
  output.rayDirection = normalize(rotatedRay.xyz);
  
  return output;
}

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4<f32> {
    var dir = normalize(input.rayDirection);

    // --- 1. KUGEL-PROJEKTION (Equirektangular) ---
    const PI = 3.14159265359;
    const TWO_PI = 6.28318530718;

    var longitude = atan2(dir.x, dir.z); 
    var latitude = asin(dir.y);

    var video_u = (longitude + PI) / TWO_PI;
    var video_v = (latitude + (PI / 2.0)) / PI;
    video_v = 1.0 - video_v;

    // --- 2. SCHNELLER LINEARER ATLAS-ZUGRIFF ---
    
    // Y-Bereich (Helligkeit): Volle Breite, obere 66.6%
    let y_uv = vec2<f32>(video_u, video_v * (2.0 / 3.0));
    let y = textureSample(videoTextureY, videoSampler, y_uv).r;

    // U-Bereich (Chroma Cb): Mittlerer Block (66.6% - 83.3%), linke Hälfte (0.0 - 0.5)
    let u_uv = vec2<f32>(video_u * 0.5, (2.0 / 3.0) + (video_v * (1.0 / 6.0)));
    let u = textureSample(videoTextureY, videoSampler, u_uv).r;

    // V-Bereich (Chroma Cr): Unterster Block (83.3% - 100%), linke Hälfte (0.0 - 0.5)
    let v_uv = vec2<f32>(video_u * 0.5, (5.0 / 6.0) + (video_v * (1.0 / 6.0)));
    let v = textureSample(videoTextureY, videoSampler, v_uv).r;

    // --- 3. SPEZIFISCHE SMPTE170M (BT.601) LIMITED-RANGE MATRIX ---
    let y_norm = y - 0.062745;
    let u_norm = u - 0.501961;
    let v_norm = v - 0.501961;

    let r = 1.164384 * y_norm + 1.596027 * v_norm;
    let g = 1.164384 * y_norm - 0.391762 * u_norm - 0.812968 * v_norm;
    let b = 1.164384 * y_norm + 2.017232 * u_norm;

    return vec4<f32>(r,g,b, 1.0);
}