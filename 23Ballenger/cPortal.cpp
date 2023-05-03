#include "cPortal.h"

cPortal::cPortal(){}
cPortal::~cPortal(){}

void cPortal::Draw(cData *Data, bool activated, cShader *Shader, cModel *Model)
{
	//vortex
	if(activated)
	{
		glPushMatrix();

		glTranslatef(x,y,z);
		ang = fmod(ang+PORTAL_SPEED,360);
		glTranslatef(0,PORTAL_SIDE/2,0);
		glRotatef(ang,0.0f,0.0f,1.0f);
		
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_VORTEX));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex3f( PORTAL_SIDE/2,-PORTAL_SIDE/2, 0);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-PORTAL_SIDE/2,-PORTAL_SIDE/2, 0);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-PORTAL_SIDE/2, PORTAL_SIDE/2, 0);
			glTexCoord2f(0.0f, 0.0f); glVertex3f( PORTAL_SIDE/2, PORTAL_SIDE/2, 0);
		glEnd();
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);

		glPopMatrix();
	}
	//f.vortex

	//portal
	glPushMatrix();

	glTranslatef(x,y,z);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_PORTAL));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_PORTAL_NMAP));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_COMPLEX_NORMALMAP);
	Shader->SetUniform("colorMap", 0);
	Shader->SetUniform("normalMap", 1);
	Shader->SetUniform("invRadius", 0.0f);
	Shader->SetUniform("alpha", 1.0f);
	Model->Draw(MODEL_PORTAL);
	Shader->Deactivate();
	
	glEnable(GL_BLEND);
	GLUquadricObj *q = gluNewQuadric();

	glTranslatef(0,PORTAL_SIDE*3/2,0);
	glColor3f(0.0f,1.0f,0.0f); //verde
	Shader->Activate(PROGRAM_SIMPLE_LIGHTBALL);
	gluSphere(q,0.2,16,16);
	Shader->Deactivate();

	glTranslatef(PORTAL_SIDE/2,-PORTAL_SIDE/2,0);
	glColor3f(1.0f,1.0f,0.0f); //amarillo
	Shader->Activate(PROGRAM_SIMPLE_LIGHTBALL);
	gluSphere(q,0.2,16,16);
	Shader->Deactivate();

	glTranslatef(-PORTAL_SIDE,0,0);
	glColor3f(0.2f,0.2f,1.0f); //azul
	Shader->Activate(PROGRAM_SIMPLE_LIGHTBALL);
	gluSphere(q,0.2,16,16);
	Shader->Deactivate();

	glTranslatef(0,-(PORTAL_SIDE-1),0);
	glColor3f(1.0f,0.0f,1.0f); //violeta
	Shader->Activate(PROGRAM_SIMPLE_LIGHTBALL);
	gluSphere(q,0.2,16,16);
	Shader->Deactivate();

	glTranslatef(PORTAL_SIDE,0,0);
	glColor3f(1.0f,0.0f,0.0f); //rojo
	Shader->Activate(PROGRAM_SIMPLE_LIGHTBALL);
	gluSphere(q,0.2,16,16);
	Shader->Deactivate();

	gluDeleteQuadric(q);
	glDisable(GL_BLEND);

	glColor4f(1,1,1,1);

	glPopMatrix();
	//f.portal
}

void cPortal::SetPos(float posx, float posy, float posz)
{
	x = posx;
	y = posy;
	z = posz;

	//rojo
	receptors[0].first  = x + PORTAL_SIDE/2;
	receptors[0].second = y + 1.0f;
	//amarillo
	receptors[1].first  = x + PORTAL_SIDE/2;
	receptors[1].second = y + PORTAL_SIDE;
	//verde
	receptors[2].first  = x;
	receptors[2].second = y + PORTAL_SIDE*3/2;
	//azul
	receptors[3].first  = x - PORTAL_SIDE/2;
	receptors[3].second = y + PORTAL_SIDE;
	//violeta
	receptors[4].first  = x - PORTAL_SIDE/2;
	receptors[4].second = y + 1.0f;
}

bool cPortal::InsidePortal(float px, float py, float pz, float r)
{
    return ( (px-r <= x+(PORTAL_SIDE/2) && px+r >= x-(PORTAL_SIDE/2)) && (py-r <= y+PORTAL_SIDE && py+r >= y) );
}

float cPortal::GetX()
{
	return x;
}
float cPortal::GetY()
{
	return y;
}
float cPortal::GetZ()
{
	return z;
}
float cPortal::GetReceptorX(int i)
{
	return receptors[i].first;
}
float cPortal::GetReceptorY(int i)
{
	return receptors[i].second;
}