#version 410 core
#define shadeSmooth 0

#if shadeSmooth
	smooth in vec4 v_color;
#else
	flat in vec4 v_color;
#endif
	
out vec4 color;

void main(){
	color = v_color;
}  