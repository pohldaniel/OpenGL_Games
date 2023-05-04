uniform sampler2D tex_top;
uniform sampler2D tex_side;
uniform float height, hmax;
varying vec3 pos_model, normal_model;
varying vec3 pos_eye, normal_eye;

void main()
{
   vec3 L = normalize(gl_LightSource[0].position.xyz - pos_eye);   
   vec3 E = normalize(-pos_eye); // we are in Eye Coordinates, so EyePos is (0,0,0)  
   vec3 R = normalize(-reflect(L,normal_eye));
 
   //calculate Ambient Term:  
   vec4 Iamb = gl_FrontLightProduct[0].ambient;    

   //calculate Diffuse Term:  
   vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(normal_eye,L), 0.0);
   Idiff = clamp(Idiff, 0.0, 1.0);     
   
   // calculate Specular Term:
   vec4 Ispec = gl_FrontLightProduct[0].specular 
                * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);
   Ispec = clamp(Ispec, 0.0, 1.0); 

   // write Total Color:  
   vec4 lightcolor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;    

	vec4 realcolor = mix( texture2D(tex_side,gl_TexCoord[0].st) , texture2D(tex_top,gl_TexCoord[0].st) , normal_model.y*normal_model.y); //en base a la pendiente
	realcolor *= (lightcolor+vec4(0.3,0.3,0.3,0.0));
	if(pos_model.y <= hmax+6) realcolor = mix( texture2D(tex_side,gl_TexCoord[0].st) , realcolor , (pos_model.y-hmax)/6.0 ); //cerca de la lava es roca
	vec4 burnedcolor;
	
    if(pos_model.y <= hmax)
    {
        if(pos_model.y > hmax-0.5) burnedcolor = mix( realcolor*vec4(0.0,0.0,0.2,1) , realcolor , (pos_model.y-(hmax-0.5))/0.5 );
        else burnedcolor = realcolor*vec4(0.0,0.0,0.0,1);
	}
    else burnedcolor = realcolor;

    if(pos_model.y <= height+6)
    {
        gl_FragColor = mix( realcolor*(2,0.8,0,1) , burnedcolor , (pos_model.y-height)/6.0 );
        if(pos_model.y < height+3) gl_FragColor = mix( realcolor*vec4(2,0.4,0,1) , gl_FragColor , (pos_model.y-height)/3.0 );
    }
    else gl_FragColor = burnedcolor;
}