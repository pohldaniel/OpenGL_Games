#include <iostream>
#include <DetourCrowd.h>
#include <DetourNavMesh.h>

#include "../engine/scene/SceneNode.h"
#include "../engine/DebugRenderer.h"
#include "NavigationMesh.h"
#include "CrowdManager.h"
#include "CrowdAgent.h"

extern const char* NAVIGATION_CATEGORY;

static const unsigned DEFAULT_MAX_AGENTS = 512;
static const float DEFAULT_MAX_AGENT_RADIUS = 0.f;

void CrowdAgentUpdateCallback(unsigned int state, dtCrowdAgent* ag, float* posVel, float dt){
	auto crowdAgent = static_cast<CrowdAgent*>(ag->params.userData);
	if (state == 0)
		crowdAgent->OnCrowdVelocityUpdate(ag, posVel, dt);
	else if (state == 1)
		crowdAgent->OnCrowdPositionUpdate(ag, posVel, dt);
	else if (state == 2)
		crowdAgent->OnCrowdVelocityUpdate(ag, posVel, dt);
	else if (state == 4)
		crowdAgent->OnCrowdPositionUpdate(ag, posVel, dt);
}

CrowdManager::CrowdManager() :
	m_crowd(nullptr),
	m_navigationMeshId(0u),
	m_maxAgents(DEFAULT_MAX_AGENTS),
	m_maxAgentRadius(DEFAULT_MAX_AGENT_RADIUS),
	m_numObstacleAvoidanceTypes(0u)
{
	
}

CrowdManager::~CrowdManager(){
	dtFreeCrowd(m_crowd);
	m_crowd = nullptr;
}

void CrowdManager::OnRenderDebug() const {
	if (m_crowd) {
		for (int i = 0; i < m_crowd->getAgentCount(); i++) {
			const dtCrowdAgent* ag = m_crowd->getAgent(i);
			if (!ag->active)
				continue;

			// Draw CrowdAgent shape (from its radius & height)
			CrowdAgent* crowdAgent = static_cast<CrowdAgent*>(ag->params.userData);
			crowdAgent->OnRenderDebug();

			// Draw move target if any
			if (crowdAgent->getTargetState() == CA_TARGET_NONE || crowdAgent->getTargetState() == CA_TARGET_VELOCITY)
				continue;

			Vector4f color(0.6f, 0.2f, 0.2f, 1.0f);

			// Draw line to target
			Vector3f pos1(ag->npos[0], ag->npos[1], ag->npos[2]);
			Vector3f pos2;
			for (int i = 0; i < ag->ncorners; ++i) {
				pos2[0] = ag->cornerVerts[i * 3];
				pos2[1] = ag->cornerVerts[i * 3 + 1];
				pos2[2] = ag->cornerVerts[i * 3 + 2];
				DebugRenderer::Get().AddLine(pos1, pos2, color);
				pos1 = pos2;
			}
			pos2[0] = ag->targetPos[0];
			pos2[1] = ag->targetPos[1];
			pos2[2] = ag->targetPos[2];
			DebugRenderer::Get().AddLine(pos1, pos2, color);

			// Draw target circle
			//debug->AddSphere(Sphere(pos2, 0.5f), color, depthTest);
		}
	}
}

void CrowdManager::updateAgentVelocity(CrowdAgent* agent, float timeStep, Vector3f& desiredVelocity, float& desiredSpeed) const {
	const CrowdAgentVelocityCallback& agentCallback = agent->getVelocityCallback();
	const CrowdAgentVelocityCallback& callback = agentCallback ? agentCallback : m_velocityCallback;
	if (callback)
		callback(agent, timeStep, desiredVelocity, desiredSpeed);
}

void CrowdManager::updateAgentPosition(CrowdAgent* agent, float timeStep, Vector3f& position) const {
	const CrowdAgentHeightCallback& agentCallback = agent->getHeightCallback();
	const CrowdAgentHeightCallback& callback = agentCallback ? agentCallback : m_heightCallback;
	if (callback)
		position[1] = callback(agent, timeStep, position);
}

const dtCrowdAgent* CrowdManager::getDetourCrowdAgent(int agent) const{
	return m_crowd ? m_crowd->getAgent(agent) : nullptr;
}

void CrowdManager::setNavigationMesh(NavigationMesh* navigationMesh){
	if (navigationMesh != m_navigationMesh) {
		m_navigationMesh = navigationMesh;
		createCrowd();
	}
}

