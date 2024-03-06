#include "Joint.h"
#include "Utils/binio.h"

Joint::Joint()
	: blurred(0)
	, length(0)
	, mass(0)
	, lower(false)
	, hasparent(false)
	, locked(false)
	, modelnum(0)
	, visible(false)
	, parent(nullptr)
	, sametwist(false)
	, label(head)
	, hasgun(0)
	, delay(0)
{
}

void Joint::load(FILE* tfile, std::vector<Joint>& joints)
{
	int parentID;

	funpackf(tfile, "Bf Bf Bf Bf Bf", &position.x, &position.y, &position.z, &length, &mass);
	funpackf(tfile, "Bb Bb", &hasparent, &locked);
	funpackf(tfile, "Bi", &modelnum);
	funpackf(tfile, "Bb Bb", &visible, &sametwist);
	funpackf(tfile, "Bi Bi", &label, &hasgun);
	funpackf(tfile, "Bb", &lower);
	funpackf(tfile, "Bi", &parentID);
	if (hasparent) {
		parent = &joints[parentID];
	}
	else {
		parent = nullptr;
	}
	velocity = 0;
	oldposition = position;
	startpos = position;
}
