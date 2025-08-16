#version 410 core

in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2D u_color;

out vec4 outColor;

void main(){
	vec4 blurSample = vec4(0, 0, 0, 0);
	vec4 tmpPix;
	vec4 offPix;
	
	for(int tx =-2;tx<3;tx++){
		for(int ty =-2;ty<3;ty++){
			vec2 uv = v_texCoord;
//			uv.x = uv.x + tx*1/1024.0;
//			uv.y = uv.y + ty*1/768.0;
			uv.x = uv.x + tx*0.01;
			uv.y = uv.y + ty*0.01;
			tmpPix = texture2D(u_color, uv);
			offPix = -0.3+tmpPix;
			offPix = offPix *32;
			blurSample = blurSample + offPix;			
		}	
    }
	blurSample = blurSample / 256;
	outColor = texture2D(u_color, v_texCoord) + blurSample * 0.2;
}