int CrowdManager::addAgent(CrowdAgent* agent, const Vector3f& pos, bool add){
	if (!m_crowd || !m_navigationMesh || !agent)
		return -1;
	dtCrowdAgentParams params;
	params.userData = agent;
	if (agent->m_radius == 0.f)
		agent->m_radius = m_navigationMesh->GetAgentRadius();
	if (agent->m_height == 0.f)
		agent->m_height = m_navigationMesh->GetAgentHeight();
	// dtCrowd::addAgent() requires the query filter type to find the nearest position on navmesh as the initial agent's position
	params.queryFilterType = (unsigned char)agent->getQueryFilterType();

	int id = m_crowd->addAgent(pos.getVec(), &params, nullptr, 1.5f);
	if (id > -1) {
		agent->m_agentCrowdId = id;
		agent->m_crowdManager = this;
		if(add)
			m_agents.push_back(agent);
	}
	return id;
}

bool CrowdManager::createCrowd(){
	if (!m_navigationMesh || !m_navigationMesh->InitializeQuery())
		return false;

	// Preserve the existing crowd configuration before recreating it
	//VariantVector queryFilterTypeConfiguration, obstacleAvoidanceTypeConfiguration;
	bool recreate = m_crowd != nullptr;
	if (recreate){
		//queryFilterTypeConfiguration = GetQueryFilterTypesAttr();
		//obstacleAvoidanceTypeConfiguration = GetObstacleAvoidanceTypesAttr();
		//dtFreeCrowd(m_crowd);
	}
	m_crowd = dtAllocCrowd();

	// Initialize the crowd
	if (m_maxAgentRadius == 0.f)
		m_maxAgentRadius = m_navigationMesh->GetAgentRadius();
	if (!m_crowd->init(m_maxAgents, m_maxAgentRadius, m_navigationMesh->navMesh_, CrowdAgentUpdateCallback)){
		std::cout << "Could not initialize DetourCrowd" << std::endl;
		return false;
	}

	if (recreate){
		// Reconfigure the newly initialized crowd
		//SetQueryFilterTypesAttr(queryFilterTypeConfiguration);
		//SetObstacleAvoidanceTypesAttr(obstacleAvoidanceTypeConfiguration);
		
		// Re-add the existing crowd agents
		/*size_t size = m_agents.size();
		for (unsigned i = 0; i < size; ++i){
			if (m_agents[i]->addAgentToCrowd(true, m_agents[i]->getPosition()) == -1){
				std::cout << "CrowdManager: " << m_agents.size() - i << " crowd agents orphaned" << std::endl;
				break;
			}
		}*/
	}

	return true;
}

bool CrowdManager::reCreateCrowd() {
	size_t size = m_agents.size();
	for (unsigned i = 0; i < size; ++i) {
		Vector3f pos = m_agents[i]->getPosition();

		removeAgent(m_agents[i]);

		
		addAgent(m_agents[i], pos, false);
		m_agents[i]->resetParameter();
		/*m_agents[i]->setHeight(2.0f, true);
		m_agents[i]->setMaxSpeed(6.0f, true);
		m_agents[i]->setMaxAccel(10.0f, true);
		m_agents[i]->setRadius(0.5f, true);
		m_agents[i]->setNavigationPushiness(NAVIGATIONPUSHINESS_MEDIUM, true);
		m_agents[i]->setSeparationWeight(3.0f, true);*/
	}
	return true;
}

void CrowdManager::removeAgent(CrowdAgent* agent)
{
	if (!m_crowd || !agent)
		return;
	dtCrowdAgent* agt = m_crowd->getEditableAgent(agent->m_agentCrowdId);
	if (agt)
		agt->params.userData = 0;
	m_crowd->removeAgent(agent->m_agentCrowdId);
}

std::vector<CrowdAgent*> CrowdManager::getAgents() {
	return m_agents;
}

void CrowdManager::setCrowdTarget(const Vector3f& position){
	if (!m_crowd)
		return;

	for (unsigned int i = 0; i < m_agents.size(); ++i){
		CrowdAgent* agent = m_agents[i];			
		agent->setTargetPosition(OnCrowdFormation(position, i ? agent : nullptr));
		agent->OnTarget(CrowdAgent::GetNearestPos());
	}
}

Vector3f CrowdManager::findNearestPoint(const Vector3f& point, int queryFilterType, dtPolyRef* nearestRef){
	if (nearestRef)
		*nearestRef = 0;
	return m_crowd && m_navigationMesh ?
		m_navigationMesh->FindNearestPoint(point, Vector3f(m_crowd->getQueryExtents()), m_crowd->getFilter(queryFilterType), nearestRef) : point;
}

void CrowdManager::update(float delta){
	m_crowd->update(delta, nullptr);
}

Vector3f CrowdManager::getRandomPointInCircle(const Vector3f& center, float radius, int queryFilterType, dtPolyRef* randomRef){
	if (randomRef)
		*randomRef = 0;

	return m_crowd && m_navigationMesh ?
		m_navigationMesh->GetRandomPointInCircle(center, radius, Vector3f(m_crowd->getQueryHalfExtents()), m_crowd->getFilter(queryFilterType), randomRef) : center;
}

