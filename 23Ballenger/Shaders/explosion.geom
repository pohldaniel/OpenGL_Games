// these lines enable the geometry shader support.
#version 120
#extension GL_EXT_geometry_shader4 : enable

uniform float time;
uniform float center_x;
uniform float center_y;
uniform float center_z;
varying in vec3 pos_model[3]; //los 3 vertices del triangulo
varying in vec3 normal_model[3]; //las 3 normales de cada vertice

const float speed = 0;

void main( void )
{
	vec3 center_world = vec3(center_x,center_y,center_z);
	vec3 n = normalize(normal_model[0]+normal_model[1]+normal_model[2]);
	for( int i = 0 ; i < gl_VerticesIn ; i++ )
	{
		gl_FrontColor = gl_FrontColorIn[ i ];
		gl_Position = gl_ModelViewProjectionMatrix * vec4( pos_model[i]+center_world+(time*speed*n) , 1.0 );
		gl_TexCoord[0] = gl_TexCoordIn  [ i ][ 0 ];
		EmitVertex();
	}
}