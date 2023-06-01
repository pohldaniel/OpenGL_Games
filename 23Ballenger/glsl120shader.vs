#version 120

uniform vec3 CameraPosition;

varying vec3 var_Normal, var_LightDirection;

void main()
{
	gl_FrontColor = gl_Color;
	var_Normal = gl_Normal;
	var_LightDirection = CameraPosition - gl_Vertex.xyz;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
