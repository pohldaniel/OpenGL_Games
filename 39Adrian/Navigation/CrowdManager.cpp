#include <iostream>
#include <DetourCrowd.h>

#include "../engine/scene/SceneNode.h"
#include "../engine/DebugRenderer.h"
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

void CrowdAgentUpdateCallback(bool positionUpdate, dtCrowdAgent* ag, float* pos, float dt)
{
	auto crowdAgent = static_cast<CrowdAgent*>(ag->params.userData);
	if (positionUpdate)
		crowdAgent->OnCrowdPositionUpdate(ag, pos, dt);
	else
		crowdAgent->OnCrowdVelocityUpdate(ag, pos, dt);
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

void CrowdManager::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	if (debug && crowd_)
	{
		
		// Current position-to-target line
		for (int i = 0; i < crowd_->getAgentCount(); i++)
		{
			const dtCrowdAgent* ag = crowd_->getAgent(i);
			if (!ag->active)
				continue;

			// Draw CrowdAgent shape (from its radius & height)
			CrowdAgent* crowdAgent = static_cast<CrowdAgent*>(ag->params.userData);
			crowdAgent->DrawDebugGeometry(debug, depthTest);

			// Draw move target if any
			if (crowdAgent->GetTargetState() == CA_TARGET_NONE || crowdAgent->GetTargetState() == CA_TARGET_VELOCITY)
				continue;

			Vector4f color(0.6f, 0.2f, 0.2f, 1.0f);

			// Draw line to target
			Vector3f pos1(ag->npos[0], ag->npos[1], ag->npos[2]);
			Vector3f pos2;
			for (int i = 0; i < ag->ncorners; ++i)
			{
				pos2[0] = ag->cornerVerts[i * 3];
				pos2[1] = ag->cornerVerts[i * 3 + 1];
				pos2[2] = ag->cornerVerts[i * 3 + 2];
				debug->AddLine(pos1, pos2, color);
				pos1 = pos2;
			}
			pos2[0] = ag->targetPos[0];
			pos2[1] = ag->targetPos[1];
			pos2[2] = ag->targetPos[2];
			debug->AddLine(pos1, pos2, color);

			// Draw target circle
			//debug->AddSphere(Sphere(pos2, 0.5f), color, depthTest);
		}
	}
}

void CrowdManager::UpdateAgentVelocity(CrowdAgent* agent, float timeStep, Vector3f& desiredVelocity, float& desiredSpeed) const
{
	const CrowdAgentVelocityCallback& agentCallback = agent->GetVelocityCallback();
	const CrowdAgentVelocityCallback& callback = agentCallback ? agentCallback : velocityCallback_;
	if (callback)
		callback(agent, timeStep, desiredVelocity, desiredSpeed);
}

void CrowdManager::UpdateAgentPosition(CrowdAgent* agent, float timeStep, Vector3f& position) const
{
	const CrowdAgentHeightCallback& agentCallback = agent->GetHeightCallback();
	const CrowdAgentHeightCallback& callback = agentCallback ? agentCallback : heightCallback_;
	if (callback)
		position[1] = callback(agent, timeStep, position);
}

const dtCrowdAgent* CrowdManager::GetDetourCrowdAgent(int agent) const{
	return crowd_ ? crowd_->getAgent(agent) : nullptr;
}

const dtQueryFilter* CrowdManager::GetDetourQueryFilter(unsigned queryFilterType) const{
	return crowd_ ? crowd_->getFilter(queryFilterType) : nullptr;
}

const CrowdObstacleAvoidanceParams& CrowdManager::GetObstacleAvoidanceParams(unsigned obstacleAvoidanceType) const{
	static const CrowdObstacleAvoidanceParams EMPTY_PARAMS = CrowdObstacleAvoidanceParams();
	const dtObstacleAvoidanceParams* params = crowd_ ? crowd_->getObstacleAvoidanceParams(obstacleAvoidanceType) : 0;
	return params ? *reinterpret_cast<const CrowdObstacleAvoidanceParams*>(params) : EMPTY_PARAMS;
}

