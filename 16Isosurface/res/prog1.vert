#version 410 core

uniform sampler3D volumeTex;
uniform ivec3 SizeMask = ivec3(63, 63, 63);
uniform ivec3 SizeShift = ivec3(0, 6, 12 );

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;
uniform float IsoValue = 0.5;

out vec2 Field;
out	vec4 Color;

out	vec3 v_normal;
out	vec4 v_color;

void main(void) {
	vec3 Pos;
	Pos.x = float((gl_VertexID >> SizeShift.x) & SizeMask.x) / (SizeMask.x + 1);
	Pos.y = float((gl_VertexID >> SizeShift.y) & SizeMask.y) / (SizeMask.y + 1);
	Pos.z = float((gl_VertexID >> SizeShift.z) & SizeMask.z) / (SizeMask.z + 1);
	
	vec4 _Field = texture(volumeTex, Pos);
	
	Pos = Pos*2 - 1;
	
	gl_Position = u_projection * u_modelView * vec4(Pos, 1.0);
	
	Field.x = _Field.x;
	// Generate in-out flags
	Field.y = (_Field.x > IsoValue) ? 1 : 0;

	Color = vec4(_Field.x);
	
	
	v_color = Color;
	v_normal = mat3(u_normal) * _Field.xyz;
	
}