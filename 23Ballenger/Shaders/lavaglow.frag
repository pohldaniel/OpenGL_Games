uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform float invRadius;
uniform float alpha;
uniform float lava_height, posy;
varying vec3 lightVec;
varying vec3 eyeVec;
varying vec2 texCoord;

void main (void)
{
	float distSqr = dot(lightVec, lightVec);
	float att = clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0);
	vec3 lVec = lightVec * inversesqrt(distSqr);

	vec3 vVec = normalize(eyeVec);
	
	vec4 base = texture2D(colorMap, texCoord);
	
	vec3 bump = normalize( texture2D(normalMap, texCoord).xyz * 2.0 - 1.0);

	vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

	float diffuse = max( dot(lVec, bump), 0.0 );
	
	vec4 vDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * 
					diffuse;	

	float specular = pow(clamp(dot(reflect(-lVec, bump), vVec), 0.0, 1.0), 
	                 gl_FrontMaterial.shininess );
	
	vec4 vSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular * 
					 specular;	
	
	vec4 realcolor = vec4( vec3(( vAmbient*base + vDiffuse*base + vSpecular)*att)*gl_Color.rgb , alpha );

	if(posy <= lava_height+6)
    {
		vec4 glowcolor = realcolor;
        glowcolor = mix( glowcolor*(2,0.8,0,1) , glowcolor , (posy-lava_height)/6.0 );
        if(posy < lava_height+3) glowcolor = mix( glowcolor*vec4(2,0.4,0,1) , glowcolor , (posy-lava_height)/3.0 );
		gl_FragColor = glowcolor;
    }
    else gl_FragColor = realcolor;
}