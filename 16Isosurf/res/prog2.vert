#version 410 core

uniform sampler3D volumeTex;
uniform ivec3 SizeMask = ivec3(63, 63, 63);
uniform ivec3 SizeShift = ivec3(0, 6, 12 );

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;
uniform float IsoValue = 0.5;
uniform float time = 0.0;

out vec2 Field;
out	vec4 Color;

out	vec3 v_normal;
out	vec4 v_color;

float flame(sampler3D volumeTex, vec3 p, float time, float noiseFreq = 0.1, float noiseAmp = 0.5, vec3 timeScale = vec3(0, -0.1, 0.0)) {
	float h = p.y*0.5+0.5;
	float r = 0.7 - length(p.xz) * h;

	r += texture(volumeTex, p*noiseFreq + time*timeScale).x * noiseAmp * h;
	return r;
}


void main(void) {
	vec3 Pos;
	Pos.x = float((gl_VertexID >> SizeShift.x) & SizeMask.x) / (SizeMask.x + 1);
	Pos.y = float((gl_VertexID >> SizeShift.y) & SizeMask.y) / (SizeMask.y + 1);
	Pos.z = float((gl_VertexID >> SizeShift.z) & SizeMask.z) / (SizeMask.z + 1);
		
	Pos = Pos*2 - 1;
	
	vec4 _Field = vec4(flame(volumeTex, Pos, time));
	
	gl_Position = u_projection * u_modelView * vec4(Pos, 1.0);
	
	Field.x = _Field.x;
	// Generate in-out flags
	Field.y = (_Field.x > IsoValue) ? 1 : 0;

	Color = vec4(_Field.x);
	
	v_color = Color;
	v_normal = mat3(u_normal) * _Field.xyz;
}