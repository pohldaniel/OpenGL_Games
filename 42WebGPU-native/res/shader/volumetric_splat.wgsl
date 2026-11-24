struct SplatUniforms {
    centre : vec3<f32>,
    radius : f32,
    color  : vec4<f32>,
};

@group(0) @binding(0) var srcTex : texture_3d<f32>;
@group(0) @binding(1) var dstTex : texture_storage_3d<rgba16float, write>;
@group(0) @binding(2) var<uniform> params : SplatUniforms;

@compute @workgroup_size(4, 4, 4)
fn cs_splat(@builtin(global_invocation_id) id : vec3<u32>) {
     let dims = textureDimensions(dstTex);
    if (any(id >= dims)) { return; }

    let texCoord = (vec3<f32>(id) + 0.5) / vec3<f32>(dims);
    let d = length(texCoord - params.centre);

    let prev = textureLoad(srcTex, vec3<i32>(id), 0);
    let splat = (1.0 - smoothstep(0.0, params.radius, d)) * params.color;

    textureStore(dstTex, vec3<i32>(id), prev + splat);
}