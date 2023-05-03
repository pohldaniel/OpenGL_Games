#include "cScene.h"

cScene::cScene(void){}
cScene::~cScene(void){}


void cScene::LoadLevel(int level, cTerrain *Terrain, cLava *Lava, float zfar)
{
	Terrain->Load(level);
	Skybox.Load(zfar);
	Lava->Load(TERRAIN_SIZE);
}

void DrawOGLAxis()
{
	GLUquadricObj *q = gluNewQuadric();

	//z axis (blue)
	glPushMatrix();
	glColor3f(0.0,0.0,1.0);
	gluCylinder(q,0.1f,0.0f,3.0f,3,1);
	glPopMatrix();

	//x axis (red)
	glPushMatrix();
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f); //el angulo positivo por la regla de la mano derecha
	glColor3f(1.0,0.0,0.0);
	gluCylinder(q,0.1f,0.0f,3.0f,3,1);
	glPopMatrix();

	//y axis (green)
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); //el angulo negativo por la regla de la mano derecha
	glColor3f(0.0,1.0,0.0);
	gluCylinder(q,0.1f,0.0f,3.0f,3,1);
	glPopMatrix();

	gluDeleteQuadric(q);
	glColor3f(1.0,1.0,1.0); //devolvemos el color a blanco para no afectar a las futuras texturas
}

void cScene::Draw(cData *Data, cTerrain *Terrain, cLava *Lava, cShader *Shader, Coord playerPos)
{
	glDisable(GL_LIGHTING);

	glDisable(GL_DEPTH_TEST); //desactivo el depth test para que el jugador no vea como el skymap corta con el escenario
	Skybox.Draw(Data->GetID(IMG_SKYBOX),playerPos.x,playerPos.y,playerPos.z);
	glEnable(GL_DEPTH_TEST);

	//DrawOGLAxis();

	Lava->Draw(Data->GetID(IMG_LAVA));

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_GRASS));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_ROCK));
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	Shader->Activate(PROGRAM_SIMPLE_TERRAIN);
	Shader->SetUniform("tex_top", 0);
	Shader->SetUniform("tex_side", 1);
	Shader->SetUniform("height", Lava->GetHeight());
	Shader->SetUniform("hmax", Lava->GetHeightMax());
	Terrain->Draw();
	Shader->Deactivate();

	glEnable(GL_LIGHTING);
}