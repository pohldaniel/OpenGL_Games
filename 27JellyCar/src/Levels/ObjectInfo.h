#ifndef ObjectInfo_H
#define ObjectInfo_H

struct KinematicControls {
	float offsetX, offsetY, secondsPerLoop, startOffset;
	float radiansPerSecond;
};

class ObjectInfo
{
public:

	std::string name;
	float posX, posY, angle, scaleX, scaleY;
	int material;

	ObjectInfo()
	{
		posX = posY = angle = scaleX = scaleY = 0.0f;
		material = 0;
	}

	KinematicControls kinematicControls;
};

#endif