void CrowdManager::setSeparationWeight(float separationWeight) {
	for (unsigned int i = 0; i < m_agents.size(); ++i) {
		CrowdAgent* agent = m_agents[i];
		const dtCrowdAgent* ag = m_agents[i]->getDetourCrowdAgent();

		if (!ag->active) continue;

		dtCrowdAgentParams params;
		memcpy(&params, &ag->params, sizeof(dtCrowdAgentParams));		
		params.separationWeight = separationWeight;
		m_crowd->updateAgentParameters(agent->getAgentCrowdId(), &params);
	}
}

void CrowdManager::setVelocityCallback(const CrowdAgentVelocityCallback& callback) { 
	m_velocityCallback = callback; 
}

void CrowdManager::setHeightCallback(const CrowdAgentHeightCallback& callback) { 
	m_heightCallback = callback; 
}

unsigned int CrowdManager::getMaxAgents() const { 
	return m_maxAgents; 
}

float CrowdManager::getMaxAgentRadius() const { 
	return m_maxAgentRadius; 
}

NavigationMesh* CrowdManager::getNavigationMesh() const { 
	return m_navigationMesh; 
}

dtCrowd* CrowdManager::getCrowd() const { 
	return m_crowd; 
}

void CrowdManager::setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, CrowdAgent* agent)> fun) {
	OnCrowdFormation = fun;
}

/*const dtQueryFilter* CrowdManager::GetDetourQueryFilter(unsigned queryFilterType) const {
	return crowd_ ? crowd_->getFilter(queryFilterType) : nullptr;
}

const CrowdObstacleAvoidanceParams& CrowdManager::GetObstacleAvoidanceParams(unsigned obstacleAvoidanceType) const {
	static const CrowdObstacleAvoidanceParams EMPTY_PARAMS = CrowdObstacleAvoidanceParams();
	const dtObstacleAvoidanceParams* params = crowd_ ? crowd_->getObstacleAvoidanceParams(obstacleAvoidanceType) : 0;
	return params ? *reinterpret_cast<const CrowdObstacleAvoidanceParams*>(params) : EMPTY_PARAMS;
}

void CrowdManager::SetObstacleAvoidanceParams(unsigned obstacleAvoidanceType, const CrowdObstacleAvoidanceParams& params) {
	if (crowd_ && obstacleAvoidanceType < DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS) {
		crowd_->setObstacleAvoidanceParams(obstacleAvoidanceType, reinterpret_cast<const dtObstacleAvoidanceParams*>(&params));
		if (numObstacleAvoidanceTypes_ < obstacleAvoidanceType + 1)
			numObstacleAvoidanceTypes_ = obstacleAvoidanceType + 1;
	}
}

void CrowdManager::SetIncludeFlags(unsigned queryFilterType, unsigned short flags) {
	dtQueryFilter* filter = const_cast<dtQueryFilter*>(GetDetourQueryFilter(queryFilterType));
	if (filter) {
		filter->setIncludeFlags(flags);
		if (numQueryFilterTypes_ < queryFilterType + 1)
			numQueryFilterTypes_ = queryFilterType + 1;
	}
}

void CrowdManager::SetExcludeFlags(unsigned queryFilterType, unsigned short flags) {
	dtQueryFilter* filter = const_cast<dtQueryFilter*>(GetDetourQueryFilter(queryFilterType));
	if (filter) {
		filter->setExcludeFlags(flags);
		if (numQueryFilterTypes_ < queryFilterType + 1)
			numQueryFilterTypes_ = queryFilterType + 1;
	}
}*/

const CrowdObstacleAvoidanceParams& CrowdManager::getObstacleAvoidanceParams(unsigned int obstacleAvoidanceType) const {
	static const CrowdObstacleAvoidanceParams EMPTY_PARAMS = CrowdObstacleAvoidanceParams();
	const dtObstacleAvoidanceParams* params = m_crowd ? m_crowd->getObstacleAvoidanceParams(obstacleAvoidanceType) : 0;
	return params ? *reinterpret_cast<const CrowdObstacleAvoidanceParams*>(params) : EMPTY_PARAMS;
}

void CrowdManager::setObstacleAvoidanceParams(unsigned int obstacleAvoidanceType, const CrowdObstacleAvoidanceParams& params) {
	if (m_crowd && obstacleAvoidanceType < DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS) {
		m_crowd->setObstacleAvoidanceParams(obstacleAvoidanceType, reinterpret_cast<const dtObstacleAvoidanceParams*>(&params));
		if (m_numObstacleAvoidanceTypes < obstacleAvoidanceType + 1)
			m_numObstacleAvoidanceTypes = obstacleAvoidanceType + 1;
	}
}

void CrowdManager::resetNavMesh(dtNavMesh* nav) {
	m_crowd->resetNavMesh(nav);
}

void CrowdManager::initNavquery(dtNavMesh* nav) {
	m_crowd->initNavquery(nav);
}