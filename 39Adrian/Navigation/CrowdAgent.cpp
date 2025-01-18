#include <DetourCommon.h>
#include <DetourCrowd.h>

#include "../engine/scene/SceneNodeLC.h"
#include "../engine/DebugRenderer.h"
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

void CrowdAgent::OnCrowdPositionUpdate(dtCrowdAgent* ag, float* pos, float dt){

}

void CrowdAgent::OnCrowdVelocityUpdate(dtCrowdAgent* ag, float* pos, float dt)
{
	//assert(ag);
	//if (node_)
	//{
		// Yes, pos actually stores desired velocity in this callback
		Vector3f desiredVelocity{ pos };
		crowdManager_->UpdateAgentVelocity(this, dt, desiredVelocity, ag->desiredSpeed);

		pos[0] = desiredVelocity[0];
		pos[1] = desiredVelocity[1];
		pos[2] = desiredVelocity[2];
	//}
}

void CrowdAgent::DrawDebugGeometry(DebugRenderer* debug, bool depthTest) {
	const Vector3f pos = GetPosition();
	const Vector3f vel = GetActualVelocity();
	const Vector3f desiredVel = GetDesiredVelocity();
	const Vector3f agentHeightVec(0.0f, height_, 0.0f);

	debug->AddLine(pos + 0.5f * agentHeightVec, pos + vel + 0.5f * agentHeightVec, Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	debug->AddLine(pos + 0.25f * agentHeightVec, pos + desiredVel + 0.25f * agentHeightVec, Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	debug->AddCylinder(pos, radius_, height_, HasArrived() ? Vector4f(0.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
}

CrowdAgentTargetState CrowdAgent::GetTargetState() const
{
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent ? (CrowdAgentTargetState)agent->targetState : CA_TARGET_NONE;
}

Vector3f CrowdAgent::GetPosition() const
{
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent ? Vector3f(agent->npos) : Vector3f::ZERO;
}

Vector3f CrowdAgent::GetDesiredVelocity() const
{
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent ? Vector3f(agent->dvel) : Vector3f::ZERO;
}

Vector3f CrowdAgent::GetActualVelocity() const
{
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent ? Vector3f(agent->vel) : Vector3f::ZERO;
}

bool CrowdAgent::HasArrived() const
{
	// Is the agent at or near the end of its path and within its own radius of the goal?
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent && (!agent->ncorners || (agent->cornerFlags[agent->ncorners - 1] & DT_STRAIGHTPATH_END &&
		dtVdist2D(agent->npos, &agent->cornerVerts[(agent->ncorners - 1) * 3]) <=
		agent->params.radius));
}

const dtCrowdAgent* CrowdAgent::GetDetourCrowdAgent() const
{
	return IsInCrowd() ? crowdManager_->GetDetourCrowdAgent(agentCrowdId_) : 0;
}

bool CrowdAgent::IsInCrowd() const
{
	return crowdManager_ && agentCrowdId_ != -1;
}

int CrowdAgent::AddAgentToCrowd(bool force)
{
	//if (!node_ || !crowdManager_ || !crowdManager_->crowd_)
		return -1;

	if (force || !IsInCrowd())
	{
		//URHO3D_PROFILE(AddAgentToCrowd);

		agentCrowdId_ = crowdManager_->AddAgent(this, GetPosition());
		if (agentCrowdId_ == -1)
			return -1;

		//ApplyAttributes();

		previousAgentState_ = GetAgentState();
		previousTargetState_ = GetTargetState();

		// Agent created, but initial state is invalid and needs to be addressed
		if (previousAgentState_ == CrowdAgentState::DT_CROWDAGENT_STATE_INVALID)
		{
			//using namespace CrowdAgentFailure;

			//VariantMap& map = GetEventDataMap();
			//map[P_NODE] = GetNode();
			//map[P_CROWD_AGENT] = this;
			//map[P_CROWD_TARGET_STATE] = previousTargetState_;
			//map[P_CROWD_AGENT_STATE] = previousAgentState_;
			//map[P_POSITION] = GetPosition();
			//map[P_VELOCITY] = GetActualVelocity();
			//crowdManager_->SendEvent(E_CROWD_AGENT_FAILURE, map);
			//Node* node = GetNode();
			//if (node)
				//node->SendEvent(E_CROWD_AGENT_NODE_FAILURE, map);

			// Reevaluate states as handling of event may have resulted in changes
			previousAgentState_ = GetAgentState();
			previousTargetState_ = GetTargetState();
		}

		// Save the initial position to prevent CrowdAgentReposition event being triggered unnecessarily
		previousPosition_ = GetPosition();
	}

	return agentCrowdId_;
}