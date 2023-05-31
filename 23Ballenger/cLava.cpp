#include <GL/glew.h>
#include <math.h>
#include "cLava.h"

cLava::cLava()
{
	height = (LAVA_HEIGHT_MAX + LAVA_HEIGHT_MIN)/2;
	ang = 0.0f;
	up = true;
}
cLava::~cLava(){}

void cLava::Load(float terrain_size)
{
	id_Lava=glGenLists(1);
	glNewList(id_Lava,GL_COMPILE);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,    1.0f*16); glVertex3f(terrain_size, 0.0f, 0.0f        );
		glTexCoord2f(1.0f*16, 1.0f*16); glVertex3f(0.0f,         0.0f, 0.0f        );
		glTexCoord2f(1.0f*16,    0.0f); glVertex3f(0.0f,         0.0f, terrain_size);
		glTexCoord2f(0.0f,       0.0f); glVertex3f(terrain_size, 0.0f, terrain_size);
	glEnd();

	glEndList();
}


void cLava::Draw(int tex_id)
{
	glPushMatrix();
	glTranslatef(0.0f,height,0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,tex_id);
	glCallList(id_Lava);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

float cLava::GetHeight()
{
	return height;
}

float cLava::GetHeightMax()
{
	return LAVA_HEIGHT_MAX;
}

void cLava::Update()
{
	if(up)
	{
		ang = fmod(ang+FLOW_SPEED,360);
		height = ((LAVA_HEIGHT_MAX - LAVA_HEIGHT_MIN)/2.0f)*sin(ang*(PI/180)) + (LAVA_HEIGHT_MAX + LAVA_HEIGHT_MIN)/2.0f;
		up = (height < LAVA_HEIGHT_MAX);
	}
	else
	{
		ang = fmod(ang-FLOW_SPEED,360);
		height = ((LAVA_HEIGHT_MAX - LAVA_HEIGHT_MIN)/2.0f)*sin(ang*(PI/180)) + (LAVA_HEIGHT_MAX + LAVA_HEIGHT_MIN)/2.0f;
		up = (height <= LAVA_HEIGHT_MIN);
	}
}