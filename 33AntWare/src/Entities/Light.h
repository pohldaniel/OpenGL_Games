#pragma once

#include <vector>
#include <array>
#include <engine/interfaces/ICounter.h>
#include <Scene/SceneNode.h>

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
	int type = static_cast<LightType2>(1);
	bool enabled = 0;
	bool padding0 = false;
	short padding1 = 0;
	float padding2, padding3, padding4;
};

class Light : public SceneNode {

public:

	Light();
	Light(Light const& rhs);
	Light& operator=(const Light& rhs);
	Light(Light&& rhs);
	Light& operator=(Light&& rhs);
	~Light() = default;

	void update(const float dt);
	friend bool operator== (const Light& l1, const Light& l2);
	void cleanup();
	
	void setUboAmbient(std::array<float, 4> ambient) const;
	void setUboDiffuse(std::array<float, 4> diffuse) const;
	void setUboSpecular(std::array<float, 4> specular) const;
	void setUboPosition(const Vector3f& position);
	void setUboDirection(std::array<float, 3> direction) const;
	void setUboAngle(float angle) const;
	void setUboType(int type);
	void toggle() const;
	void print();

	static std::vector<Light>& GetLights();
	static void SetLights(const std::vector<Light>& lights);
	static Light& AddLight(const LightBuffer& light = { {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 30.0f , {0.0f, -1.0f, 0.0f}, LightType2::POINTAW2, false, false, 0, 0.0f, 0.0f, 0.0f });
	static void Print();
	static void UpdateLightUbo(unsigned int& ubo, size_t size);

	int m_index;
	bool m_isStatic;

	static std::vector<Light> Lights;
	static LightBuffer Buffer[20];
};