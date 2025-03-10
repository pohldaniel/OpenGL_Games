#include <DetourCommon.h>
#include <DetourCrowd.h>

#include "../engine/DebugRenderer.h"
#include "CrowdAgent.h"
#include "NavigationMesh.h"

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

CrowdAgent::CrowdAgent() : 
	m_crowdManager(nullptr),
	m_agentCrowdId(-1),
	m_requestedTargetType(DEFAULT_AGENT_REQUEST_TARGET_TYPE),
	m_maxAccel(DEFAULT_AGENT_MAX_ACCEL),
	m_maxSpeed(DEFAULT_AGENT_MAX_SPEED),
	m_radius(0.0f),
	m_height(0.0f),
	m_queryFilterType(DEFAULT_AGENT_QUERY_FILTER_TYPE),
	m_obstacleAvoidanceType(DEFAULT_AGENT_OBSTACLE_AVOIDANCE_TYPE),
	m_navQuality(DEFAULT_AGENT_AVOIDANCE_QUALITY),
	m_navPushiness(DEFAULT_AGENT_NAVIGATION_PUSHINESS),
	m_previousTargetState(CA_TARGET_NONE),
	m_previousAgentState(CrowdAgentState::DT_CROWDAGENT_STATE_WALKING),
	m_active(false),
	m_forceArrived(false),
	m_forceActive(false)
{

}

CrowdAgent::~CrowdAgent(){

}

void CrowdAgent::OnCrowdPositionUpdate(dtCrowdAgent* ag, float* , float dt){
	
	CrowdAgent* self(this);
	Vector3f newPos(ag->npos);
	Vector3f newVel(ag->vel);	

	m_crowdManager->updateAgentPosition(this, dt, newPos);

	if (newPos != m_previousPosition){
		m_previousPosition = newPos;
		OnAddAgent(newPos);
	}
	
	if (isActive()){
		m_active = true;
		m_forceActive = false;
		m_previousPosition = newPos;
		OnPositionVelocityUpdate(newPos, newVel);	
		return;
	}
		
	if (m_active) {
		if (hasArrived(10.0f)) {
			resetAgent();
			OnInactive();
		}
	}
}

void CrowdAgent::OnCrowdVelocityUpdate(dtCrowdAgent* ag, float* pos, float dt){	
	Vector3f desiredVelocity{ pos };
	m_crowdManager->updateAgentVelocity(this, dt, desiredVelocity, ag->desiredSpeed);

	pos[0] = desiredVelocity[0];
	pos[1] = desiredVelocity[1];
	pos[2] = desiredVelocity[2];
}

