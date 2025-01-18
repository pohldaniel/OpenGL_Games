#include <DetourCommon.h>
#include <DetourCrowd.h>

#include "CrowdAgent.h"

extern const char* NAVIGATION_CATEGORY;

static const CrowdAgentRequestedTarget DEFAULT_AGENT_REQUEST_TARGET_TYPE = CA_REQUESTEDTARGET_NONE;
static const float DEFAULT_AGENT_MAX_SPEED = 0.f;
static const float DEFAULT_AGENT_MAX_ACCEL = 0.f;
static const unsigned DEFAULT_AGENT_QUERY_FILTER_TYPE = 0;
static const unsigned DEFAULT_AGENT_OBSTACLE_AVOIDANCE_TYPE = 0;
static const NavigationQuality DEFAULT_AGENT_AVOIDANCE_QUALITY = NAVIGATIONQUALITY_HIGH;
static const NavigationPushiness DEFAULT_AGENT_NAVIGATION_PUSHINESS = NAVIGATIONPUSHINESS_MEDIUM;

static const unsigned SCOPE_NAVIGATION_QUALITY_PARAMS = 1;
static const unsigned SCOPE_NAVIGATION_PUSHINESS_PARAMS = 2;
static const unsigned SCOPE_BASE_PARAMS = std::numeric_limits<unsigned int>::max() & ~SCOPE_NAVIGATION_QUALITY_PARAMS & ~SCOPE_NAVIGATION_PUSHINESS_PARAMS;

static const char* crowdAgentRequestedTargetTypeNames[] = {
	"None",
	"Position",
	"Velocity",
	0
};

static const char* crowdAgentAvoidanceQualityNames[] = {
	"Low",
	"Medium",
	"High",
	0
};

static const char* crowdAgentPushinessNames[] = {
	"Low",
	"Medium",
	"High",
	"None",
	0
};

CrowdAgent::CrowdAgent() :
	agentCrowdId_(-1),
	requestedTargetType_(DEFAULT_AGENT_REQUEST_TARGET_TYPE),
	updateNodePosition_(true),
	maxAccel_(DEFAULT_AGENT_MAX_ACCEL),
	maxSpeed_(DEFAULT_AGENT_MAX_SPEED),
	radius_(0.0f),
	height_(0.0f),
	queryFilterType_(DEFAULT_AGENT_QUERY_FILTER_TYPE),
	obstacleAvoidanceType_(DEFAULT_AGENT_OBSTACLE_AVOIDANCE_TYPE),
	navQuality_(DEFAULT_AGENT_AVOIDANCE_QUALITY),
	navPushiness_(DEFAULT_AGENT_NAVIGATION_PUSHINESS),
	previousTargetState_(CA_TARGET_NONE),
	previousAgentState_(CrowdAgentState::DT_CROWDAGENT_STATE_WALKING),
	ignoreTransformChanges_(false)
{
	//SubscribeToEvent(E_NAVIGATION_TILE_ADDED, URHO3D_HANDLER(CrowdAgent, HandleNavigationTileAdded));
}

CrowdAgent::~CrowdAgent()
{
	//RemoveAgentFromCrowd();
}

void CrowdAgent::OnCrowdUpdate(dtCrowdAgent* ag, float dt){

}