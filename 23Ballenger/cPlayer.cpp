#include "cPlayer.h"

cPlayer::cPlayer()
{
	fade = true;
}
cPlayer::~cPlayer(){}

void cPlayer::Draw(cData *Data,cCamera *Camera,cLava *Lava,cShader *Shader)
{
	glPushMatrix();
	glTranslatef(GetX(),GetY(),GetZ());

	//rotacion por movimiento
	if(cos(GetYaw()*(PI/180)) >= 0.0f) glRotatef(GetPitch(), cos(GetYaw()*(PI/180)),0.0,-sin(GetYaw()*(PI/180)) );
	else glRotatef(GetPitch(), -cos(GetYaw()*(PI/180)),0.0,sin(GetYaw()*(PI/180)) );

	if(Camera->GetState() != STATE_FPS)
	{
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_PLAYER));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,Data->GetID(IMG_PLAYER_NMAP));
		glDisable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		glEnable(GL_BLEND);
		GLUquadricObj *q = gluNewQuadric();
		gluQuadricTexture(q,true);

		Shader->Activate(PROGRAM_COMPLEX_NORMALMAP_LAVAGLOW);
		Shader->SetUniform("colorMap", 0);
		Shader->SetUniform("normalMap", 1);
		Shader->SetUniform("invRadius", 0.0f);
		if(fade) Shader->SetUniform("alpha", Camera->GetDistance()/3.0f);
		else Shader->SetUniform("alpha", 1.0f);
		Shader->SetUniform("lava_height", Lava->GetHeight());
		Shader->SetUniform("posy", GetY()-RADIUS);
		gluSphere(q,RADIUS,16,16);
		Shader->Deactivate();

		gluDeleteQuadric(q);
		glDisable(GL_BLEND);
	}

	glPopMatrix();
}

void cPlayer::SetFade(bool b)
{
	fade = b;
}