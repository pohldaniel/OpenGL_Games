#pragma once


enum editortypes
{
	typeactive,
	typesitting,
	typesittingwall,
	typesleeping,
	typedead1,
	typedead2,
	typedead3,
	typedead4
};

#define passivetype 0
#define guardtype 1
#define searchtype 2
#define attacktype 3
#define attacktypecutoff 4
#define playercontrolled 5
#define gethelptype 6
#define getweapontype 7
#define pathfindtype 8

enum bodypart
{
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