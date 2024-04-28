#version 330

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;

void main()
{
	vec4 vEyeSpacePosVertex = matrices.viewMatrix*matrices.modelMatrix*vec4(inPosition, 1.0);
	gl_Position = matrices.projMatrix*vEyeSpacePosVertex;
}