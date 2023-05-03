#include "cColumn.h"

cColumn::cColumn(){}
cColumn::~cColumn(){}

void cColumn::Draw(cShader *Shader,cModel *Model,cData *Data,int id)
{
	glPushMatrix();

	glTranslatef(x,y,z);
	glRotatef(yaw,0.0f,1.0f,0.0f);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_COLUMN));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_COLUMN_NMAP));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_COMPLEX_NORMALMAP);
	Shader->SetUniform("colorMap", 0);
	Shader->SetUniform("normalMap", 1);
	Shader->SetUniform("invRadius", 0.0f);
	Shader->SetUniform("alpha", 1.0f);
	Model->Draw(MODEL_COLUMN);
	Shader->Deactivate();

	//color dye
	if(id==0) glColor3f(1.0f,0.0f,0.0f); //rojo
	if(id==1) glColor3f(1.0f,1.0f,0.0f); //amarillo
	if(id==2) glColor3f(0.0f,1.0f,0.0f); //verde
	if(id==3) glColor3f(0.1f,0.1f,1.0f); //azul
	if(id==4) glColor3f(1.0f,0.0f,1.0f); //violeta

	glTranslatef(0,COLUMN_HEIGHT+ENERGY_BALL_RADIUS,0);

	glEnable(GL_BLEND);
	GLUquadricObj *q = gluNewQuadric();

	Shader->Activate(PROGRAM_SIMPLE_LIGHTBALL);
	gluSphere(q,ENERGY_BALL_RADIUS,32,32);
	Shader->Deactivate();

	gluDeleteQuadric(q);
	glDisable(GL_BLEND);

	glColor4f(1,1,1,1);

	glPopMatrix();
}

void cColumn::SetColumn(float posx, float posy, float posz, float ang)
{
	x = posx;
	y = posy;
	z = posz;
	yaw = ang;
}
bool cColumn::InsideGatheringArea(float posx, float posy, float posz)
{
	if(yaw == -90)
		return (posz <= z + GATHERNG_AREA_SIDE/2 && posz >= z - GATHERNG_AREA_SIDE/2 && posx <= x && posx >= x - GATHERNG_AREA_SIDE);
	else if(yaw == 90)
		return (posz <= z + GATHERNG_AREA_SIDE/2 && posz >= z - GATHERNG_AREA_SIDE/2 && posx <= x + GATHERNG_AREA_SIDE && posx >= x);
	else //if(yaw == 180)
		return (posz <= z && posz >= z - GATHERNG_AREA_SIDE && posx <= x + GATHERNG_AREA_SIDE/2 && posx >= x - GATHERNG_AREA_SIDE/2);
}

float cColumn::GetX()
{
	return x;
}
float cColumn::GetY()
{
	return y;
}
float cColumn::GetZ()
{
	return z;
}
float cColumn::GetHoleX()
{
	if(yaw == -90) return x - 1.5f;
	else if(yaw == 90)  return x + 1.5f;
	else /*if(yaw == 180)*/ return x;
}
float cColumn::GetHoleY()
{
	return y + 1.0f;
}
float cColumn::GetHoleZ()
{
	if(yaw == 180) return z - 1.5f;
	else return z;
}
float cColumn::GetYaw()
{
	return yaw;
}