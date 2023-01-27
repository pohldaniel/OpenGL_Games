#version 430 core

layout (local_size_x =8, local_size_y = 8, local_size_z = 8) in;
layout(binding=0, rgba16f) restrict readonly uniform image3D currentTex;
layout(binding=1, rgba16f) restrict writeonly uniform image3D resultImage;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_invModelView;

uniform int steps = 128;
//uniform float stepsize = 0.1;
uniform float brightness = 1.0;
uniform float density = 1.0;
uniform float threshold = 0.99;
uniform vec3 boxMin = vec3(-1.0, -1.0, -1.0);
uniform vec3 boxMax = vec3(1.0, 1.0, 1.0);

uniform int constantWidth = 128, constantHeight = 128;
uniform float constantAngle = 45.0, constantNCP = 1.0;

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

void main(){
	
	ivec3 dims = imageSize(resultImage);
    ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);
	
	if(pixel_coords.x < dims.x && pixel_coords.y < dims.y && pixel_coords.z < dims.z) {
        ivec3 dims = imageSize(resultImage);
		ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);
		
		vec3 pos = vec3(float(pixel_coords.x) / dims.x, float(pixel_coords.y)/ dims.y, float(pixel_coords.z)/ dims.z) * 2 - vec3(1.0, 1.0, 1.0);
		pos = (u_projection * u_modelView  * vec4(pos, 1.0)).xyz;
		
		vec3 eyerayo = (u_invModelView * vec4(0, 0, 0, 1)).xyz;
		vec3 eyerayd = normalize(pos - eyerayo);
		
		// calculate ray intersection with bounding box
		float tnear, tfar;
		bool hit = IntersectBox(eyerayo, eyerayd, boxMin, boxMax, tnear, tfar);
		vec4 c = vec4(0);
		
		if(hit){
			if (tnear < 0.0) tnear = 0.0;
			
		
			// calculate intersection points
			vec3 Pnear = eyerayo + eyerayd * tnear;
			vec3 Pfar = eyerayo + eyerayd * tfar;
			// convert to texture space
			Pnear = Pnear * 0.5 + 0.5;
			Pfar = Pfar * 0.5 + 0.5;
			
			// march along ray, accumulating color
			float stepsize = 1.7 / steps;
			

			#ifdef FRONT_TO_BACK
				// use front-to-back rendering
				vec3 P = Pnear;
				vec3 Pstep = eyerayd * stepsize;
			#else
				// use back-to-front rendering
				vec3 P = Pfar;
				vec3 Pstep = -eyerayd * stepsize;
			#endif
			
			for(int i=0; i < steps; i++) {
				vec4 s = imageLoad(currentTex, pixel_coords);

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
		}
		c.rgb *= brightness;
		imageStore(resultImage, pixel_coords, c);
	}   
}