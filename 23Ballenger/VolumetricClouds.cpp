#include "VolumetricClouds.h"
#include "Application.h"
#include "Constants.h"

#define TIMETO(CODE, TASK) 	t1 = glfwGetTime(); CODE; t2 = glfwGetTime(); std::cout << "Time to " + std::string(TASK) + " :" << (t2 - t1)*1e3 << "ms" << std::endl;



VolumetricClouds::VolumetricClouds(int SW, int SH, CloudsModel * model): SCR_WIDTH(SW), SCR_HEIGHT(SH), model(model) {

	//cloudsFBO = new TextureSet(SW, SH, 4);
	//cloudsPostProcessingFBO = new FrameBufferObject(Application::Width, Application::Height, 2);

	m_textureSet[0].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[1].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[2].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[3].createEmptyTexture(Application::Width, Application::Height, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	m_textureSet[0].setWrapMode(GL_REPEAT);
	m_textureSet[1].setWrapMode(GL_REPEAT);
	m_textureSet[2].setWrapMode(GL_REPEAT);
	m_textureSet[3].setWrapMode(GL_REPEAT);

	cloudsBuffer.create(Application::Width, Application::Height);
	cloudsBuffer.attachTexture(AttachmentTex::RGBA32F);
	cloudsBuffer.attachTexture(AttachmentTex::DEPTH32F);

	Texture::SetFilter(cloudsBuffer.getColorTexture(0), GL_LINEAR_MIPMAP_LINEAR);
	Texture::SetFilter(cloudsBuffer.getDepthTexture(), GL_LINEAR);
}


void VolumetricClouds::draw(const Camera& camera, const glm::mat4& projection2, const glm::mat4& view2, const glm::vec3& campos2, const glm::vec3& lightpos2) {

	
}



VolumetricClouds::~VolumetricClouds()
{
}
