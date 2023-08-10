#include "Uniforms.glsl"

#ifdef COMPILEVS

#include "Transform.glsl"

in vec3 position;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out vec4 vWorldPos;
out vec3 vNormal;
out vec4 vTangent;
out vec3 vViewNormal;
out vec2 vTexCoord;
noperspective out vec2 vScreenPos;

#else

#include "Lighting.glsl"

in vec4 vWorldPos;
in vec3 vNormal;
in vec4 vTangent;
in vec3 vViewNormal;
in vec2 vTexCoord;
noperspective in vec2 vScreenPos;
out vec4 fragColor[2];

layout(std140) uniform PerMaterialData3
{
    vec4 matDiffColor;
    vec4 matSpecColor;
};

uniform sampler2D diffuseTex0;
uniform sampler2D normalTex1;

vec3 DecodeNormal(vec4 normalInput)
{
    vec3 normal;
    normal.xy = normalInput.ag * 2.0 - 1.0;
    normal.z = sqrt(max(1.0 - dot(normal.xy, normal.xy), 0.0));
    return normal;
}

#endif

void vert(){

    mat3x4 world = GetWorldMatrix();   
    vWorldPos.xyz = vec4(position, 1.0) * world;
    //gl_Position = vec4(position.xyz, 1.0) * viewProjMatrix;
  
	//gl_Position = vec4(position, 1.0) * GetWorldMatrix4() * view * projection;
	gl_Position = projection * view * GetWorldMatrix4() * vec4(position, 1.0); 	
}

void frag()
{
    
	fragColor[0] = vec4(1.0, 0.0, 0.0, 1.0);
}
