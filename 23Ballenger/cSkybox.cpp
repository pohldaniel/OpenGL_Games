#include "cSkybox.h"

cSkybox::cSkybox(){}
cSkybox::~cSkybox(){}

void cSkybox::Load(float zfar)
{
	float dist = zfar*cos(45.0f); //zfar a secas era la distancia del jugador a una de las esquinas de la hitbox
	float th = 1.0f/3.0f; // un tercio en coordenadas de textura
	float of = (1.0f/1024.0f)/2; //offset de ~1/2 texel

	id_Skybox=glGenLists(1);
	glNewList(id_Skybox,GL_COMPILE);

	// Top
	glBegin(GL_QUADS);
		glTexCoord2f(0.25f+of, 1*th-of); glVertex3f(dist, dist,  dist);
		glTexCoord2f(0.50f-of, 1*th-of); glVertex3f(-dist, dist,  dist);
		glTexCoord2f(0.50f-of, 0.0f+of); glVertex3f(-dist, dist,  -dist);
		glTexCoord2f(0.25f+of, 0.0f+of); glVertex3f(dist, dist,  -dist);
	glEnd();
	// Bottom
	glBegin(GL_QUADS); 
		glTexCoord2f(0.25f+of, 1.0f-of); glVertex3f(dist, -dist,  -dist);
		glTexCoord2f(0.50f-of, 1.0f-of); glVertex3f(-dist, -dist,  -dist);
		glTexCoord2f(0.50f-of, 2*th+of); glVertex3f(-dist, -dist,  dist);
		glTexCoord2f(0.25f+of, 2*th+of); glVertex3f(dist, -dist,  dist);
	glEnd();

	// Front
	glBegin(GL_QUADS);
		glTexCoord2f(0.25f, 2*th-of); glVertex3f(dist, -dist,  dist);
		glTexCoord2f(0.50f, 2*th-of); glVertex3f(-dist, -dist,  dist);
		glTexCoord2f(0.50f, 1*th+of); glVertex3f(-dist, dist,  dist);
		glTexCoord2f(0.25f, 1*th+of); glVertex3f(dist, dist,  dist);
	glEnd();
	// Back
	glBegin(GL_QUADS);
		glTexCoord2f(0.75f, 2*th-of); glVertex3f(-dist, -dist,  -dist);
		glTexCoord2f(1.00f, 2*th-of); glVertex3f(dist, -dist,  -dist);
		glTexCoord2f(1.00f, 1*th+of); glVertex3f(dist, dist,  -dist);
		glTexCoord2f(0.75f, 1*th+of); glVertex3f(-dist, dist,  -dist);
	glEnd();

	// Right
	glBegin(GL_QUADS);
		glTexCoord2f(0.50f, 2*th-of); glVertex3f(-dist, -dist,  dist);
		glTexCoord2f(0.75f, 2*th-of); glVertex3f(-dist, -dist,  -dist);
		glTexCoord2f(0.75f, 1*th+of); glVertex3f(-dist, dist,  -dist);
		glTexCoord2f(0.50f, 1*th+of); glVertex3f(-dist, dist,  dist);
	glEnd();
	// Left
	glBegin(GL_QUADS);
		glTexCoord2f(0.00f, 2*th-of); glVertex3f(dist, -dist,  -dist);
		glTexCoord2f(0.25f, 2*th-of); glVertex3f(dist, -dist,  dist);
		glTexCoord2f(0.25f, 1*th+of); glVertex3f(dist, dist,  dist);
		glTexCoord2f(0.00f, 1*th+of); glVertex3f(dist, dist,  -dist);
	glEnd();

	glEndList();
}


void cSkybox::Draw(int tex_id,float player_x,float player_y,float player_z)
{
	glPushMatrix();
	glTranslatef(player_x,player_y,player_z);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);
	glCallList(id_Skybox);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}