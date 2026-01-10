#pragma once

#include <string>
#include <unordered_map>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <engine/TileSet.h>
#include <engine/Framebuffer.h>

#include "DataStructs.h"
#include "Zone.h"

class Overworld : public Zone {

	static const int MAX_POINTS = 1000;

public:

	Overworld(const Camera& camera, const bool initDebug = true);
	~Overworld();

	void draw() override;
	void resize() override;
	void loadZone(const std::string path, const std::string currentTileset) override;
	
private:

	void updatePoints();
	void initDebug();

	unsigned int m_vao, m_vbo;
	Vector3f* m_pointBatch, * m_pointBatchPtr;
	uint32_t m_pointCount;

	Framebuffer m_mainRenderTarget;
};