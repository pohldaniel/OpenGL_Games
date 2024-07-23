//https://gamedev.stackexchange.com/questions/205467/add-a-rounded-border-to-a-texture-with-a-fragment-shader

#version 410 core

in vec2 v_texCoord;
in vec4 v_color;									

out vec4 outColor;

uniform float u_radius = 5;
uniform uint u_edge = 0;
uniform vec2 u_dimensions;

float calcDistance(vec2 uv) {
	vec2 pos = (abs(uv - 0.5) + 0.5);
	vec2 delta = max(pos * u_dimensions - u_dimensions + pos *u_radius, 0.0);
	return length(delta);
}

void main() {
    float dist = calcDistance(v_texCoord);
	
	if(u_edge == 0 && dist > u_radius){
      discard;  
	//bottom left
    }else if(u_edge == 1 && dist > u_radius && v_texCoord.x < 0.5 && v_texCoord.y < 0.5) {
      discard;
	//top left
    }else if(u_edge == 2 && dist > u_radius && v_texCoord.x < 0.5 && v_texCoord.y > 0.5) {
      discard;
	//top right
    }else if(u_edge == 3 && dist > u_radius && v_texCoord.x > 0.5 && v_texCoord.y > 0.5) {
      discard;
	//bottom right
    }else if(u_edge == 4 && dist > u_radius && v_texCoord.x > 0.5 && v_texCoord.y < 0.5) {
      discard;
	//right
	}else if(u_edge == 5 && dist > u_radius && v_texCoord.x > 0.5) {
      discard;
	//left
	}else if(u_edge == 6 && dist > u_radius && v_texCoord.x < 0.5) {
      discard;
	//top
	}else if(u_edge == 7 && dist > u_radius && v_texCoord.y > 0.5) {
      discard;
	//bottom
	}else if(u_edge == 8 && dist > u_radius && v_texCoord.y < 0.5) {
      discard;
	}	
	
	outColor = v_color;
}