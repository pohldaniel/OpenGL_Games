#ifdef COMPILEVS

uniform mat4 viewProjMatrix;

uniform mat4 view;
uniform mat4 projection;

in vec3 position;
in vec4 color;

out vec4 vColor;

#else

in vec4 vColor;
out vec4 fragColor;

#endif

void vert()
{
    vColor = color;
    gl_Position = vec4(position, 1.0) * view * projection;
	
	//gl_Position = projection * view * vec4(position, 1.0);
}

void frag()
{
    fragColor = vec4(vColor.rgb, 1.0);
}
