#ifndef _JOINT_HPP_
#define _JOINT_HPP_

#include "XYZ.h"

#include <vector>

enum bodypart{
	head,
	neck,
	leftshoulder,
	leftelbow,
	leftwrist,
	lefthand,
	rightshoulder,
	rightelbow,
	rightwrist,
	righthand,
	abdomen,
	lefthip,
	righthip,
	groin,
	leftknee,
	leftankle,
	leftfoot,
	rightknee,
	rightankle,
	rightfoot
};

class Joint{

public:
	XYZ position;
	XYZ oldposition;
	XYZ realoldposition;
	XYZ velocity;
	XYZ oldvelocity;
	XYZ startpos;
	float blurred;
	float length;
	float mass;
	bool lower;
	bool hasparent;
	bool locked;
	int modelnum;
	bool visible;
	Joint* parent;
	bool sametwist;
	bodypart label;
	int hasgun;
	float delay;
	XYZ velchange;

	Joint();
	void load(FILE* tfile, std::vector<Joint>& joints);
};

#endif
