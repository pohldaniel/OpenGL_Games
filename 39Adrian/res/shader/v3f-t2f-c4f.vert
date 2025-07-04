#version 410 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 color;

uniform mat4 model = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 projection = mat4(1.0);

out vec2 texCoord;
out vec4 vertColor;

void main(){
    gl_Position       = projection*(view*(model*vec4(vertex,1.0)));
	texCoord = tex_coord.xy;
    vertColor     = color;
}
