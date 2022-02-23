#pragma once

#include "StateMachine.h"
#include "AssetManger.h"

class Pause : public State {
public:
	Pause(StateMachine& machine);
	~Pause();

	virtual void FixedUpdate() override;
	virtual void Update() override;
	virtual void Render(unsigned int &m_frameBuffer) override;

private:
	std::unordered_map<std::string, unsigned int> m_Sprites;

	AssetManager<Texture> m_TextureManager;
	Quad *m_quad;
	Shader *m_shader;

	void InitSprites();
	void InitAssets();
};