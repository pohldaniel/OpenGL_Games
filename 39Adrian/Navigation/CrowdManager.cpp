#include <DetourCrowd.h>

#include "NavigationMesh.h"
#include "CrowdManager.h"
#include "CrowdAgent.h"

extern const char* NAVIGATION_CATEGORY;

static const unsigned DEFAULT_MAX_AGENTS = 512;
static const float DEFAULT_MAX_AGENT_RADIUS = 0.f;

const char* filterTypesStructureElementNames[] =
{
	"Query Filter Type Count",
	"   Include Flags",
	"   Exclude Flags",
	"   >AreaCost",
	0
};

const char* obstacleAvoidanceTypesStructureElementNames[] =
{
	"Obstacle Avoid. Type Count",
	"   Velocity Bias",
	"   Desired Velocity Weight",
	"   Current Velocity Weight",
	"   Side Bias Weight",
	"   Time of Impact Weight",
	"   Time Horizon",
	"   Grid Size",
	"   Adaptive Divs",
	"   Adaptive Rings",
	"   Adaptive Depth",
	0
};

void CrowdAgentUpdateCallback(dtCrowdAgent* ag, float dt)
{
	static_cast<CrowdAgent*>(ag->params.userData)->OnCrowdUpdate(ag, dt);
}


CrowdManager::CrowdManager() :
	crowd_(0),
	navigationMeshId_(0),
	maxAgents_(DEFAULT_MAX_AGENTS),
	maxAgentRadius_(DEFAULT_MAX_AGENT_RADIUS),
	numQueryFilterTypes_(0),
	numObstacleAvoidanceTypes_(0)
{
	// The actual buffer is allocated inside dtCrowd, we only track the number of "slots" being configured explicitly
	numAreas_.reserve(DT_CROWD_MAX_QUERY_FILTER_TYPE);
	for (unsigned i = 0; i < DT_CROWD_MAX_QUERY_FILTER_TYPE; ++i)
		numAreas_.push_back(0);
}

CrowdManager::~CrowdManager()
{
	dtFreeCrowd(crowd_);
	crowd_ = 0;
}