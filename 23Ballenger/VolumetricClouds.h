#pragma once

#include "_texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <algorithm>
#include "drawableObject.h"
#include "CloudsModel.h"

#include "engine/Texture.h"
#include "engine/Vector.h"
#include "engine/Camera.h"
#include "engine/Framebuffer.h"

//VolumetricClouds handles the FrameBufferObjects (and textures) where the clouds will be rendered, and it's responsible to set up the uniforms and calling the draw command.
class VolumetricClouds : public drawableObject
{
public:
	VolumetricClouds(int SW, int SH, CloudsModel * model);
	virtual void draw(const Camera& camera, const glm::mat4& projection2, const glm::mat4& view2, const glm::vec3& campos2, const glm::vec3& lightpos2);
	~VolumetricClouds();

	enum cloudsTextureNames {fragColor, bloom, alphaness, cloudDistance};

	unsigned int getCloudsTexture() { 
		return (model->postProcess ? cloudsBuffer.getColorTexture(0) : getCloudsRawTexture());
	}

	unsigned int getCloudsTexture(int i) {
		return m_textureSet[i].getTexture();
	}

	unsigned int getCloudsRawTexture(){
		return m_textureSet[0].getTexture();
	}



private:
	int SCR_WIDTH, SCR_HEIGHT;

	//TextureSet * cloudsFBO;
	//FrameBufferObject *cloudsPostProcessingFBO;

	Texture m_textureSet[4];
	Framebuffer cloudsBuffer;

	CloudsModel * model;
	int time = 0;

};

