#pragma once
#include <limits>
#include <functional>

#include "../engine/Vector.h"
#include "CrowdManager.h"

enum CrowdAgentRequestedTarget{
	CA_REQUESTEDTARGET_NONE = 0,
	CA_REQUESTEDTARGET_POSITION,
	CA_REQUESTEDTARGET_VELOCITY
};

enum CrowdAgentTargetState{
	CA_TARGET_NONE = 0,
	CA_TARGET_FAILED,
	CA_TARGET_VALID,
	CA_TARGET_REQUESTING,
	CA_TARGET_WAITINGFORQUEUE,
	CA_TARGET_WAITINGFORPATH,
	CA_TARGET_VELOCITY
};

enum NavigationQuality{
	NAVIGATIONQUALITY_LOW = 0,
	NAVIGATIONQUALITY_MEDIUM = 1,
	NAVIGATIONQUALITY_HIGH = 2
};

enum NavigationPushiness{
	NAVIGATIONPUSHINESS_LOW = 0,
	NAVIGATIONPUSHINESS_MEDIUM,
	NAVIGATIONPUSHINESS_HIGH,
	NAVIGATIONPUSHINESS_NONE
};

enum CrowdAgentState : int;

class DebugRenderer;

class CrowdAgent {

	friend class CrowdManager;

public:

	CrowdAgent();
	virtual ~CrowdAgent();
	void OnRenderDebug();

	void setVelocityCallback(const CrowdAgentVelocityCallback& callback);
	void setHeightCallback(const CrowdAgentHeightCallback& callback);
	void setTargetPosition(const Vector3f& position);
	void setMaxAccel(float maxAccel);
	void setMaxSpeed(float maxSpeed);
	void setRadius(float radius);
	void setHeight(float height);
	void setNavigationPushiness(NavigationPushiness val);
	void setSeparationWeight(float separationWeight);

	const CrowdAgentVelocityCallback& getVelocityCallback() const;
	const CrowdAgentHeightCallback& getHeightCallback() const;
	Vector3f getPosition() const;
	Vector3f getDesiredVelocity() const;
	Vector3f getActualVelocity() const;
	const Vector3f& getTargetPosition() const;
	const Vector3f& getTargetVelocity() const;
	CrowdAgentRequestedTarget getRequestedTargetType() const;
	int getAgentCrowdId() const;
	float getMaxAccel() const;
	float getMaxSpeed() const;
	float getRadius() const;
	float getHeight() const;
	unsigned int getQueryFilterType() const;
	unsigned int getObstacleAvoidanceType() const;
	NavigationQuality getNavigationQuality() const;
	NavigationPushiness getNavigationPushiness() const;
	bool hasRequestedTarget() const;


	CrowdAgentState getAgentState() const;
	CrowdAgentTargetState getTargetState() const;
	bool hasArrived(const float scale = 1.0f) const;
	bool isInCrowd() const;

	
	virtual void OnCrowdVelocityUpdate(dtCrowdAgent* ag, float* pos, float dt);
	virtual void OnCrowdPositionUpdate(dtCrowdAgent* ag, float* pos, float dt);
	int addAgentToCrowd(bool force = false, const Vector3f& initialPosition = Vector3f::ZERO);
	

	void setOnPositionVelocityUpdate(std::function<void(const Vector3f& pos, const Vector3f& vel)> fun) const;
	void setOnInactive(std::function<void()> fun) const;
	void setOnTarget(std::function<void(const Vector3f& pos)> fun) const;

	void setOnCrowdFormation(std::function<Vector3f(const Vector3f& pos, const unsigned int index, CrowdAgent* agent)> fun);
	void resetAgent();
	bool isActive();

	static const Vector3f& GetNearestPos();

protected:

	const dtCrowdAgent* getDetourCrowdAgent() const;

private:

	void updateParameters(unsigned int scope = std::numeric_limits<unsigned int>::max());

	CrowdAgentVelocityCallback m_velocityCallback;
	CrowdAgentHeightCallback m_heightCallback;
	int m_agentCrowdId;
	Vector3f m_targetPosition;
	Vector3f m_targetVelocity;
	CrowdAgentRequestedTarget m_requestedTargetType;
	float m_maxAccel;
	float m_maxSpeed;
	float m_radius;
	float m_height;
	float m_separationWeight;
	
	unsigned int m_queryFilterType;
	unsigned int m_obstacleAvoidanceType;
	NavigationQuality m_navQuality;
	NavigationPushiness m_navPushiness;
	Vector3f m_previousPosition;
	CrowdAgentTargetState m_previousTargetState;
	CrowdAgentState m_previousAgentState;
	CrowdManager* m_crowdManager;
	bool m_active;

	mutable std::function<void(const Vector3f& pos, const Vector3f& vel)> OnPositionVelocityUpdate;
	mutable std::function<void()> OnInactive;
	mutable std::function<void(const Vector3f& pos)> OnTarget;

	std::function<Vector3f(const Vector3f& pos, const unsigned int index, CrowdAgent* agent)> OnCrowdFormation;
	
	static Vector3f NearestPos;
};