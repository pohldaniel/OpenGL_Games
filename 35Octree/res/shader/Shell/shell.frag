#version 410 core
#extension GL_EXT_gpu_shader4_1 : enable

uniform sampler2D u_texture;
uniform sampler2D u_bump;
uniform vec3 u_lightPos;

flat in mat3 geomPositionFront;
flat in mat3 geomNormalFront;
flat in mat3 geomTangentFront;
flat in mat3 geomTexCoordFront;
flat in vec3 geomWFront;

noperspective in vec3 geomBarycentric;

in vec3 geomPosition;
in vec3 geomNormal;
in vec3 geomTangent;
in vec3 geomTexCoord;

out vec4 fragColor;

vec3 vcdiv(vec3 a, vec3 b){
	return vec3(a.x/b.x, a.y/b.y, a.z/b.z);
}

void main(){

	const vec3 one = vec3(1.0, 1.0, 1.0);

	vec3 bzfv = vcdiv(geomBarycentric,geomWFront);
	float idobzfv = 1.0/dot(one,bzfv);

	vec3 p0 = geomPosition;
	vec3 p1 = (geomPositionFront*bzfv)*idobzfv;

	vec3 n0 = geomNormal;
	vec3 n1 = (geomNormalFront*bzfv)*idobzfv;

	vec3 t0 = geomTangent;
	vec3 t1 = (geomTangentFront*bzfv)*idobzfv;

	vec3 tc0 = geomTexCoord;
	vec3 tc1 = (geomTexCoordFront*bzfv)*idobzfv;

	float tl = textureQueryLod(u_bump, tc1.xy).x;
	ivec2 ts = textureSize(u_bump, int(tl));
	int mts = max(ts.x, ts.y);
	vec2 dtc = tc1.xy - tc0.xy;
	float mdtc = max(abs(dtc.x), abs(dtc.y));

    int nsam = max(min(int(mdtc*mts), mts), 1);;
    float step = 1.0 / nsam;
	
    for(int s=0; s<=nsam; ++s){
		vec3 tc = mix(tc1, tc0, s*step);
		vec4 bm = texture(u_bump, tc.xy);
		if(tc.z <= bm.w+0.01){
			vec3 p = mix(p1, p0, s*step);
			vec3 n = mix(n1, n0, s*step);
			vec3 t = mix(t1, t0, s*step);
			vec3 b = cross(n, t);

			vec3 ldir = normalize(u_lightPos - p);
			vec3 nml = normalize(t*bm.x+b*bm.y+n*bm.z);
			float l = max(dot(ldir, nml), 0.0)*max(dot(ldir, n)+0.3,0.0)+0.2;
			vec3 color = texture(u_texture, tc.xy).rgb;
			fragColor.rgb = color * l;
			fragColor.a = 1.0;
			return;
        }
    }
    discard;
}