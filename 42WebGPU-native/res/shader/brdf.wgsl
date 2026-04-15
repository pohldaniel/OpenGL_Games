struct VertexOutput {
	@builtin(position) position : vec4<f32>, 
	@location(0) uv : vec2<f32>,
}

@vertex 
fn vs_main(@builtin(vertex_index) vertex_index : u32) ->VertexOutput {
    var output : VertexOutput;
    let x           = f32((vertex_index << 1u) & 2u);
    let y           = f32(vertex_index & 2u);
    output.position = vec4<f32>(x * 2.0 - 1.0, y * 2.0 - 1.0, 0.0, 1.0);
    output.uv       = vec2<f32>(x, 1.0 - y);
    return output;
}

const PI : f32 = 3.1415926535897932384626433832795;

fn radical_inverse_vdc(bits_in : u32) ->f32 {
    var bits = bits_in;
    bits     = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return f32(bits) * 2.3283064365386963e-10;
}

fn hammersley(i : u32, n : u32) ->vec2<f32> {
    return vec2<f32>(f32(i) / f32(n), radical_inverse_vdc(i));
}

fn importance_sample_ggx(xi : vec2<f32>, n : vec3<f32>, roughness : f32) ->vec3<f32> {
    let a         = roughness * roughness;
    let phi       = 2.0 * PI * xi.x;
    let cos_theta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    let sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    let h = vec3<f32>(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);

    let up = select(vec3<f32>(1.0, 0.0, 0.0), vec3<f32>(0.0, 0.0, 1.0), abs(n.z) < 0.999);
    let tangent   = normalize(cross(up, n));
    let bitangent = cross(n, tangent);

    return normalize(tangent * h.x + bitangent * h.y + n * h.z);
}

fn geometry_schlick_ggx(n_dot_v : f32, roughness : f32) ->f32 {
    let a = roughness;
    let k = (a * a) / 2.0;
    return n_dot_v / (n_dot_v * (1.0 - k) + k);
}

fn geometry_smith(n : vec3<f32>, v : vec3<f32>, l : vec3<f32>, roughness : f32) ->f32 {
    let n_dot_v = max(dot(n, v), 0.0);
    let n_dot_l = max(dot(n, l), 0.0);
    let ggx2    = geometry_schlick_ggx(n_dot_v, roughness);
    let ggx1    = geometry_schlick_ggx(n_dot_l, roughness);
    return ggx1 * ggx2;
}

fn integrate_brdf(n_dot_v : f32, roughness : f32) ->vec2<f32> {
    let v = vec3<f32>(sqrt(1.0 - n_dot_v * n_dot_v), 0.0, n_dot_v);
    var a = 0.0;
     var b = 0.0;
    let n = vec3<f32>(0.0, 0.0, 1.0);

    const SAMPLE_COUNT = 1024u;
    for (var i = 0u; i < SAMPLE_COUNT; i++) {
        let xi = hammersley(i, SAMPLE_COUNT);
        let h  = importance_sample_ggx(xi, n, roughness);
        let l  = normalize(2.0 * dot(v, h) * h - v);

        let n_dot_l = max(l.z, 0.0);
        let n_dot_h = max(h.z, 0.0);
        let v_dot_h = max(dot(v, h), 0.0);

        if (n_dot_l > 0.0) {
            let g     = geometry_smith(n, v, l, roughness);
            let g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
            let fc    = pow(1.0 - v_dot_h, 5.0);

            a += (1.0 - fc) * g_vis;
            b += fc * g_vis;
        }
    }

    return vec2<f32>(a / f32(SAMPLE_COUNT), b / f32(SAMPLE_COUNT));
}

@fragment 
fn fs_main(input : VertexOutput)->@location(0) vec4<f32> {
    let integrated_brdf = integrate_brdf(input.uv.x, 1.0 - input.uv.y);
    return vec4<f32>(integrated_brdf, 0.0, 0.0);
}