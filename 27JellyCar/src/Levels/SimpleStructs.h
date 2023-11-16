#pragma once

struct ObjectInfo {
	char name[64];
	float posX;
	float posY;
	float angle;
	float scaleX;
	float scaleY;
	int material;
	bool isPlatform;
	bool isMotor;
	float offsetX;
	float offsetY;
	float secondsPerLoop;
	float startOffset;
	float radiansPerSecond;
};

struct Point {
	float x;
	float y;
	float mass;
};

struct Spring {
	int pt1;
	int pt2;
	float k;
	float damp;
};

struct Triangle {
	int pt0;
	int pt1;
	int pt2;
};

struct SoftBodyAttributes {
	char name[64];

	float colorR;
	float colorG;
	float colorB;

	float massPerPoint;
	float edgeK;
	float edgeDamping;

	bool isKinematic;
	bool shapeMatching;
	float shapeK;
	float shapeDamping;
	float velDamping;

	bool pressureized;
	float pressure;
};

struct SoftBodyInfo {
	SoftBodyAttributes softBodyAttributes;

	int pointCount;
	std::vector<Point> points;

	int springCount;
	std::vector<Spring> springs;

	int polygonCount;
	std::vector<Triangle> polygons;
};