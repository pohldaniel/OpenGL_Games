#pragma once
#include "engine/Camera.h"
#include "engine/Line.h"
#include "RenderableObject.h"

class RaySet : public RenderableObject {

public:

	RaySet();
	~RaySet();
	void init();
	void draw(const Camera& camera) override;
	void update(const float dt) override;
	void deploy(const Vector3f& begin, const Vector3f& end, int id, int numDeployed);

private:

	Line m_line;
	std::vector<Vector4f> lineColors;
	std::vector<Vector4f> colors;
	std::vector<unsigned short> m_pickedIds;
	unsigned short m_numDeployed;
	float ang;


	std::vector<Vector3f> m_startPoints;
	std::vector<Vector3f> m_endPoints;
};