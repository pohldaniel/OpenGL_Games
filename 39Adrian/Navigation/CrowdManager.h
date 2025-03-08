#pragma once

#include <vector>
#include <functional>

typedef unsigned int dtPolyRef;

class dtCrowd;
class dtQueryFilter;
class dtNavMesh;
struct dtCrowdAgent;

class CrowdAgent;
class NavigationMesh;
class SceneNodeLC;

/// Parameter structure for obstacle avoidance params (copied from DetourObstacleAvoidance.h in order to hide Detour header from Urho3D library users).
struct CrowdObstacleAvoidanceParams{
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

/// Callback used to adjust crowd agent velocity.
using CrowdAgentVelocityCallback = std::function<void(CrowdAgent* agent, float timeStep, Vector3f& desiredVelocity, float& desiredSpeed)>;
/// Callback used to evaluate crowd agent Y position.
using CrowdAgentHeightCallback = std::function<float(CrowdAgent* agent, float timeStep, const Vector3f& position)>;

class CrowdManager{
	friend class CrowdAgent;

public:

	CrowdManager();
	virtual ~CrowdManager();
	void OnRenderDebug() const;
	void update(float delta);
	
	void updateAgentVelocity(CrowdAgent* agent, float timeStep, Vector3f& desiredVelocity, float& desiredSpeed) const;
	void updateAgentPosition(CrowdAgent* agent, float timeStep, Vector3f& position) const;

	void setCrowdTarget(const Vector3f& position);
	void setNavigationMesh(NavigationMesh* navigationMesh);
	
	std::vector<CrowdAgent*> getAgents();
	Vector3f findNearestPoint(const Vector3f& point, int queryFilterType, dtPolyRef* nearestRef = 0);
	Vector3f getRandomPointInCircle(const Vector3f& center, float radius, int queryFilterType, dtPolyRef* randomRef = 0);
	
	void setVelocityCallback(const CrowdAgentVelocityCallback& callback);
	void setHeightCallback(const CrowdAgentHeightCallback& callback);

	unsigned int getMaxAgents() const;
	float getMaxAgentRadius() const;
	NavigationMesh* getNavigationMesh() const;

	bool createCrowd();
	bool reCreateCrowd();
	int addAgent(CrowdAgent* agent, const Vector3f& pos, bool add = true);
	void removeAgent(CrowdAgent* agent);

	const dtCrowdAgent* getDetourCrowdAgent(int agent) const;
	dtCrowd* getCrowd() const;
	void setSeparationWeight(float separationWeight);

	void setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, CrowdAgent* agent)> fun);

	const CrowdObstacleAvoidanceParams& getObstacleAvoidanceParams(unsigned int obstacleAvoidanceType) const;
	void setObstacleAvoidanceParams(unsigned int obstacleAvoidanceType, const CrowdObstacleAvoidanceParams& params);

	void resetNavMesh(dtNavMesh* nav);
	void initNavquery(dtNavMesh* nav);

	void resetAgents();

	/// Get the number of configured obstacle avoidance types.
	//unsigned GetNumObstacleAvoidanceTypes() const { return numObstacleAvoidanceTypes_; }
	/// Get the number of configured query filter types.
	//unsigned GetNumQueryFilterTypes() const { return numQueryFilterTypes_; }
	/// Get the detour query filter.
	//const dtQueryFilter* GetDetourQueryFilter(unsigned queryFilterType) const;
	/// Get the params for the specified obstacle avoidance type.
	//const CrowdObstacleAvoidanceParams& GetObstacleAvoidanceParams(unsigned obstacleAvoidanceType) const;
	/// Set the params for the specified obstacle avoidance type.
	//void SetObstacleAvoidanceParams(unsigned obstacleAvoidanceType, const CrowdObstacleAvoidanceParams& params);
	/// Set the include flags for the specified query filter type.
	//void SetIncludeFlags(unsigned queryFilterType, unsigned short flags);
	/// Set the exclude flags for the specified query filter type.
	//void SetExcludeFlags(unsigned queryFilterType, unsigned short flags);
	/// Get the include flags for the specified query filter type.
	//unsigned short GetIncludeFlags(unsigned queryFilterType) const;
	/// Get the exclude flags for the specified query filter type.
	//unsigned short GetExcludeFlags(unsigned queryFilterType) const;
	
private:
	
	dtCrowd* m_crowd;
	CrowdAgentVelocityCallback m_velocityCallback;
	CrowdAgentHeightCallback m_heightCallback;
	NavigationMesh* m_navigationMesh;
	unsigned int m_navigationMeshId;
	unsigned int m_maxAgents;
	float m_maxAgentRadius;
	unsigned int m_numObstacleAvoidanceTypes;
	std::vector<CrowdAgent*> m_agents;

	std::function<Vector3f(const Vector3f& pos, CrowdAgent* agent)> OnCrowdFormation;

	/// Number of query filter types configured in the crowd. Limit to DT_CROWD_MAX_QUERY_FILTER_TYPE.
	//unsigned int numQueryFilterTypes_;
	/// Number of configured area in each filter type. Limit to DT_MAX_AREAS.
	//std::vector<unsigned int> numAreas_;
	/// Number of obstacle avoidance types configured in the crowd. Limit to DT_CROWD_MAX_OBSTAVOIDANCE_PARAMS.
	//unsigned int numObstacleAvoidanceTypes_;
};