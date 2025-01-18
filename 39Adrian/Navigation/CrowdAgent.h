#pragma once
#include <limits>
#include "../engine/Vector.h"
#include "CrowdManager.h"

enum CrowdAgentRequestedTarget
{
	CA_REQUESTEDTARGET_NONE = 0,
	CA_REQUESTEDTARGET_POSITION,
	CA_REQUESTEDTARGET_VELOCITY
};

enum CrowdAgentTargetState
{
	CA_TARGET_NONE = 0,
	CA_TARGET_FAILED,
	CA_TARGET_VALID,
	CA_TARGET_REQUESTING,
	CA_TARGET_WAITINGFORQUEUE,
	CA_TARGET_WAITINGFORPATH,
	CA_TARGET_VELOCITY
};



enum NavigationQuality
{
	NAVIGATIONQUALITY_LOW = 0,
	NAVIGATIONQUALITY_MEDIUM = 1,
	NAVIGATIONQUALITY_HIGH = 2
};

enum NavigationPushiness
{
	NAVIGATIONPUSHINESS_LOW = 0,
	NAVIGATIONPUSHINESS_MEDIUM,
	NAVIGATIONPUSHINESS_HIGH,
	NAVIGATIONPUSHINESS_NONE
};
enum CrowdAgentState : int;

class CrowdAgent {

	friend class CrowdManager;
	friend void CrowdAgentUpdateCallback(dtCrowdAgent* ag, float dt);

public:

	/// Construct.
	CrowdAgent();
	/// Destruct.
	virtual ~CrowdAgent();

	virtual void OnCrowdUpdate(dtCrowdAgent* ag, float dt);

private:
	/// Update Detour crowd agent parameter.
	void UpdateParameters(unsigned scope = std::numeric_limits<unsigned int>::max());
	/// Add agent into crowd.
	int AddAgentToCrowd(bool force = false);
	/// Remove agent from crowd.
	void RemoveAgentFromCrowd();
	/// Crowd manager.
	CrowdManager* crowdManager_;
	/// Crowd manager reference to this agent.
	int agentCrowdId_;
	/// Requested target position.
	Vector3f targetPosition_;
	/// Requested target velocity.
	Vector3f targetVelocity_;
	/// Requested target type.
	CrowdAgentRequestedTarget requestedTargetType_;
	/// Flag indicating the node's position should be updated by Detour crowd manager.
	bool updateNodePosition_;
	/// Agent's max acceleration.
	float maxAccel_;
	/// Agent's max Velocity.
	float maxSpeed_;
	/// Agent's radius, if 0 the navigation mesh's setting will be used.
	float radius_;
	/// Agent's height, if 0 the navigation mesh's setting will be used.
	float height_;
	/// Agent's query filter type, it is an index to the query filter buffer configured in Detour crowd manager.
	unsigned queryFilterType_;
	/// Agent's obstacle avoidance type, it is an index to the obstacle avoidance array configured in Detour crowd manager. It is ignored when agent's navigation quality is not set to "NAVIGATIONQUALITY_HIGH".
	unsigned obstacleAvoidanceType_;
	/// Agent's navigation quality. The higher the setting, the higher the CPU usage during crowd simulation.
	NavigationQuality navQuality_;
	/// Agent's navigation pushiness. The higher the setting, the stronger the agent pushes its colliding neighbours around.
	NavigationPushiness navPushiness_;
	/// Agent's previous position used to check for position changes.
	Vector3f previousPosition_;
	/// Agent's previous target state used to check for state changes.
	CrowdAgentTargetState previousTargetState_;
	/// Agent's previous agent state used to check for state changes.
	CrowdAgentState previousAgentState_;
	/// Internal flag to ignore transform changes because it came from us, used in OnCrowdAgentReposition().
	bool ignoreTransformChanges_;
};