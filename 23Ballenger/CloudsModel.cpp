#include <GL/glew.h>
#include <imgui.h>

#include "CloudsModel.h"

void CloudsModel::setGui() {

	ImGui::Begin("Clouds controls: ");
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Post Proceesing");
	// ImGui::Text("This is some useful text.");               
	// Display some text (you can use a format strings too)
	ImGui::Checkbox("Post Processing (Gaussian Blur)", &postProcess);
	ImGui::Checkbox("God Rays", &enableGodRays);
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Clouds rendering");

	ImGui::SliderFloat("Coverage", &coverage, 0.0f, 1.0f);
	ImGui::SliderFloat("Speed", &cloudSpeed, 0.0f, 5.0E3);
	ImGui::SliderFloat("Crispiness", &crispiness, 0.0f, 120.0f);
	ImGui::SliderFloat("Curliness", &curliness, 0.0f, 3.0f);
	ImGui::SliderFloat("Density", &density, 0.0f, 0.1f);
	ImGui::SliderFloat("Light absorption", &absorption, 0.0f, 1.5f);
	ImGui::Checkbox("Enable sugar powder effect", &enablePowder);


	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Dome controls");
	ImGui::SliderFloat("Sky dome radius", &earthRadius, 10000.0f, 5000000.0f);
	ImGui::SliderFloat("Clouds bottom height", &sphereInnerRadius, 1000.0f, 15000.0f);
	ImGui::SliderFloat("Clouds top height", &sphereOuterRadius, 1000.0f, 40000.0f);

	if (ImGui::SliderFloat("Clouds frequency", &perlinFrequency, 0.0f, 4.0f))
		generateWeatherMap();

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Clouds colors");
	Vector3f * cloudBottomColor = &cloudColorBottom;
	ImGui::ColorEdit3("Cloud color", (float*)cloudBottomColor); // Edit 3 floats representing a color

	ImGui::End();
}

CloudsModel::CloudsModel() {
	sceneSeed = Vector3f(0.0f, 0.0f, 0.0f);
	initVariables();
	initShaders();
	generateModelTextures();
}

void CloudsModel::initShaders(){
	volumetricCloudsShader = new Shader("shaders/volumetric_clouds.comp");
	//postProcessingShader = new ScreenSpaceShader("shaders/clouds_post.frag");
	//compute shaders
	weatherShader = new Shader("shaders/weather.comp");

}

void CloudsModel::generateModelTextures(){
	/////////////////// TEXTURE GENERATION //////////////////
	if (!perlinTex) {
		//compute shaders
		Shader comp("shaders/perlinworley.comp");
		

		//make texture

		Texture::CreateTexture3D(this->perlinTex, 128, 128, 128, GL_RGBA8, GL_RGBA, GL_FLOAT);
		Texture::SetWrapMode(this->perlinTex, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(this->perlinTex, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
		//this->perlinTex = generateTexture3D(128, 128, 128);
		//compute
		comp.use();
		comp.loadVector("u_resolution", Vector3f(128, 128, 128));
		std::cout << "computing perlinworley!" << std::endl;
		glActiveTexture(GL_TEXTURE0);
		comp.loadInt("outVolTex", 0);
		glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	  	glBindImageTexture(0, this->perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		std::cout << "computed!!" << std::endl;
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!worley32) {
		//compute shaders
		Shader worley_git("shaders/worley.comp");
		

		//make texture
		//this->worley32 = generateTexture3D(32, 32, 32);

		Texture::CreateTexture3D(this->worley32, 32, 32, 32, GL_RGBA8, GL_RGBA, GL_FLOAT);
		Texture::SetWrapMode(this->worley32, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(this->worley32, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);

		//compute
		worley_git.use();
		//worley_git.setVec3("u_resolution", glm::vec3(32, 32, 32));
		glActiveTexture(GL_TEXTURE0);
	  	glBindTexture(GL_TEXTURE_3D, this->worley32);
	  	glBindImageTexture(0, this->worley32, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		std::cout << "computing worley 32!" << std::endl;
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		std::cout << "computed!!" << std::endl;
		glGenerateMipmap(GL_TEXTURE_3D);
	}


	////////////////////////

	if (!weatherTex) {
		//make texture

		Texture::CreateTexture2D(this->weatherTex, 1024, 1024, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		Texture::SetWrapMode(this->weatherTex, GL_REPEAT);

		//this->weatherTex = generateTexture2D(1024, 1024);

		//compute
		generateWeatherMap();

		seed = sceneSeed;
		oldSeed = seed;
	}
}

CloudsModel::~CloudsModel()
{
	delete volumetricCloudsShader;
	delete weatherShader;
}

void CloudsModel::update()
{
	seed = sceneSeed;
	if (seed != oldSeed) {
		generateWeatherMap();
		oldSeed = seed;
	}
}

void CloudsModel::generateWeatherMap() {
	glBindImageTexture(0, weatherTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	weatherShader->use();
	weatherShader->loadVector("seed", sceneSeed);
	weatherShader->loadFloat("perlinFrequency", perlinFrequency);
	std::cout << "computing weather!" << std::endl;
	glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
	std::cout << "weather computed!!" << std::endl;
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void CloudsModel::initVariables()
{
	cloudSpeed = 450.0;
	coverage = 0.45;
	crispiness = 40.;
	curliness = .1;
	density = 0.02;
	absorption = 0.35;

	earthRadius = 600000.0;
	sphereInnerRadius = 5000.0;
	sphereOuterRadius = 17000.0;

	perlinFrequency = 0.8;

	enableGodRays = false;
	enablePowder = false;
	postProcess = true;

	seed = Vector3f(0.0, 0.0, 0.0);
	oldSeed = Vector3f(0.0, 0.0, 0.0);

	cloudColorTop = (Vector3f(169., 149., 149.)*(1.5f / 255.f));
	cloudColorBottom = (Vector3f(65., 70., 80.)*(1.5f / 255.f));

	weatherTex = 0;
	perlinTex = 0;
	worley32 = 0;
}
