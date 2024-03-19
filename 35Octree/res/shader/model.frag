#version 430 core

in vec2 v_texCoord;
in vec3 vertLightDirection;

uniform vec3 lightColour = {1.0,0.6,0.6};
uniform vec3 emissiveColour = {0,0,0};
uniform vec3 ambientColour  = {0.05f,0.05f,0.1f};
uniform vec3 diffuseColour  = {1.0f,1.0f,1.0f};
uniform float alpha         = 1.0f;

uniform sampler2D u_diffuse;
uniform sampler2D u_normal;

out vec4 colorOut;

vec3 CalculateMappedNormal(){
	return 2.0 * texture(u_normal,v_texCoord).xyz - vec3(1.0,1.0,1.0);
}

void main()
{
	vec3 toLight = vertLightDirection;
	vec3 normal = CalculateMappedNormal();
	float lightDistanceSquared = pow(toLight.x,2) + pow(toLight.y,2) + pow(toLight.z,2);
	float lightPower = 2;

	vec3 texColour = vec3(texture(u_diffuse,v_texCoord));
	vec3 diffuse = diffuseColour * lightColour * max( dot( normal, toLight ), 0)  / (lightDistanceSquared/lightPower);

	colorOut = vec4( emissiveColour + texColour*(ambientColour + diffuse), alpha);
}

