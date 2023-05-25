#pragma once
#include "engine/Camera.h"
#include "engine/Line.h"
#include "RenderableObject.h"

#include "cColumn.h"
#include "cPortal.h"

class RaySet : public RenderableObject {



public:

	RaySet(const cPortal& portal);
	~RaySet();
	void init();
	void draw(const Camera& camera) override;
	void update(const float dt, std::vector<cColumn>& columns, cPortal& portal);
	void deploy(const Vector3f& begin, const Vector3f& end, int id, int numDeployed);

	Line m_line;

	std::vector<Vector4f> lineColors;
	std::vector<Vector4f> colors;
	std::vector<unsigned short> m_pickedIds;
	unsigned short m_numDeployed;
	float ang;
	const cPortal& m_portal;
};