void CrowdAgent::OnRenderDebug() {
	const Vector3f pos = getPosition();
	const Vector3f vel = getActualVelocity();
	const Vector3f desiredVel = getDesiredVelocity();
	const Vector3f agentHeightVec(0.0f, m_height, 0.0f);

	DebugRenderer::Get().AddLine(pos + 0.5f * agentHeightVec, pos + vel + 0.5f * agentHeightVec, Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	DebugRenderer::Get().AddLine(pos + 0.25f * agentHeightVec, pos + desiredVel + 0.25f * agentHeightVec, Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	DebugRenderer::Get().AddCylinder(pos, m_radius, m_height, isActive() ? Vector4f(1.0f, 1.0f, 1.0f, 1.0f) : Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
}

void CrowdAgent::initCallbacks() {
	OnPositionVelocityUpdate = [](const Vector3f& pos, const Vector3f& vel) { };
	OnInactive = []() {};
	OnTarget = [](const Vector3f& pos) {};
	OnAddAgent = [](const Vector3f& pos) {};
}

bool CrowdAgent::hasRequestedTarget() const { 
	return m_requestedTargetType != CA_REQUESTEDTARGET_NONE;
}

bool CrowdAgent::hasArrived(const float scale) const {
	// Is the agent at or near the end of its path and within its own radius of the goal?
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	return agent && (!agent->ncorners || (agent->cornerFlags[agent->ncorners - 1] & DT_STRAIGHTPATH_END &&
		dtVdist2D(agent->npos, &agent->cornerVerts[(agent->ncorners - 1) * 3]) <=
		agent->params.radius * scale)) || m_forceArrived;
}

bool CrowdAgent::isInCrowd() const {
	return m_crowdManager && m_agentCrowdId != -1;
}

int CrowdAgent::addAgentToCrowd(bool force, const Vector3f& initialPosition, bool add){
	if (force || !isInCrowd()){
		m_agentCrowdId = m_crowdManager->addAgent(this, initialPosition, add);
		if (m_agentCrowdId == -1)
			return -1;

		resetParameter();
		m_previousAgentState = getAgentState();
		m_previousTargetState = getTargetState();
		m_previousPosition = getPosition();
	}
	return m_agentCrowdId;
}

void CrowdAgent::resetParameter() {
	m_maxAccel = std::max(0.f, m_maxAccel);
	m_maxSpeed = std::max(0.f, m_maxSpeed);
	m_radius = std::max(0.f, m_radius);
	m_height = std::max(0.f, m_height);
	m_queryFilterType = std::min(m_queryFilterType, (unsigned)DT_CROWD_MAX_QUERY_FILTER_TYPE - 1);
	m_obstacleAvoidanceType = std::min(m_obstacleAvoidanceType, (unsigned)DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS - 1);

	updateParameters();

	CrowdAgentRequestedTarget requestedTargetType = m_requestedTargetType;

	if (CA_REQUESTEDTARGET_NONE != m_requestedTargetType) {
		// Assign a dummy value such that the value check in the setter method passes
		m_requestedTargetType = CA_REQUESTEDTARGET_NONE;
		if (requestedTargetType == CA_REQUESTEDTARGET_POSITION) 
			setTargetPosition(m_targetPosition);
		else
			setTargetVelocity(m_targetVelocity);
	}else {
		m_requestedTargetType = CA_REQUESTEDTARGET_POSITION;
		resetTarget();
	}
}

void CrowdAgent::removeAgentFromCrowd() {
	if (isInCrowd()){
		m_crowdManager->removeAgent(this);
		m_agentCrowdId = -1;
	}
}

void CrowdAgent::updateParameters(unsigned int scope) {
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	if (agent){
		dtCrowdAgentParams params = agent->params;

		if (scope & SCOPE_NAVIGATION_QUALITY_PARAMS){
			switch (m_navQuality){
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
			switch (m_navPushiness){
				case NAVIGATIONPUSHINESS_LOW:
					params.collisionQueryRange = m_radius * 16.0f;
					break;

				case NAVIGATIONPUSHINESS_MEDIUM:
					params.collisionQueryRange = m_radius * 8.0f;
					break;

				case NAVIGATIONPUSHINESS_HIGH:
					params.collisionQueryRange = m_radius * 1.0f;
					break;

				case NAVIGATIONPUSHINESS_NONE:
					params.collisionQueryRange = m_radius * 1.0f;
					break;
			}
		}

		if (scope & SCOPE_SEPARATION_WEIGHT) {
			params.separationWeight = m_separationWeight;
		}

		if (scope & SCOPE_BASE_PARAMS){
			params.radius = m_radius;
			params.height = m_height;
			params.maxAcceleration = m_maxAccel;
			params.maxSpeed = m_maxSpeed;
			params.pathOptimizationRange = m_radius * 30.0f;
			params.queryFilterType = (unsigned char)m_queryFilterType;
			params.obstacleAvoidanceType = (unsigned char)m_obstacleAvoidanceType;
		}

		m_crowdManager->getCrowd()->updateAgentParameters(m_agentCrowdId, &params);
	}
}

const dtCrowdAgent* CrowdAgent::getDetourCrowdAgent() const {
	return isInCrowd() ? m_crowdManager->getDetourCrowdAgent(m_agentCrowdId) : 0;
}

CrowdAgentState CrowdAgent::getAgentState() const {
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	return agent ? (CrowdAgentState)agent->state : CrowdAgentState::DT_CROWDAGENT_STATE_INVALID;
}

CrowdAgentTargetState CrowdAgent::getTargetState() const {
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	return agent ? (CrowdAgentTargetState)agent->targetState : CA_TARGET_NONE;
}

Vector3f CrowdAgent::getPosition() const {
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	return agent ? Vector3f(agent->npos) : Vector3f::ZERO;
}

Vector3f CrowdAgent::getDesiredVelocity() const {
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	return agent ? Vector3f(agent->dvel) : Vector3f::ZERO;
}

Vector3f CrowdAgent::getActualVelocity() const {
	const dtCrowdAgent* agent = getDetourCrowdAgent();
	return agent ? Vector3f(agent->vel) : Vector3f::ZERO;
}

const Vector3f& CrowdAgent::getTargetPosition() const {
	return m_targetPosition;
}

const Vector3f& CrowdAgent::getTargetVelocity() const {
	return m_targetVelocity;
}

CrowdAgentRequestedTarget CrowdAgent::getRequestedTargetType() const {
	return m_requestedTargetType;
}

int CrowdAgent::getAgentCrowdId() const {
	return m_agentCrowdId;
}

float CrowdAgent::getMaxAccel() const {
	return m_maxAccel;
}

float CrowdAgent::getMaxSpeed() const {
	return m_maxSpeed;
}

float CrowdAgent::getRadius() const {
	return m_radius;
}

float CrowdAgent::getHeight() const {
	return m_height;
}

unsigned int CrowdAgent::getQueryFilterType() const {
	return m_queryFilterType;
}

unsigned int CrowdAgent::getObstacleAvoidanceType() const {
	return m_obstacleAvoidanceType;
}

NavigationQuality CrowdAgent::getNavigationQuality() const {
	return m_navQuality;
}

NavigationPushiness CrowdAgent::getNavigationPushiness() const {
	return m_navPushiness;
}

void CrowdAgent::setMaxAccel(float maxAccel, bool force){
	if (maxAccel != m_maxAccel && maxAccel >= 0.f ||force){
		m_maxAccel = maxAccel;
		
		updateParameters(SCOPE_BASE_PARAMS);
	}
}

void CrowdAgent::setMaxSpeed(float maxSpeed, bool force){
	if (maxSpeed != m_maxSpeed && maxSpeed >= 0.f || force){
		m_maxSpeed = maxSpeed;
		updateParameters(SCOPE_BASE_PARAMS);
	}
}

void CrowdAgent::setRadius(float radius, bool force){
	if (radius != m_radius && radius > 0.f || force){
		m_radius = radius;
		updateParameters(SCOPE_BASE_PARAMS | SCOPE_NAVIGATION_PUSHINESS_PARAMS);
	}
}

void CrowdAgent::setHeight(float height, bool force){
	if (height != m_height && height > 0.f || force){
		m_height = height;
		updateParameters(SCOPE_BASE_PARAMS);
	}
}

void CrowdAgent::setSeparationWeight(float separationWeight, bool force) {
	if (separationWeight != m_separationWeight && separationWeight > 0.f || force) {
		m_separationWeight = separationWeight;
		updateParameters(SCOPE_SEPARATION_WEIGHT);
	}
}

void CrowdAgent::setNavigationPushiness(NavigationPushiness val, bool force){
	if (val != m_navPushiness || force){
		m_navPushiness = val;
		updateParameters(SCOPE_NAVIGATION_PUSHINESS_PARAMS);
	}
}

void CrowdAgent::setTargetPosition(const Vector3f& position){
	if (position != m_targetPosition || CA_REQUESTEDTARGET_POSITION != m_requestedTargetType){
		m_targetPosition = position;
		m_requestedTargetType = CA_REQUESTEDTARGET_POSITION;

		if (isInCrowd()){		
			dtPolyRef nearestRef;
			NearestPos = m_crowdManager->findNearestPoint(position, m_queryFilterType, &nearestRef);
			m_crowdManager->getCrowd()->requestMoveTarget(m_agentCrowdId, nearestRef, NearestPos.getVec());
		}
	}
}

void CrowdAgent::setTargetVelocity(const Vector3f& velocity){
	if (velocity != m_targetVelocity || CA_REQUESTEDTARGET_VELOCITY != m_requestedTargetType){
		m_targetVelocity = velocity;
		m_requestedTargetType = CA_REQUESTEDTARGET_VELOCITY;

		if (isInCrowd())
			m_crowdManager->getCrowd()->requestMoveVelocity(m_agentCrowdId, velocity.getVec());
	}
}

void CrowdAgent::setVelocityCallback(const CrowdAgentVelocityCallback& callback) { 
	m_velocityCallback = callback; 
}

const CrowdAgentVelocityCallback& CrowdAgent::getVelocityCallback() const { 
	return m_velocityCallback; 
}

void CrowdAgent::setHeightCallback(const CrowdAgentHeightCallback& callback) { 
	m_heightCallback = callback; 
}

const CrowdAgentHeightCallback& CrowdAgent::getHeightCallback() const { 
	return m_heightCallback;
}

void CrowdAgent::setOnPositionVelocityUpdate(std::function<void(const Vector3f& pos, const Vector3f& vel)> fun) const {
	OnPositionVelocityUpdate = fun;
}

void CrowdAgent::setOnInactive(std::function<void()> fun) const {
	OnInactive = fun;
}

void CrowdAgent::setOnTarget(std::function<void(const Vector3f& pos)> fun) const {
	OnTarget = fun;
}

void CrowdAgent::setOnAddAgent(std::function<void(const Vector3f& pos)> fun) const {
	OnAddAgent = fun;
}

void CrowdAgent::resetAgent() const {
	m_active = false;
	m_forceArrived = true;
	m_requestedTargetType = CA_REQUESTEDTARGET_NONE;
	m_crowdManager->getCrowd()->resetMoveTarget(m_agentCrowdId);
}

void CrowdAgent::resetTarget() {
	if (CA_REQUESTEDTARGET_NONE != m_requestedTargetType){
		m_requestedTargetType = CA_REQUESTEDTARGET_NONE;
		if (isInCrowd())
			m_crowdManager->getCrowd()->resetMoveTarget(m_agentCrowdId);
	}
}

bool CrowdAgent::isActive() {
	return m_crowdManager->getCrowd()->isActive(m_agentCrowdId, 5.0f) || m_forceActive;
}

void CrowdAgent::setForceArrived(bool forceArrived) const {
	m_forceArrived = forceArrived;
}

void CrowdAgent::setForceActive(bool forceActive) const {
	m_forceActive = forceActive;
}

bool CrowdAgent::OnTileAdded(const std::array<int, 2>& tile){

	if (!m_crowdManager)
		return false;

	NavigationMesh* mesh = m_crowdManager->getNavigationMesh();

	const Vector3f pos = getPosition();
	const std::array<int, 2> agentTile = mesh->GetTileIndex(pos);
	const BoundingBox boundingBox = mesh->GetTileBoudningBox(agentTile);

	if (tile == agentTile && isInCrowd()){
		m_crowdManager->removeAgent(this);
		m_crowdManager->addAgent(this, pos, false);
		return true;
	}

	return false;
}

const Vector3f& CrowdAgent::GetNearestPos() {
	return NearestPos;
}

void CrowdAgent::SetNearestPos(const Vector3f& nearestPos) {
	NearestPos = nearestPos;
}