void CrowdManager::SetObstacleAvoidanceParams(unsigned obstacleAvoidanceType, const CrowdObstacleAvoidanceParams& params){
	if (crowd_ && obstacleAvoidanceType < DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS){
		crowd_->setObstacleAvoidanceParams(obstacleAvoidanceType, reinterpret_cast<const dtObstacleAvoidanceParams*>(&params));
		if (numObstacleAvoidanceTypes_ < obstacleAvoidanceType + 1)
			numObstacleAvoidanceTypes_ = obstacleAvoidanceType + 1;
	}
}

void CrowdManager::SetIncludeFlags(unsigned queryFilterType, unsigned short flags){
	dtQueryFilter* filter = const_cast<dtQueryFilter*>(GetDetourQueryFilter(queryFilterType));
	if (filter){
		filter->setIncludeFlags(flags);
		if (numQueryFilterTypes_ < queryFilterType + 1)
			numQueryFilterTypes_ = queryFilterType + 1;
	}
}

void CrowdManager::SetExcludeFlags(unsigned queryFilterType, unsigned short flags){
	dtQueryFilter* filter = const_cast<dtQueryFilter*>(GetDetourQueryFilter(queryFilterType));
	if (filter){
		filter->setExcludeFlags(flags);
		if (numQueryFilterTypes_ < queryFilterType + 1)
			numQueryFilterTypes_ = queryFilterType + 1;
	}
}

void CrowdManager::SetNavigationMesh(NavigationMesh* navMesh)
{
	//UnsubscribeFromEvent(E_COMPONENTADDED);
	//UnsubscribeFromEvent(E_NAVIGATION_MESH_REBUILT);
	//UnsubscribeFromEvent(E_COMPONENTREMOVED);

	if (navMesh != navigationMesh_)     // It is possible to reset navmesh pointer back to 0
	{
		//Scene* scene = GetScene();

		navigationMesh_ = navMesh;
		//navigationMeshId_ = navMesh ? navMesh->GetID() : 0;

		if (navMesh)
		{
			//SubscribeToEvent(navMesh, E_NAVIGATION_MESH_REBUILT, URHO3D_HANDLER(CrowdManager, HandleNavMeshChanged));
			//SubscribeToEvent(scene, E_COMPONENTREMOVED, URHO3D_HANDLER(CrowdManager, HandleNavMeshChanged));
		}

		CreateCrowd();
		//MarkNetworkUpdate();
	}
}

int CrowdManager::AddAgent(CrowdAgent* agent, const Vector3f& pos)
{
	if (!crowd_ || !navigationMesh_ || !agent)
		return -1;
	dtCrowdAgentParams params;
	params.userData = agent;
	if (agent->radius_ == 0.f)
		agent->radius_ = navigationMesh_->GetAgentRadius();
	if (agent->height_ == 0.f)
		agent->height_ = navigationMesh_->GetAgentHeight();
	// dtCrowd::addAgent() requires the query filter type to find the nearest position on navmesh as the initial agent's position
	params.queryFilterType = (unsigned char)agent->GetQueryFilterType();
	return crowd_->addAgent(pos.getVec(), &params);
}

