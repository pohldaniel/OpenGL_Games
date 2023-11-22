#version 430 core

out vec4 FragColor;
in vec3 gFacetNormal;
in vec3 gTriDistance;
uniform vec3 LightPosition;

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main()
{
    vec3 N = normalize(gFacetNormal);
    vec3 L = normalize(LightPosition);
    float df = abs(dot(N, L));
    vec3 color = vec3(df);

    float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
    if (d1 > 0.05) {
        discard;
        return;
    }
    d1 = 1 - amplify(d1, 100, -0.5);
    color = color - d1 * vec3(1);
    
    if (!gl_FrontFacing) {
        discard;
        return;
    }
        
    FragColor = vec4(color, 1.0);
}