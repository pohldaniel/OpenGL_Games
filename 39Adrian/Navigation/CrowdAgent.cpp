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
static const unsigned SCOPE_SEPARATION_WEIGHT = 3;
static const unsigned SCOPE_BASE_PARAMS = std::numeric_limits<unsigned int>::max() & ~SCOPE_NAVIGATION_QUALITY_PARAMS & ~SCOPE_NAVIGATION_PUSHINESS_PARAMS & ~SCOPE_SEPARATION_WEIGHT;

Vector3f CrowdAgent::NearestPos;

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

}

CrowdAgent::~CrowdAgent(){
	//RemoveAgentFromCrowd();
}

void CrowdAgent::OnCrowdPositionUpdate(dtCrowdAgent* ag, float* , float dt){
	
	CrowdAgent* self(this);

	Vector3f newPos(ag->npos);
	Vector3f newVel(ag->vel);		
	crowdManager_->UpdateAgentPosition(this, dt, newPos);

	if (isActive()){
		m_active = true;
		previousPosition_ = newPos;
		OnPositionVelocityUpdate(newPos, newVel, this);	
		return;
	}
		
	if (m_active) {
		if (HasArrived(10.0f)) {
			m_active = false;
			resetAgent();
			OnInactive();
		}
	}
}

void CrowdAgent::OnCrowdVelocityUpdate(dtCrowdAgent* ag, float* pos, float dt){	
	Vector3f desiredVelocity{ pos };
	crowdManager_->UpdateAgentVelocity(this, dt, desiredVelocity, ag->desiredSpeed);

	pos[0] = desiredVelocity[0];
	pos[1] = desiredVelocity[1];
	pos[2] = desiredVelocity[2];
}

void CrowdAgent::DrawDebugGeometry(DebugRenderer* debug, bool depthTest) {
	const Vector3f pos = GetPosition();
	const Vector3f vel = GetActualVelocity();
	const Vector3f desiredVel = GetDesiredVelocity();
	const Vector3f agentHeightVec(0.0f, height_, 0.0f);

	debug->AddLine(pos + 0.5f * agentHeightVec, pos + vel + 0.5f * agentHeightVec, Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	debug->AddLine(pos + 0.25f * agentHeightVec, pos + desiredVel + 0.25f * agentHeightVec, Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	debug->AddCylinder(pos, radius_, height_, isActive() ? Vector4f(1.0f, 1.0f, 1.0f, 1.0f) : Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
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

bool CrowdAgent::HasArrived(const float scale) const
{
	// Is the agent at or near the end of its path and within its own radius of the goal?
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	return agent && (!agent->ncorners || (agent->cornerFlags[agent->ncorners - 1] & DT_STRAIGHTPATH_END &&
		dtVdist2D(agent->npos, &agent->cornerVerts[(agent->ncorners - 1) * 3]) <=
		agent->params.radius * scale));
}

const dtCrowdAgent* CrowdAgent::GetDetourCrowdAgent() const
{
	return IsInCrowd() ? crowdManager_->GetDetourCrowdAgent(agentCrowdId_) : 0;
}

bool CrowdAgent::IsInCrowd() const
{
	return crowdManager_ && agentCrowdId_ != -1;
}

int CrowdAgent::AddAgentToCrowd(bool force, const Vector3f& initialPosition){

	if (force || !IsInCrowd()){
		agentCrowdId_ = crowdManager_->AddAgent(this, initialPosition);
		if (agentCrowdId_ == -1)
			return -1;

		previousAgentState_ = GetAgentState();
		previousTargetState_ = GetTargetState();
		previousPosition_ = GetPosition();
	}
	UpdateParameters();
	return agentCrowdId_;
}

void CrowdAgent::UpdateParameters(unsigned scope){
	const dtCrowdAgent* agent = GetDetourCrowdAgent();
	if (agent){
		dtCrowdAgentParams params = agent->params;

		if (scope & SCOPE_NAVIGATION_QUALITY_PARAMS){
			switch (navQuality_){
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

		if (scope & SCOPE_NAVIGATION_PUSHINESS_PARAMS){
			switch (navPushiness_){
			case NAVIGATIONPUSHINESS_LOW:
				params.collisionQueryRange = radius_ * 16.0f;
				break;

			case NAVIGATIONPUSHINESS_MEDIUM:
				params.collisionQueryRange = radius_ * 8.0f;
				break;

			case NAVIGATIONPUSHINESS_HIGH:
				params.collisionQueryRange = radius_ * 1.0f;
				break;

			case NAVIGATIONPUSHINESS_NONE:
				params.collisionQueryRange = radius_ * 1.0f;
				break;
			}
		}

		if (scope & SCOPE_SEPARATION_WEIGHT) {
			params.separationWeight = separationWeight_;
		}

		if (scope & SCOPE_BASE_PARAMS){
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

void CrowdAgent::SetSeparationWeight(float separationWeight) {
	if (separationWeight != separationWeight_ && separationWeight > 0.f) {
		separationWeight_ = separationWeight;
		UpdateParameters(SCOPE_SEPARATION_WEIGHT);
	}
}

void CrowdAgent::SetNavigationPushiness(NavigationPushiness val){
	if (val != navPushiness_){
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
			NearestPos = crowdManager_->FindNearestPoint(position, queryFilterType_, &nearestRef);
			crowdManager_->GetCrowd()->requestMoveTarget(agentCrowdId_, nearestRef, NearestPos.getVec());
		}
	}
}

void CrowdAgent::setOnPositionVelocityUpdate(std::function<void(const Vector3f& pos, const Vector3f& vel, CrowdAgent* agent)> fun) {
	OnPositionVelocityUpdate = fun;
}

void CrowdAgent::setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, const unsigned int index, CrowdAgent* agent)> fun) {
	OnCrowdFormation = fun;
}

void CrowdAgent::setOnTarget(std::function<void(const Vector3f& pos)> fun) {
	OnTarget = fun;
}

void CrowdAgent::setOnInactive(std::function<void()> fun) {
	OnInactive = fun;
}

void CrowdAgent::resetAgent() {
	crowdManager_->GetCrowd()->resetMoveTarget(agentCrowdId_);
}

bool CrowdAgent::isActive() {
	return crowdManager_->GetCrowd()->isActive(agentCrowdId_, 0.05f);
}

const Vector3f& CrowdAgent::GetNearestPos() {
	return NearestPos;
}