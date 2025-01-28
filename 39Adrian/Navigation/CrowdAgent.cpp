#include <DetourCommon.h>
#include <DetourCrowd.h>

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

void CrowdAgent::OnCrowdPositionUpdate(dtCrowdAgent* ag, float* , float dt){

	//assert(ag);
	//if (node_)
	//{
		// Use pointer to self to check for destruction after sending events
		CrowdAgent* self(this);

		Vector3f newPos(ag->npos);
		Vector3f newVel(ag->vel);

		crowdManager_->UpdateAgentPosition(this, dt, newPos);

		// Notify parent node of the reposition
		if (newPos != previousPosition_)
		{
			previousPosition_ = newPos;
			OnPositionVelocityUpdate(newPos, newVel);
			//if (updateNodePosition_)
			{
				ignoreTransformChanges_ = true;
				//node_->SetWorldPosition(newPos);
				ignoreTransformChanges_ = false;
			}
			/*HandleCrowdAgentReposition(newPos, newVel, HasArrived(), dt);
			using namespace CrowdAgentReposition;

			VariantMap& map = GetEventDataMap();
			map[P_NODE] = node_;
			map[P_CROWD_AGENT] = this;
			map[P_POSITION] = newPos;
			map[P_VELOCITY] = newVel;
			map[P_ARRIVED] = HasArrived();
			map[P_TIMESTEP] = dt;
			crowdManager_->SendEvent(E_CROWD_AGENT_REPOSITION, map);
			if (self.Expired())
				return;
			node_->SendEvent(E_CROWD_AGENT_NODE_REPOSITION, map);*/
			//if (Expired())
				//return;
		}

		// Send a notification event if we've reached the destination
		CrowdAgentTargetState newTargetState = GetTargetState();
		CrowdAgentState newAgentState = GetAgentState();
		if (newAgentState != previousAgentState_ || newTargetState != previousTargetState_)
		{
			/*using namespace CrowdAgentStateChanged;

			VariantMap& map = GetEventDataMap();
			map[P_NODE] = node_;
			map[P_CROWD_AGENT] = this;
			map[P_CROWD_TARGET_STATE] = newTargetState;
			map[P_CROWD_AGENT_STATE] = newAgentState;
			map[P_POSITION] = newPos;
			map[P_VELOCITY] = newVel;
			crowdManager_->SendEvent(E_CROWD_AGENT_STATE_CHANGED, map);
			if (self.Expired())
				return;
			node_->SendEvent(E_CROWD_AGENT_NODE_STATE_CHANGED, map);
			if (self.Expired())
				return;*/

			// Send a failure event if either state is a failed status
			if (newAgentState == CrowdAgentState::DT_CROWDAGENT_STATE_INVALID || newTargetState == CA_TARGET_FAILED)
			{
				/*VariantMap& map = GetEventDataMap();
				map[P_NODE] = node_;
				map[P_CROWD_AGENT] = this;
				map[P_CROWD_TARGET_STATE] = newTargetState;
				map[P_CROWD_AGENT_STATE] = newAgentState;
				map[P_POSITION] = newPos;
				map[P_VELOCITY] = newVel;
				crowdManager_->SendEvent(E_CROWD_AGENT_FAILURE, map);
				if (self.Expired())
					return;
				node_->SendEvent(E_CROWD_AGENT_NODE_FAILURE, map);
				if (self.Expired())
					return;*/
			}

			// State may have been altered during the handling of the event
			previousAgentState_ = GetAgentState();
			previousTargetState_ = GetTargetState();
		}
	//}
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

CrowdAgentState CrowdAgent::GetAgentState() const
{
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent ? (CrowdAgentState)agent->state : CrowdAgentState::DT_CROWDAGENT_STATE_INVALID;
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

int CrowdAgent::AddAgentToCrowd(bool force, const Vector3f& initialPosition)
{
	//if (!node_ || !crowdManager_ || !crowdManager_->crowd_)
		//return -1;

	if (force || !IsInCrowd())
	{
		//URHO3D_PROFILE(AddAgentToCrowd);

		agentCrowdId_ = crowdManager_->AddAgent(this, initialPosition);
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
	UpdateParameters();
	return agentCrowdId_;
}

void CrowdAgent::UpdateParameters(unsigned scope)
{
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	if (agent)
	{
		dtCrowdAgentParams params = agent->params;

		if (scope & SCOPE_NAVIGATION_QUALITY_PARAMS)
		{
			switch (navQuality_)
			{
			case NAVIGATIONQUALITY_LOW:
				params.updateFlags = 0
					| DT_CROWD_OPTIMIZE_VIS
					| DT_CROWD_ANTICIPATE_TURNS;
				break;

			case NAVIGATIONQUALITY_MEDIUM:
				params.updateFlags = 0
					| DT_CROWD_OPTIMIZE_TOPO
					| DT_CROWD_OPTIMIZE_VIS
					| DT_CROWD_ANTICIPATE_TURNS
					| DT_CROWD_SEPARATION;
				break;

			case NAVIGATIONQUALITY_HIGH:
				params.updateFlags = 0
					// Path finding
					| DT_CROWD_OPTIMIZE_TOPO
					| DT_CROWD_OPTIMIZE_VIS
					// Steering
					| DT_CROWD_ANTICIPATE_TURNS
					| DT_CROWD_SEPARATION
					// Velocity planning
					| DT_CROWD_OBSTACLE_AVOIDANCE;
				break;
			}
		}

		if (scope & SCOPE_NAVIGATION_PUSHINESS_PARAMS)
		{
			switch (navPushiness_)
			{
			case NAVIGATIONPUSHINESS_LOW:
				params.separationWeight = 4.0f;
				params.collisionQueryRange = radius_ * 16.0f;
				break;

			case NAVIGATIONPUSHINESS_MEDIUM:
				params.separationWeight = 2.0f;
				params.collisionQueryRange = radius_ * 8.0f;
				break;

			case NAVIGATIONPUSHINESS_HIGH:
				params.separationWeight = 0.5f;
				params.collisionQueryRange = radius_ * 1.0f;
				break;

			case NAVIGATIONPUSHINESS_NONE:
				params.separationWeight = 0.0f;
				params.collisionQueryRange = radius_ * 1.0f;
				break;
			}
		}

		if (scope & SCOPE_BASE_PARAMS)
		{
			params.radius = radius_;
			params.height = height_;
			params.maxAcceleration = maxAccel_;
			params.maxSpeed = maxSpeed_;
			params.pathOptimizationRange = radius_ * 30.0f;
			params.queryFilterType = (unsigned char)queryFilterType_;
			params.obstacleAvoidanceType = (unsigned char)obstacleAvoidanceType_;
		}

		crowdManager_->GetCrowd()->updateAgentParameters(agentCrowdId_, &params);
	}
}

void CrowdAgent::SetMaxAccel(float maxAccel){
	if (maxAccel != maxAccel_ && maxAccel >= 0.f){
		maxAccel_ = maxAccel;
		UpdateParameters(SCOPE_BASE_PARAMS);
	}
}

void CrowdAgent::SetMaxSpeed(float maxSpeed){
	if (maxSpeed != maxSpeed_ && maxSpeed >= 0.f){
		maxSpeed_ = maxSpeed;
		UpdateParameters(SCOPE_BASE_PARAMS);
	}
}

void CrowdAgent::SetRadius(float radius){
	if (radius != radius_ && radius > 0.f){
		radius_ = radius;
		UpdateParameters(SCOPE_BASE_PARAMS | SCOPE_NAVIGATION_PUSHINESS_PARAMS);
	}
}

void CrowdAgent::SetHeight(float height){
	if (height != height_ && height > 0.f){
		height_ = height;
		UpdateParameters(SCOPE_BASE_PARAMS);
	}
}

void CrowdAgent::SetNavigationPushiness(NavigationPushiness val)
{
	if (val != navPushiness_)
	{
		navPushiness_ = val;
		UpdateParameters(SCOPE_NAVIGATION_PUSHINESS_PARAMS);
	}
}

void CrowdAgent::SetTargetPosition(const Vector3f& position){
	if (position != targetPosition_ || CA_REQUESTEDTARGET_POSITION != requestedTargetType_){
		targetPosition_ = position;
		requestedTargetType_ = CA_REQUESTEDTARGET_POSITION;

		if (!IsInCrowd())
			AddAgentToCrowd();

		if (IsInCrowd()){
			dtPolyRef nearestRef;
			Vector3f nearestPos = crowdManager_->FindNearestPoint(position, queryFilterType_, &nearestRef);
			crowdManager_->GetCrowd()->requestMoveTarget(agentCrowdId_, nearestRef, nearestPos.getVec());
		}
	}
}

void CrowdAgent::HandleCrowdAgentReposition(const Vector3f& position, const Vector3f& velocity, bool isArrived, float dt) {
	//Node* node = static_cast<Node*>(eventData[P_NODE].GetPtr());
	//CrowdAgent* agent = static_cast<CrowdAgent*>(eventData[P_CROWD_AGENT].GetPtr());
	//Vector3f velocity = eventData[P_VELOCITY].GetVector3();
	//float timeStep = eventData[P_TIMESTEP].GetFloat();
}

void CrowdAgent::setOnPositionVelocityUpdate(std::function<void(const Vector3f& pos, const Vector3f& vel)> fun) {
	OnPositionVelocityUpdate = fun;
}

void CrowdAgent::setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, const unsigned int index, CrowdAgent* agent)> fun) {
	OnCrowdFormation = fun;
}