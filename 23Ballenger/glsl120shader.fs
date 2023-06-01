#version 120

varying vec3 var_Normal, var_LightDirection;

void main()
{
	gl_FragColor = gl_Color;

	float NdotLD = dot(normalize(var_Normal), normalize(var_LightDirection));

	gl_FragColor.rgb *= 0.5 + 0.5 * NdotLD;
}
