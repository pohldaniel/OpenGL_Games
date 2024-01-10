#version 410 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_position;
in vec3 v_worldPos;
noperspective in vec3 v_edgeDistance;

uniform float u_wireframeWidth = 0.2;
uniform vec4 u_wireframeColor = vec4(0.5, 0.5, 0.5, 1.0);
uniform bool u_drawOverlay = true;
uniform vec4 u_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

out vec4 colorOut;

void main() {
   
	colorOut = u_color;

	if(!u_drawOverlay)
	  return;

    if (v_edgeDistance.x >= 0) {
	float d = min( v_edgeDistance.x, min(v_edgeDistance.y, v_edgeDistance.z ));

        float mixVal = 0.0;
        if (d < u_wireframeWidth - 1) {
            mixVal = 1.0;
        } else if (d > u_wireframeWidth + 1) {
            mixVal = 0.0;
        } else {
            float x = d - (u_wireframeWidth - 1);
            mixVal = exp2(-2.0 * x * x);
        }        

        colorOut = mix(colorOut, u_wireframeColor, mixVal);
    }	
}
