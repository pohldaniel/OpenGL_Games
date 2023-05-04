uniform float hmax;
varying vec3 pos_model;
varying vec3 pos_eye;
varying vec3 normal_eye;

#define PI 3.14159265358979323846f

void main()
{
    float factor = (hmax - pos_model.z)/hmax;

	float yaw; //positive angle (evita que la orientacion horizontal de la camera varie las intensidades del cilindro)
	float dx = abs(pos_eye.x), dz = abs(pos_eye.z);
	if(pos_eye.z > 0 && pos_eye.x >= 0) yaw = atan( dx / dz ); //primer cuadrante
	if(pos_eye.z <= 0 && pos_eye.x > 0) yaw = PI/2 + atan( dz / dx ); //segundo cuadrante
	if(pos_eye.z < 0 && pos_eye.x <= 0) yaw = PI + atan( dx / dz );//tercer cuadrante
	if(pos_eye.z >= 0 && pos_eye.x < 0) yaw = PI*3/2 + atan( dz / dx );//cuarto cuadrante

	vec3 normal;
	normal.x = cos(yaw)*normal_eye.x - sin(yaw)*normal_eye.z;
	normal.y = normal_eye.y;
	normal.z = sin(yaw)*normal_eye.x + cos(yaw)*normal_eye.z;
	normal = normalize(normal);

	float pitch; //positive angle (evita que al mirar arriba y abajo el cilindro cambie de intensidad)
	float dy = abs(normal.y);
	dz = abs(normal.z);
	if(normal.z > 0 && normal.y >= 0) pitch = atan( dy / dz ); //primer cuadrante
	if(normal.z <= 0 && normal.y > 0) pitch = PI/2 + atan( dz / dy ); //segundo cuadrante
	if(normal.z < 0 && normal.y <= 0) pitch = PI + atan( dy / dz );//tercer cuadrante
	if(normal.z >= 0 && normal.y < 0) pitch = PI*3/2 + atan( dz / dy );//cuarto cuadrante

	float normalz = sin(pitch)*normal.y + cos(pitch)*normal.z;
	
	factor *= abs(normalz*normalz*normalz);

    if(hmax < 10) gl_FragColor = vec4(gl_Color.rgb,factor);
	else gl_FragColor = vec4( mix(gl_Color.rgb,gl_Color.rgb+vec3(0.7,0.7,0.7),factor*factor) , 0.4*factor );
}

/*
**************************
eje y
A
|
|
obs_eye(0,0,0) ---> eje x
O
eje z(hacia ti)

**************************
*/