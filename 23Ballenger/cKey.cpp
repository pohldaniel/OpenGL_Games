#include "cKey.h"
#include <iostream>

cKey::cKey()
{
	ang = 0.0f;
	deployed = false;
}
cKey::~cKey(){}

void cKey::DrawLevitating(cShader *Shader, cModel *Model, cData *Data, float dist)
{
	GLUquadricObj *q = gluNewQuadric();

	//key
	glPushMatrix();

	ang = fmod(ang+LEVITATION_SPEED,360);
	glTranslatef(x,y+0.5f+(sin(ang*(PI/180)))/2,z);
	glRotatef(ang,0.0f,1.0f,0.0f);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_KEY));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_KEY_NMAP));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_COMPLEX_NORMALMAP);
	Shader->SetUniform("colorMap", 0);
	Shader->SetUniform("normalMap", 1);
	Shader->SetUniform("invRadius", 0.0f);
	Shader->SetUniform("alpha", 1.0f);
	Model->Draw(MODEL_KEY);
	Shader->Deactivate();

	glPopMatrix();
	//f.key

	//beacon
	glPushMatrix();

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE); //desactivo la escritura en el zbuffer (me aprovecho del algoritmo del pintor para que todos los triangulos se pinten en el orden que le toca [ssi estan a la vista])
	glDisable(GL_CULL_FACE);
	glTranslatef(x,y,z);
	glRotatef(-90,1.0f,0.0f,0.0f);

	Shader->Activate(PROGRAM_SIMPLE_LIGHTBEAM);
	Shader->SetUniform("hmax", BEACON_HEIGHT-y);
	if(dist/100 < BEACON_MIN_RADIUS) gluCylinder(q,BEACON_MIN_RADIUS,BEACON_MIN_RADIUS,BEACON_HEIGHT-y,16,16);
	else gluCylinder(q,dist/100,dist/100,BEACON_HEIGHT-y,16,16);
	Shader->Deactivate();

	gluDeleteQuadric(q);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	glPopMatrix();
	//f.beacon
}

void cKey::DrawPicked(float playerx,float playery, float playerz, float camera_yaw, cModel *Model, cData *Data, cShader *Shader)
{
	glPushMatrix();

	ang = fmod(ang+LEVITATION_SPEED,360);
	glTranslatef(playerx,playery+0.7,playerz);
	glRotatef(ang,0.0f,1.0f,0.0f);
	//glRotatef(camera_yaw*(180/PI),0.0f,1.0f,0.0f);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_KEY));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_KEY_NMAP));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_COMPLEX_NORMALMAP);
	Shader->SetUniform("colorMap", 0);
	Shader->SetUniform("normalMap", 1);
	Shader->SetUniform("invRadius", 0.0f);
	Shader->SetUniform("alpha", 1.0f);
	Model->Draw(MODEL_KEY);
	Shader->Deactivate();
	
	glPopMatrix();
}

void cKey::DrawDeployed(float holex,float holey,float holez, float yaw, cModel *Model, cData *Data, cShader *Shader)
{
	glPushMatrix();

	glTranslatef(holex,holey,holez);
	glRotatef(yaw,0,1,0);
	glRotatef(180+45,1,0,0);
	glTranslatef(0,-0.69,0);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_KEY));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_KEY_NMAP));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_COMPLEX_NORMALMAP);
	Shader->SetUniform("colorMap", 0);
	Shader->SetUniform("normalMap", 1);
	Shader->SetUniform("invRadius", 0.0f);
	Shader->SetUniform("alpha", 1.0f);
	Model->Draw(MODEL_KEY);
	Shader->Deactivate();
	
	glPopMatrix();
}

void cKey::SetPos(float posx, float posy, float posz)
{
	x = posx;
	y = posy;
	z = posz;
}
float cKey::GetX() const
{
    return x;
}
float cKey::GetY() const
{
    return y;
}
float cKey::GetZ() const
{
    return z;
}
void cKey::Deploy()
{
	std::cout << "Deploy: " << std::endl;
	deployed = true;
}
bool cKey::IsDeployed() const
{
	return deployed;
}