struct WaveUniforms {
    voxelSize : vec3<f32>,
    damping   : f32,
};

@group(0) @binding(0) var currentTex  : texture_3d<f32>;
@group(0) @binding(1) var prevTex     : texture_3d<f32>;
@group(0) @binding(2) var resultImage : texture_storage_3d<rgba16float, write>;
@group(0) @binding(3) var<uniform> params : WaveUniforms;

fn loadVoxel(tex : texture_3d<f32>, coord : vec3<i32>, dims : vec3<i32>) -> vec4<f32> {
    if (any(coord < vec3<i32>(0)) || any(coord >= dims)) {
        return vec4<f32>(0.0);
    }
    return textureLoad(tex, coord, 0);
}

@compute @workgroup_size(4, 4, 4)
fn cs_waves(@builtin(global_invocation_id) global_id : vec3<u32>) {
    let dims = vec3<i32>(textureDimensions(resultImage));
    let x = vec3<i32>(global_id);

    // Sicherheitscheck, falls Texturgröße kein Vielfaches von 4 ist
    if (any(x >= dims)) {
        return;
    }

    let maxCoord = dims - vec3<i32>(1);

    // 6-Nachbarn-Laplace-Filter (mit Clamp an den Grenzen)
    var smoothed = loadVoxel(currentTex, x + vec3<i32>( 1,  0,  0), dims);
    smoothed    += loadVoxel(currentTex, x + vec3<i32>(-1,  0,  0), dims);
    smoothed    += loadVoxel(currentTex, x + vec3<i32>( 0,  1,  0), dims);
    smoothed    += loadVoxel(currentTex, x + vec3<i32>( 0, -1,  0), dims);
    smoothed    += loadVoxel(currentTex, x + vec3<i32>( 0,  0,  1), dims);
    smoothed    += loadVoxel(currentTex, x + vec3<i32>( 0,  0, -1), dims);
    smoothed    /= 6.0;

    let prev = textureLoad(prevTex, x, 0);

    let pixel = (smoothed * 2.0 - prev) * params.damping;
    textureStore(resultImage, x, pixel);
}