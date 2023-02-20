#version 410 core

layout(location = 0) in vec2 i_position;

layout(std140) uniform u_vertices {
	vec4 vertices[512];
};

uniform mat4 u_transform;
uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;

out vec3 v_Normal;
out vec4 v_fragPos;

void main(void)
{
    vec3 p[16];

    for (int i = 0; i < 16; i++) {
        p[i] = vertices[gl_InstanceID * 16 + i].xyz;
    }

    vec2 uv = i_position;

    vec2 B0 = (1 - uv) * (1 - uv) * (1 - uv);
    vec2 B1 = 3 * uv * (1 - uv) * (1 - uv);
    vec2 B2 = 3 * uv * uv * (1 - uv);
    vec2 B3 = uv * uv * uv;

    vec3 pos =
        (B0.x * p[ 0] + B1.x * p[ 1] + B2.x * p[ 2] + B3.x * p[ 3]) * B0.y +
        (B0.x * p[ 4] + B1.x * p[ 5] + B2.x * p[ 6] + B3.x * p[ 7]) * B1.y +
        (B0.x * p[ 8] + B1.x * p[ 9] + B2.x * p[10] + B3.x * p[11]) * B2.y +
        (B0.x * p[12] + B1.x * p[13] + B2.x * p[14] + B3.x * p[15]) * B3.y;

    vec2 T0 = (1 - uv) * (1 - uv);
    vec2 T1 = 2 * uv * (1 - uv);
    vec2 T2 = uv * uv;

    vec3 dv =
        (B0.x * (p[ 4]-p[0]) + B1.x * (p[ 5]-p[1]) + B2.x * (p[ 6]-p[ 2]) + B3.x * (p[ 7]-p[ 3])) * T0.y +
        (B0.x * (p[ 8]-p[4]) + B1.x * (p[ 9]-p[5]) + B2.x * (p[10]-p[ 6]) + B3.x * (p[11]-p[ 7])) * T1.y +
        (B0.x * (p[12]-p[8]) + B1.x * (p[13]-p[9]) + B2.x * (p[14]-p[10]) + B3.x * (p[15]-p[11])) * T2.y;

    vec3 du =
        (T0.x * (p[ 1]-p[ 0]) + T1.x * (p[ 2]-p[ 1]) + T2.x * (p[ 3]-p[ 2])) * B0.y +
        (T0.x * (p[ 5]-p[ 4]) + T1.x * (p[ 6]-p[ 5]) + T2.x * (p[ 7]-p[ 6])) * B1.y +
        (T0.x * (p[ 9]-p[ 8]) + T1.x * (p[10]-p[ 9]) + T2.x * (p[11]-p[10])) * B2.y +
        (T0.x * (p[13]-p[12]) + T1.x * (p[14]-p[13]) + T2.x * (p[15]-p[14])) * B3.y;
		
	v_fragPos = u_modelView * vec4(pos, 1.0);
    gl_Position = u_projection * v_fragPos;

    vec3 nor = cross(du, dv);
    nor = (length(nor) != 0) ? normalize(nor) : vec3(0.0);
    v_Normal = mat3(u_normal) * nor;
}