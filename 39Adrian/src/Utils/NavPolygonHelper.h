#pragma once
#include <vector>
#include <fstream>
#include <iomanip>
#include <engine/Vector.h>
#include <engine/Camera.h>
#include <Physics/Physics.h>
#include <Physics/MousePicker.h>

class Shader;

struct EditPolygon {
	int userPointerOffset = 0;
	int size = 0;
	std::vector<Vector3f> edgePoints;
};


struct NavPolygonHelper {
	NavPolygonHelper(const MousePicker& mousePicker, const Camera& camera);

	void draw(Shader* shader);
	bool loadPolygonCache();
	void clearPolygonCache();
	void savePolygonCache();
	void addPolygon(int x, int y, const std::vector<btCollisionObject*>& colliosionFilter);
	void movePolygon(int x, int y, const btCollisionObject* colliosionFilter);
	void remove();
	
	std::vector<EditPolygon> m_editPolygons;
	std::vector<Vector3f> m_edgePoints;
	std::vector<btCollisionObject*> m_collisionObjects;
	EditPolygon* m_currentPolygon;
	float m_markerSize = 20.0f;
	bool m_drawPolygon = false;
	int m_globalUserIndex;

	const MousePicker& mousePicker;
	const Camera& camera;
};