#include "cRespawnPoint.h"

cRespawnPoint::cRespawnPoint(){}
cRespawnPoint::~cRespawnPoint(){}

//suponiendo matrices 4x4
/*
bool MatrixInverse(GLfloat m[16], GLfloat invOut[16])
{
    double inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}
*/

void cRespawnPoint::Draw(int tex_id, bool activated, cShader *Shader)
{
	//GLfloat ViewMatrix[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, ViewMatrix); //las ultimas transformaciones fueron las de gluLookAt
	
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glTranslatef(x,y+HEIGHT_OFFSET,z);

	//circle
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( CIRCLE_RADIUS, 0,-CIRCLE_RADIUS);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-CIRCLE_RADIUS, 0,-CIRCLE_RADIUS);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-CIRCLE_RADIUS, 0, CIRCLE_RADIUS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( CIRCLE_RADIUS, 0, CIRCLE_RADIUS);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	//f.circle
	
	//aura
	glDepthMask(GL_FALSE); //desactivo la escritura en el zbuffer (me aprovecho del algoritmo del pintor para que todos los triangulos se pinten en el orden que le toca [ssi estan a la vista])
	glDisable(GL_CULL_FACE);
	GLUquadricObj *q = gluNewQuadric();
	glRotatef(-90.0f,1.0f,0.0f,0.0f);
	if(activated) glColor4f(1.0f,0.4f,0.0f,0.6f);
	else glColor4f(0.5f,0.5f,1.0f,0.6f);

	Shader->Activate(PROGRAM_SIMPLE_LIGHTBEAM);
	Shader->SetUniform("hmax", AURA_HEIGHT);
	gluCylinder(q,CIRCLE_RADIUS,CIRCLE_RADIUS,AURA_HEIGHT,16,16);
	Shader->Deactivate();

	//glTranslatef(0,0,HEIGHT_OFFSET);
	//gluDisk(q,0,CIRCLE_RADIUS,16,16);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	gluDeleteQuadric(q);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	//f.aura

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void cRespawnPoint::Draw1(int tex_id) {
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glTranslatef(x, y + HEIGHT_OFFSET, z);

	//circle
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_id);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(CIRCLE_RADIUS, 0, -CIRCLE_RADIUS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-CIRCLE_RADIUS, 0, -CIRCLE_RADIUS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-CIRCLE_RADIUS, 0, CIRCLE_RADIUS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(CIRCLE_RADIUS, 0, CIRCLE_RADIUS);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glPopMatrix();
}

void cRespawnPoint::Draw2(bool activated, cShader *Shader) {
	if (activated) glColor4f(1.0f, 0.4f, 0.0f, 0.6f);
	else glColor4f(0.5f, 0.5f, 1.0f, 0.6f);
	
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glTranslatef(x, y + HEIGHT_OFFSET, z);

	//aura
	glDepthMask(GL_FALSE); //desactivo la escritura en el zbuffer (me aprovecho del algoritmo del pintor para que todos los triangulos se pinten en el orden que le toca [ssi estan a la vista])
	glDisable(GL_CULL_FACE);
	GLUquadricObj *q = gluNewQuadric();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	

	Shader->Activate(PROGRAM_SIMPLE_LIGHTBEAM);
	Shader->SetUniform("hmax", AURA_HEIGHT);
	gluCylinder(q, CIRCLE_RADIUS, CIRCLE_RADIUS, AURA_HEIGHT, 16, 16);
	Shader->Deactivate();

	glTranslatef(0,0,HEIGHT_OFFSET);
	gluDisk(q,0,CIRCLE_RADIUS,16,16);

	gluDeleteQuadric(q);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	//f.aura

	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);

	glPopMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void cRespawnPoint::SetPos(float posx, float posy, float posz)
{
	x = posx;
	y = posy;
	z = posz;
}
float cRespawnPoint::GetX()
{
    return x;
}
float cRespawnPoint::GetY()
{
    return y;
}
float cRespawnPoint::GetZ()
{
    return z;
}