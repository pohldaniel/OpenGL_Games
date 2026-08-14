
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
@group(0) @binding(3) var videoTextureUV: texture_2d<f32>;

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

    // --- 2. HARDWARE-ZEILEN-ZUGRIFF (Kein Atlas mehr!) ---
    let uv_coords = vec2<f32>(video_u, video_v);

    // Y-Wert direkt aus der Helligkeits-Textur lesen (Rot-Kanal)
    let y = textureSample(videoTextureY, videoSampler, uv_coords).r;

    // U und V liegen zusammen in der biplanaren UV-Textur
    // Rot-Kanal (.r) = U (Chroma Cb)
    // Grün-Kanal (.g) = V (Chroma Cr)
    let uv = textureSample(videoTextureUV, videoSampler, uv_coords).rg;
    let u = uv.x;
    let v = uv.y;

    // --- 3. HARDWARE BT.709 LIMITED-RANGE MATRIX (Standard für HD/4K) ---
    // Korrektur der Pegel (Y-Offset: 16/255 = 0.062745, UV-Offset: 128/255 = 0.501961)
    let y_norm = y - 0.062745;
    let u_norm = u - 0.501961;
    let v_norm = v - 0.501961;

    // Offizielle BT.709 Konvertierungsmatrix für kristallklare 4K Farben
    let r = 1.164383 * y_norm + 1.792741 * v_norm;
    let g = 1.164383 * y_norm - 0.213249 * u_norm - 0.532909 * v_norm;
    let b = 1.164383 * y_norm + 2.112402 * u_norm;

    // Begrenzung (Clamping), um Farb-Artefakte im HDR/SDR-Übergang zu vermeiden
    let rgb = clamp(vec3<f32>(r, g, b), vec3<f32>(0.0), vec3<f32>(1.0));

    return vec4<f32>(rgb, 1.0);
}