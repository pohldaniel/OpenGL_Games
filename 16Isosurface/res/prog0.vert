#version 410 core

uniform ivec3 SizeMask = ivec3(63, 63, 63);
uniform ivec3 SizeShift = ivec3(0, 6, 12 );

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;
uniform float IsoValue = 1.0;

uniform int Num_Metaballs = 2;
uniform vec4 Metaballs[2] = vec4[2](vec4(-0.5, 0.0, 0.0, 0.2 ), vec4(0.6, 0.0, 0.0, 0.1));

out vec2 Field;
out vec3 Normal;
out	vec4 Color;

out	vec4 v_color;
out	vec3 v_normal;

vec4 Metaball(vec3 Pos, vec3 Center, float RadiusSq) {
	float epsilon = 0.001;

	vec3 Dist = Pos - Center;
	float InvDistSq = 1 / (dot(Dist, Dist) + epsilon);

	vec4 o;
	o.xyz = -2 * RadiusSq * InvDistSq * InvDistSq * Dist;
	o.w = RadiusSq * InvDistSq;
	return o;
}

void main(void) {

	vec3 Pos;
#if 1
	// Generate sampling point position based on its index
	Pos.x = float((gl_VertexID >> SizeShift.x) & SizeMask.x) / (SizeMask.x + 1);
	Pos.y = float((gl_VertexID >> SizeShift.y) & SizeMask.y) / (SizeMask.y + 1);
	Pos.z = float((gl_VertexID >> SizeShift.z) & SizeMask.z) / (SizeMask.z + 1);
	Pos = Pos*2 - 1;
#else
	// read position from vertex
	Pos = pos.xyz;
#endif

	// Sum up contributions from all metaballs
	vec4 _Field = vec4(0);
		
	for (int i = 0; i < Num_Metaballs; i++){
		_Field += Metaball(Pos, Metaballs[i].xyz, Metaballs[i].w);
	}
	
	gl_Position = u_projection * u_modelView * vec4(Pos, 1.0);
	Normal = mat3(u_normal) * _Field.xyz;
	
	Field.x = _Field.w;

	// Generate in-out flags
	Field.y = (_Field.w < IsoValue) ? 1 : 0;

	Color = (_Field*0.5+0.5) * (_Field.w / 10.0);
	
	v_color = Color;
	v_normal = Normal;
}