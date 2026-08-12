
const position = array(
    vec2(-1.0, -1.0), vec2(3.0f,  -1.0), vec2(-1.0, 3.0)
);

struct CameraUniforms {
    viewMatrix: mat4x4<f32>,
    fovAndAspect: vec2<f32>,    
};

@group(0) @binding(0) var<uniform> camera: CameraUniforms;
@group(0) @binding(1) var videoSampler: sampler;
@group(0) @binding(2) var videoTexture: texture_2d<f32>;

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

    // --- MATHEMATISCHE KUGEL-PROJEKTION (Equirektangular) ---
    // Wir wandeln den 3D-Richtungsvektor (x,y,z) in 2D-Kugelkoordinaten (U,V) um,
    // um die flache Videokarte perfekt verzerrungsfrei auf eine Kugel zu mappen.
    
    // Pi-Konstanten für die Kreis-Berechnung
    const PI = 3.14159265359;
    const TWO_PI = 6.28318530718;

    // Längengrad (Horizontaler Winkel um die Y-Achse) von -PI bis +PI
    var longitude = atan2(dir.x, dir.z); 
    
    // Breitengrad (Vertikaler Winkel) von -PI/2 bis +PI/2
    var latitude = asin(dir.y);

    // Winkel in standardisierte Texturkoordinaten (0.0 bis 1.0) umrechnen
    var u = (longitude + PI) / TWO_PI;
    var v = (latitude + (PI / 2.0)) / PI;

	//u = 1.0 -u;
    // Da Texturen von oben nach unten gezählt werden, invertieren wir V für WebGPU
    v = 1.0 - v;

    // Pixelfarbe aus dem neuen FFmpeg-RGBA-Buffer via WebGPU Sampler holen
    var color = textureSample(videoTexture, videoSampler, vec2<f32>(u, v));

     return color;
}