#pragma once

#include <vector>
#include <array>
#include <engine/interfaces/ICounter.h>
#include <Scene/ObjectNode.h>

enum LightType2 {
	DIRECTIONAL2,
	POINTAW2,
	SPOT2
};

struct LightBuffer {
	float ambient[4] = { 0.0f };
	float diffuse[4] = { 0.0f };
	float specular[4] = { 0.0f };
	float position[3] = { 0.0f };
	float angle = 0.0f;
	float direction[3] = { 0.0f };
	float padding = 0.0f;
	LightType2 type;
	bool enabled;
};

class Light : public ObjectNode, private ICounter<Light> {

public:

	Light();

	void update(const float dt);

	//friend bool operator== (const Light& l1, const Light& l2);


	void cleanup();
	
	void updateLightUbo(unsigned int& ubo, size_t size);
	void setAmbient(std::array<float, 4> ambient) const;
	void setDiffuse(std::array<float, 4> diffuse) const;
	void setSpecular(std::array<float, 4> specular) const;
	void setPosition(const Vector3f& position) override;
	void setDirection(std::array<float, 3> direction) const;
	void setAngle(float angle) const;
	

	static std::vector<Light>& GetLights();
	static void SetLights(const std::vector<Light>& lights);
	static Light& AddLight(const LightBuffer& light = { {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 30.0f , {0.0f, -1.0f, 0.0f} , 0.0f, LightType2::POINTAW2, true });
	static void Print();

	static std::vector<Light> Lights;
	static LightBuffer Buffer[20];
};