bool CrowdManager::CreateCrowd(){

	if (!navigationMesh_ || !navigationMesh_->InitializeQuery())
		return false;

	// Preserve the existing crowd configuration before recreating it
	//VariantVector queryFilterTypeConfiguration, obstacleAvoidanceTypeConfiguration;
	bool recreate = crowd_ != 0;
	if (recreate)
	{
		//queryFilterTypeConfiguration = GetQueryFilterTypesAttr();
		//obstacleAvoidanceTypeConfiguration = GetObstacleAvoidanceTypesAttr();
		dtFreeCrowd(crowd_);
	}
	crowd_ = dtAllocCrowd();

	// Initialize the crowd
	if (maxAgentRadius_ == 0.f)
		maxAgentRadius_ = navigationMesh_->GetAgentRadius();
	if (!crowd_->init(maxAgents_, maxAgentRadius_, navigationMesh_->navMesh_, CrowdAgentUpdateCallback))
	{
		//URHO3D_LOGERROR("Could not initialize DetourCrowd");

		std::cout << "Could not initialize DetourCrowd" << std::endl;

		return false;
	}
	if (recreate){
		// Reconfigure the newly initialized crowd
		//SetQueryFilterTypesAttr(queryFilterTypeConfiguration);
		//SetObstacleAvoidanceTypesAttr(obstacleAvoidanceTypeConfiguration);
		
		// Re-add the existing crowd agents
		std::vector<CrowdAgent*> agents = GetAgents();
		for (unsigned i = 0; i < agents.size(); ++i){
			// Keep adding until the crowd cannot take it anymore
			if (agents[i]->AddAgentToCrowd(true) == -1){
				//URHO3D_LOGWARNINGF("CrowdManager: %d crowd agents orphaned", agents.Size() - i);

				std::cout << "CrowdManager: " << agents.size() - i << " crowd agents orphaned" << std::endl;

				break;
			}
		}
	}

	return true;
}

std::vector<CrowdAgent*> CrowdManager::GetAgents(SceneNodeLC* node, bool inCrowdFilter) {
	//if (!node)
		//node = GetScene();
	//std::vector<CrowdAgent*> agents;
	//node->GetComponents<CrowdAgent>(agents, true);
	if (inCrowdFilter){
		std::vector<CrowdAgent*>::iterator i = m_agents.begin();
		while (i != m_agents.end()){
			if ((*i)->IsInCrowd())
				++i;
			else
				i = m_agents.erase(i);
		}
	}
	return m_agents;
}

void CrowdManager::SetCrowdTarget(const Vector3f& position)
{
	if (!crowd_)
		return;
	//std::cout << m_agents.size() << std::endl;
	for (unsigned int i = 0; i < m_agents.size(); ++i)
	{
		CrowdAgent* agent = m_agents[i];			
		agent->SetTargetPosition(agent->OnCrowdFormation(position, i, agent));
	}
	//std::vector<CrowdAgent*> agents = GetAgents(node, false);     // Get all crowd agent components
	//Vector3f moveTarget(position);
	/*for (unsigned i = 0; i < agents.size(); ++i)
	{
		// Give application a chance to determine the desired crowd formation when they reach the target position
		CrowdAgent* agent = agents[i];

		/*using namespace CrowdAgentFormation;

		VariantMap& map = GetEventDataMap();
		map[P_NODE] = agent->GetNode();
		map[P_CROWD_AGENT] = agent;
		map[P_INDEX] = i;
		map[P_SIZE] = agents.Size();
		map[P_POSITION] = moveTarget;   // Expect the event handler will modify this position accordingly

		SendEvent(E_CROWD_AGENT_FORMATION, map);

		moveTarget = map[P_POSITION].GetVector3();
		agent->SetTargetPosition(position);
	}
	agent->SetTargetPosition(position);*/
}

Vector3f CrowdManager::FindNearestPoint(const Vector3f& point, int queryFilterType, dtPolyRef* nearestRef){
	if (nearestRef)
		*nearestRef = 0;
	return crowd_ && navigationMesh_ ?
		navigationMesh_->FindNearestPoint(point, Vector3f(crowd_->getQueryExtents()), crowd_->getFilter(queryFilterType), nearestRef) : point;
}

void CrowdManager::Update(float delta){
	crowd_->update(delta, nullptr);
	const dtCrowdAgent* agent = crowd_->getAgent(0);
}

Vector3f CrowdManager::GetRandomPointInCircle(const Vector3f& center, float radius, int queryFilterType, dtPolyRef* randomRef)
{
	if (randomRef)
		*randomRef = 0;
	return crowd_ && navigationMesh_ ?
		navigationMesh_->GetRandomPointInCircle(center, radius, Vector3f(crowd_->getQueryExtents()),crowd_->getFilter(queryFilterType), randomRef) : center;
}