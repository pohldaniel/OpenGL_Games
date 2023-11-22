#version 410 core

in vec2 texCoord;
in vec3 eyePosition;
in vec4 material;

out vec4 positionOut;
out vec4 normalOut;
out vec4 baseOut;
 
void main() {
	
	vec3 normal;
	
	normal.xy = texCoord * 2.0 - 1.0;

    // make the ball circular
	float radius = 1.0 - dot( normal.xy, normal.xy);
	
	if (radius < 0.0)
	    discard;
	    
	normal.z = sqrt(radius);
	  
	normal = normalize(normal);  
	
	vec3 finalEyePos = eyePosition;
	finalEyePos.z += 1.5 / 12.0 * normal.z; //normal is also the normalized displacement from the center 1.5/12.0 is the radius of the ball
		
	positionOut = vec4( finalEyePos, 0.0);
	normalOut = vec4( normal * 0.5 + 0.5, 1.0);
	baseOut = material;
	

}