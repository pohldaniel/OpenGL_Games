varying vec3 pos_eye;
varying vec3 normal_eye;

#define PI 3.14159265358979323846f

void main()
{
	float yaw; //positive angle (evita que la orientacion horizontal de la camera varie las intensidades de la esfera)
	float dx = abs(pos_eye.x), dz = abs(pos_eye.z);
	if(pos_eye.z > 0 && pos_eye.x >= 0) yaw = atan( dx / dz ); //primer cuadrante
	if(pos_eye.z <= 0 && pos_eye.x > 0) yaw = PI/2 + atan( dz / dx ); //segundo cuadrante
	if(pos_eye.z < 0 && pos_eye.x <= 0) yaw = PI + atan( dx / dz );//tercer cuadrante
	if(pos_eye.z >= 0 && pos_eye.x < 0) yaw = PI*3/2 + atan( dz / dx );//cuarto cuadrante

	float normalz = sin(yaw)*normal_eye.x + cos(yaw)*normal_eye.z;
	normalz = abs(normalz*normalz*normalz);

    gl_FragColor = vec4( mix(gl_Color.rgb,vec3(1,1,1),normalz*normalz) , normalz );
	
	//gl_FragColor = vec4(normal_eye , 1.0);
}