#version 410 core

in vec2 texCoord;
in vec4 vertColor;

uniform sampler2D texture;

out vec4 color;

void main(){
    float a = texture2D(texture, texCoord.xy).r;
    color = vec4(vertColor.rgb, vertColor.a*a);
	
	//color = vertColor * vec4(1.0, 1.0, 1.0, a);
}
