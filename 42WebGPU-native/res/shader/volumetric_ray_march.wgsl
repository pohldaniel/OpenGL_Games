struct RayMarchUniforms {
    u_projection   : mat4x4<f32>,
    u_modelView    : mat4x4<f32>,
    u_invModelView : mat4x4<f32>,
    boxMin         : vec3<f32>,
    density        : f32,
    boxMax         : vec3<f32>,
    brightness     : f32,
    steps          : i32,
    threshold      : f32,
    _pad           : vec2<f32>, // Padding auf 16-Byte-Vielfaches
};

@group(0) @binding(0) var<uniform> u : RayMarchUniforms;
@group(0) @binding(1) var u_texture : texture_3d<f32>;
@group(0) @binding(2) var u_sampler : sampler;

struct VertexInput {
    @location(0) position  : vec3<f32>
};

struct VertexOutput {
    @builtin(position) clipPos : vec4<f32>,
    @location(0) v_texCoord    : vec3<f32>,
    @location(1) v_eyerayo     : vec3<f32>,
    @location(2) v_eyerayd     : vec3<f32>,
};

@vertex
fn vs_main(in : VertexInput) -> VertexOutput {
    var out : VertexOutput;
    out.clipPos    = u.u_projection * u.u_modelView * vec4<f32>(in.position, 1.0);
    out.v_texCoord = in.position * 0.5 + 0.5;
    out.v_eyerayo  = (u.u_invModelView * vec4<f32>(0.0, 0.0, 0.0, 1.0)).xyz;
    out.v_eyerayd  = in.position - out.v_eyerayo;
    return out;
}

struct BoxHit {
    hit   : bool,
    tnear : f32,
    tfar  : f32,
};

fn IntersectBox(o : vec3<f32>, d : vec3<f32>, boxmin : vec3<f32>, boxmax : vec3<f32>) -> BoxHit {
    let invR = 1.0 / d;
    let tbot = invR * (boxmin - o);
    let ttop = invR * (boxmax - o);

    let tmin = min(ttop, tbot);
    let tmax = max(ttop, tbot);

    let t0 = max(tmin.xx, tmin.yz);
    let largest_tmin = max(t0.x, t0.y);
    let t1 = min(tmax.xx, tmax.yz);
    let smallest_tmax = min(t1.x, t1.y);

    var res : BoxHit;
    res.hit   = (largest_tmin <= smallest_tmax);
    res.tnear = largest_tmin;
    res.tfar  = smallest_tmax;
    return res;
}

@fragment
fn fs_main(in : VertexOutput) -> @location(0) vec4<f32> {
    let stepsize = 1.7 / f32(u.steps);
    let eyerayd  = normalize(in.v_eyerayd);

    let boxHit = IntersectBox(in.v_eyerayo, eyerayd, u.boxMin, u.boxMax);
    if (!boxHit.hit) {
        discard;
    }

    var tnear = max(boxHit.tnear, 0.0);
    let tfar  = boxHit.tfar;

    // Schnittpunkte in Texturraum [0..1] umrechnen
    var P = (in.v_eyerayo + eyerayd * tnear) * 0.5 + 0.5;
    // Hinweis: Pstep im Texturraum (0.5 Skalierung wie im Original!)
    let Pstep = eyerayd * stepsize * 0.5; 

    var c = vec4<f32>(0.0);

    for (var i : i32 = 0; i < u.steps; i++) {
        // textureSampleLevel statt textureSample wegen nicht-uniformem break/discard
        var s = textureSampleLevel(u_texture, u_sampler, P, 0.0);

        s = abs(s);
        s.a = clamp(s.a, 0.0, 1.0);
        s.a *= u.density;

        // Front-to-Back Blending
        s = vec4<f32>(s.rgb * s.a, s.a);
        c = (1.0 - c.a) * s + c;

        if (c.a > u.threshold) {
            break;
        }

        P += Pstep;
    }

    return vec4<f32>(c.rgb * u.brightness, 1.0);
}