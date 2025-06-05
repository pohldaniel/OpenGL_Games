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

/// Parameter structure for obstacle avoidance params.
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

class CrowdManager {
	friend class CrowdAgent;

public:

	CrowdManager();
	virtual ~CrowdManager();
	void OnRenderDebug() const;
	void update(float delta);
	
	void updateAgentVelocity(CrowdAgent* agent, float timeStep, Vector3f& desiredVelocity, float& desiredSpeed) const;
	void updateAgentPosition(CrowdAgent* agent, float timeStep, Vector3f& position) const;

	bool setCrowdTarget(const Vector3f& position);
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
	CrowdAgent* addAgent(const Vector3f& pos);
	
	void removeAgent(const CrowdAgent* agent, bool force = true);
	void removeAgents();
	const dtCrowdAgent* getDetourCrowdAgent(int agent) const;
	dtCrowd* getCrowd() const;
	void setSeparationWeight(float separationWeight);

	void setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, CrowdAgent* agent)> fun);

	const CrowdObstacleAvoidanceParams& getObstacleAvoidanceParams(unsigned int obstacleAvoidanceType) const;
	void setObstacleAvoidanceParams(unsigned int obstacleAvoidanceType, const CrowdObstacleAvoidanceParams& params);

	void resetNavMesh(dtNavMesh* nav);
	void initNavquery(dtNavMesh* nav);

	void resetAgents();

private:

	int addAgent(CrowdAgent* agent, const Vector3f& pos, bool force = true);
	Vector3f OnCrowdFormationDefault(const Vector3f& pos, CrowdAgent* agent);
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

};