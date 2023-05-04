// simple vertex shader

varying vec3 pos_model;
varying vec3 pos_eye;
varying vec3 normal_model;
varying vec3 normal_eye;

void main()
{
    pos_model    = vec3(gl_Vertex);
    pos_eye      = vec3(gl_ModelViewMatrix*gl_Vertex);
    normal_model = normalize(gl_Normal);
    normal_eye   = normalize(gl_NormalMatrix*gl_Normal);

    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor  = gl_Color;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
