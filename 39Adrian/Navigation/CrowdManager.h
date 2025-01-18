#pragma once

#include <vector>

#ifdef DT_POLYREF64
typedef uint64_t dtPolyRef;
#else
typedef unsigned int dtPolyRef;
#endif

class dtCrowd;
class dtQueryFilter;
struct dtCrowdAgent;

class CrowdAgent;
class NavigationMesh;

/// Parameter structure for obstacle avoidance params (copied from DetourObstacleAvoidance.h in order to hide Detour header from Urho3D library users).
struct CrowdObstacleAvoidanceParams
{
	float velBias;
	float weightDesVel;
	float weightCurVel;
	float weightSide;
	float weightToi;
	float horizTime;
	unsigned char gridSize;         ///< grid
	unsigned char adaptiveDivs;     ///< adaptive
	unsigned char adaptiveRings;    ///< adaptive
	unsigned char adaptiveDepth;    ///< adaptive
};

class CrowdManager{
	friend class CrowdAgent;

public:

	/// Construct.
	CrowdManager();
	/// Destruct.
	virtual ~CrowdManager();
	/// Register object factory.

private:
	/// Handle the scene subsystem update event.
	//void HandleSceneSubsystemUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle navigation mesh changed event. It can be navmesh being rebuilt or being removed from its node.
	//void HandleNavMeshChanged(StringHash eventType, VariantMap& eventData);
	/// Handle component added in the scene to check for late addition of the navmesh.
	//void HandleComponentAdded(StringHash eventType, VariantMap& eventData);

	/// Internal Detour crowd object.
	dtCrowd* crowd_;
	/// NavigationMesh for which the crowd was created.
	NavigationMesh* navigationMesh_;
	/// The NavigationMesh component Id for pending crowd creation.
	unsigned navigationMeshId_;
	/// The maximum number of agents the crowd can manage.
	unsigned maxAgents_;
	/// The maximum radius of any agent that will be added to the crowd.
	float maxAgentRadius_;
	/// Number of query filter types configured in the crowd. Limit to DT_CROWD_MAX_QUERY_FILTER_TYPE.
	unsigned numQueryFilterTypes_;
	/// Number of configured area in each filter type. Limit to DT_MAX_AREAS.
	std::vector<unsigned> numAreas_;
	/// Number of obstacle avoidance types configured in the crowd. Limit to DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS.
	unsigned numObstacleAvoidanceTypes_;
};