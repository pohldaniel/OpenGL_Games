#version 410 core

uniform sampler2D tex_top;
uniform sampler2D tex_side;
uniform float height, hmax;

uniform vec3 lightPos = vec3(50.0f, 50.0f, 50.0f);
uniform vec4 lightAmbient = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);

uniform vec4 matAmbient = vec4(0.7, 0.7, 0.7, 1.0);
uniform vec4 matDiffuse = vec4(0.8, 0.8, 0.8, 1.0);
uniform vec4 matEmission = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 matSpecular = vec4(0.3, 0.3, 0.3, 1.0);

uniform float matShininess = 100.0;

in vec3 pos_model, normal_model;
in vec3 pos_eye, normal_eye;
in vec2 texCoord;

out vec4 color;

void main() {
	vec3 L = normalize(lightPos - pos_eye);   
	vec3 E = normalize(-pos_eye); // we are in Eye Coordinates, so EyePos is (0,0,0)  
	vec3 R = normalize(-reflect(L,normal_eye));

	//calculate Ambient Term:  
	vec4 Iamb = (lightAmbient * matAmbient);    

	//calculate Diffuse Term:  
	vec4 Idiff = (lightDiffuse * matDiffuse) * max(dot(normal_eye,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);     
   
	// calculate Specular Term:
	vec4 Ispec = (lightSpecular * matSpecular) * pow(max(dot(R,E),0.0),0.3 * matShininess);
	Ispec = clamp(Ispec, 0.0, 1.0); 

	vec4 frontColor = matEmission + matAmbient * lightAmbient;

	// write Total Color:  
	vec4 lightcolor = frontColor + Iamb + Idiff + Ispec;   

	vec4 realcolor = mix( texture2D(tex_side,texCoord) , texture2D(tex_top,texCoord) , normal_model.y*normal_model.y); //en base a la pendiente
	realcolor *= (lightcolor+vec4(0.3,0.3,0.3,0.0));
	if(pos_model.y <= hmax+6) realcolor = mix( texture2D(tex_side,texCoord) , realcolor , (pos_model.y-hmax)/6.0 ); //cerca de la lava es roca
	vec4 burnedcolor;

	if(pos_model.y <= hmax){
        if(pos_model.y > hmax-0.5) burnedcolor = mix( realcolor*vec4(0.0,0.0,0.2,1) , realcolor , (pos_model.y-(hmax-0.5))/0.5 );
        else burnedcolor = realcolor*vec4(0.0, 0.0, 0.0, 1.0);
		
	}else burnedcolor = realcolor;

    if(pos_model.y <= height+6){
        color = mix( realcolor*(2,0.8,0,1) , burnedcolor , (pos_model.y-height)/6.0 );
        if(pos_model.y < height+3) color = mix( realcolor*vec4(2,0.4,0,1) , color , (pos_model.y-height)/3.0 );
    }else color = burnedcolor;
}