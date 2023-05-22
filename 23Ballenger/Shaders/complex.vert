varying vec3 lightVec; 
varying vec3 eyeVec;
varying vec2 texCoord;
varying vec3 tangent;
varying vec3 normal;
					 
void main(void)
{
	gl_Position = ftransform();
	gl_FrontColor  = gl_Color;
	texCoord = gl_MultiTexCoord0.xy;
	
	vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0)); 
	if(length(c1)>length(c2)) tangent = c1;	
	else tangent = c2;	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * tangent);
	vec3 b = cross(n, t);
	
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 tmpVec = gl_LightSource[0].position.xyz - vVertex;

	lightVec.x = dot(tmpVec, t);
	lightVec.y = dot(tmpVec, b);
	lightVec.z = dot(tmpVec, n);

	tmpVec = -vVertex;
	eyeVec.x = dot(tmpVec, t);
	eyeVec.y = dot(tmpVec, b);
	eyeVec.z = dot(tmpVec, n);
}