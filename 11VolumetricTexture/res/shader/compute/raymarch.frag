#version 410 core

uniform sampler3D u_texture;

in vec3 v_texCoord;
in vec3 v_eyerayo;
in vec3 v_eyerayd;

uniform int steps = 128;
//uniform float stepsize = 0.1;
uniform float brightness = 1.0;
uniform float density = 1.0;
uniform float threshold = 0.99;
uniform vec3 boxMin = vec3(-1.0, -1.0, -1.0);
uniform vec3 boxMax = vec3(1.0, 1.0, 1.0);

out vec4 color;

bool IntersectBox(vec3 o, vec3 d, vec3 boxmin, vec3 boxmax, out float tnear, out float tfar) {

	// compute intersection of ray with all six bbox planes
    vec3 invR = 1.0 / d;
    vec3 tbot = invR * (boxmin.xyz - o);
    vec3 ttop = invR * (boxmax.xyz - o);

    // re-order intersections to find smallest and largest on each axis
    vec3 tmin = min (ttop, tbot);
    vec3 tmax = max (ttop, tbot);
	
	// find the largest tmin and the smallest tmax
    vec2 t0 = max (tmin.xx, tmin.yz);
    float largest_tmin = max (t0.x, t0.y);
    t0 = min (tmax.xx, tmax.yz);
    float smallest_tmax = min (t0.x, t0.y);

	 // check for hit
    bool hit;
    if ((largest_tmin > smallest_tmax)) 
        hit = false;
    else
        hit = true;

    tnear = largest_tmin;
    tfar = smallest_tmax;

    return hit;
}

#define FRONT_TO_BACK

void main(void){
	float stepsize = 1.7 / steps;

    vec3 eyerayd = normalize(v_eyerayd);
	
	// calculate ray intersection with bounding box
    float tnear, tfar;
    bool hit = IntersectBox(v_eyerayo, eyerayd, boxMin, boxMax, tnear, tfar);
    if (!hit) discard;
    if (tnear < 0.0) tnear = 0.0;

    // calculate intersection points
    vec3 Pnear = v_eyerayo + eyerayd * tnear;
    vec3 Pfar = v_eyerayo + eyerayd * tfar;
    // convert to texture space
    Pnear = Pnear * 0.5 + 0.5;
    Pfar = Pfar * 0.5 + 0.5;
	
	// march along ray, accumulating color
    vec4 c = vec4(0);

#ifdef FRONT_TO_BACK
    // use front-to-back rendering
    vec3 P = Pnear;
    vec3 Pstep = eyerayd * stepsize;
#else
    // use back-to-front rendering
    vec3 P = Pfar;
    vec3 Pstep = -eyerayd * stepsize;
#endif


	for(int i=0; i<steps; i++) {
        vec4 s = texture(u_texture, P);

        s = abs(s);
        s.a = clamp(s.a, 0, 1);
        s.a *= density;

#ifdef FRONT_TO_BACK
        s.rgb *= s.a;   // premultiply alpha
        c = (1 - c.a)*s + c;
        // early exit if opaque
        if (c.a > threshold)
            break;
#else
        c = lerp(c, s, s.a);
#endif

        P += Pstep;
    }
    c.rgb *= brightness;

	color = c;
}
