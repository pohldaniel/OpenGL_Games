#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;

out vec3 color;
out vec2 texCoord;

out vec3 normal;


uniform mat4 camMatrix;
uniform mat4 modelMatrix = mat4(1.0);


void main(){
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
	// Assigns the colors from the Vertex Data to "color"
	color = aPos;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
	normal = aNormal;
}

