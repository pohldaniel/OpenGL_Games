#pragma once
#include <limits>
#include <functional>

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
class DebugRenderer;

class CrowdAgent {

	friend class CrowdManager;
	friend void CrowdAgentUpdateCallback(dtCrowdAgent* ag, float dt);

public:

	/// Construct.
	CrowdAgent();
	/// Destruct.
	virtual ~CrowdAgent();

	/// Handle enabled/disabled state change.
	//virtual void OnSetEnabled();
	/// Draw debug geometry.
	void DrawDebugGeometry(bool depthTest);
	/// Draw debug feelers.
	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

	/// Set velocity callback.
	void SetVelocityCallback(const CrowdAgentVelocityCallback& callback) { velocityCallback_ = callback; }
	/// Return current velocity callback.
	const CrowdAgentVelocityCallback& GetVelocityCallback() const { return velocityCallback_; }
	/// Set height callback.
	void SetHeightCallback(const CrowdAgentHeightCallback& callback) { heightCallback_ = callback; }
	/// Return current height callback.
	const CrowdAgentHeightCallback& GetHeightCallback() const { return heightCallback_; }


	/// Submit a new target position request for this agent.
	void SetTargetPosition(const Vector3f& position);
	/// Submit a new target velocity request for this agent.
	void SetTargetVelocity(const Vector3f& velocity);
	/// Reset any target request for the specified agent. Note that the agent will continue to move into the current direction; set a zero target velocity to actually stop.
	void ResetTarget();
	/// Update the node position. When set to false, the node position should be updated by other means (e.g. using Physics) in response to the E_CROWD_AGENT_REPOSITION event.
	void SetUpdateNodePosition(bool unodepos);
	/// Set the agent's max acceleration.
	void SetMaxAccel(float maxAccel);
	/// Set the agent's max velocity.
	void SetMaxSpeed(float maxSpeed);
	/// Set the agent's radius.
	void SetRadius(float radius);
	/// Set the agent's height.
	void SetHeight(float height);
	/// Set the agent's query filter type.
	void SetQueryFilterType(unsigned queryFilterType);
	/// Set the agent's obstacle avoidance type.
	void SetObstacleAvoidanceType(unsigned obstacleAvoidanceType);
	/// Set the agent's navigation quality.
	void SetNavigationQuality(NavigationQuality val);
	/// Set the agent's navigation pushiness.
	void SetNavigationPushiness(NavigationPushiness val);

	/// Return the agent's position.
	Vector3f GetPosition() const;
	/// Return the agent's desired velocity.
	Vector3f GetDesiredVelocity() const;
	/// Return the agent's actual velocity.
	Vector3f GetActualVelocity() const;

	/// Return the agent's requested target position.
	const Vector3f& GetTargetPosition() const { return targetPosition_; }

	/// Return the agent's requested target velocity.
	const Vector3f& GetTargetVelocity() const { return targetVelocity_; }

	/// Return the agent's requested target type, if any.
	CrowdAgentRequestedTarget GetRequestedTargetType() const { return requestedTargetType_; }

	/// Return the agent's  state.
	CrowdAgentState GetAgentState() const;
	/// Return the agent's target state.
	CrowdAgentTargetState GetTargetState() const;

	/// Return true when the node's position should be updated by the CrowdManager.
	bool GetUpdateNodePosition() const { return updateNodePosition_; }

	/// Return the agent id.
	int GetAgentCrowdId() const { return agentCrowdId_; }

	/// Get the agent's max acceleration.
	float GetMaxAccel() const { return maxAccel_; }

	/// Get the agent's max velocity.
	float GetMaxSpeed() const { return maxSpeed_; }

	/// Get the agent's radius.
	float GetRadius() const { return radius_; }

	/// Get the agent's height.
	float GetHeight() const { return height_; }

	/// Get the agent's query filter type.
	unsigned GetQueryFilterType() const { return queryFilterType_; }

	/// Get the agent's obstacle avoidance type.
	unsigned GetObstacleAvoidanceType() const { return obstacleAvoidanceType_; }

	/// Get the agent's navigation quality.
	NavigationQuality GetNavigationQuality() const { return navQuality_; }

	/// Get the agent's navigation pushiness.
	NavigationPushiness GetNavigationPushiness() const { return navPushiness_; }

	/// Return true when the agent has a target.
	bool HasRequestedTarget() const { return requestedTargetType_ != CA_REQUESTEDTARGET_NONE; }

	/// Return true when the agent has arrived at its target.
	bool HasArrived(const float scale = 1.0f) const;
	/// Return true when the agent is in crowd (being managed by a crowd manager).
	bool IsInCrowd() const;

	/// Handle crowd agent being updated. It is called by CrowdManager::Update() via callback.
	//virtual void OnCrowdUpdate(dtCrowdAgent* ag, float dt);

	virtual void OnCrowdVelocityUpdate(dtCrowdAgent* ag, float* pos, float dt);
	/// Handle crowd agent being updated. It is called by CrowdManager::Update() via callback.
	virtual void OnCrowdPositionUpdate(dtCrowdAgent* ag, float* pos, float dt);
	/// Add agent into crowd.
	int AddAgentToCrowd(bool force = false, const Vector3f& initialPosition = Vector3f::ZERO);
	CrowdManager* crowdManager_;

	void setOnPositionVelocityUpdate(std::function<void(const Vector3f& pos, const Vector3f& vel, CrowdAgent* agent)> fun);
	void setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, const unsigned int index, CrowdAgent* agent)> fun);
	void setOnTarget(std::function<void(const Vector3f& pos)> fun);
	void setOnInactive(std::function<void()> fun);
	void SetSeparationWeight(float separationWeight);

	void resetAgent();
	bool isActive();

	static const Vector3f& GetNearestPos();

protected:
	/// Handle node being assigned.
	//virtual void OnNodeSet(Node* node);
	/// Handle node being assigned.
	//virtual void OnSceneSet(Scene* scene);
	/// \todo Handle node transform being dirtied.
	//virtual void OnMarkedDirty(Node* node);
	/// Get internal Detour crowd agent.
	const dtCrowdAgent* GetDetourCrowdAgent() const;
	/// Handle navigation mesh tile added.
	//void HandleNavigationTileAdded(StringHash eventType, VariantMap& eventData);

private:
	/// Update Detour crowd agent parameter.
	void UpdateParameters(unsigned scope = std::numeric_limits<unsigned int>::max());
	
	/// Remove agent from crowd.
	void RemoveAgentFromCrowd();
	/// Crowd manager.
	
	/// Velocity callback.
	CrowdAgentVelocityCallback velocityCallback_;
	/// Height callback.
	CrowdAgentHeightCallback heightCallback_;
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

	float separationWeight_;
	

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

	std::function<void(const Vector3f& pos, const Vector3f& vel, CrowdAgent* agent)> OnPositionVelocityUpdate;
	std::function<Vector3f(const Vector3f& pos, const unsigned int index, CrowdAgent* agent)> OnCrowdFormation;
	std::function<void(const Vector3f& pos)> OnTarget;
	std::function<void()> OnInactive;
	static Vector3f NearestPos;

	bool m_active = false;
};