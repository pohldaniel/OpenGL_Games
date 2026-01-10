#pragma once
#include <array>
#include "Constraint.h"

/// DoF 6 limit indices
enum D6LimitIndexType
{
	D6_LINEAR_X = 0,
	D6_LINEAR_Y,
	D6_LINEAR_Z,
	D6_ANGULAR_X,
	D6_ANGULAR_Y,
	D6_ANGULAR_Z,
};

class PhysicsWorld;
class RigidBody;

class Constraint6DoF : public Constraint{

	friend class RigidBody;

public:

	Constraint6DoF();
	~Constraint6DoF();
	/// Apply constraint frames.
	virtual void ApplyFrames();

	void SetLinearLowerLimit(const Vector3f& linearLower);
	const Vector3f& GetLinearLowerLimit() const;
	void SetLinearUpperLimit(const Vector3f& linearUpper);
	const Vector3f& GetLinearUpperLimit() const;
	void SetAngularLowerLimit(const Vector3f& angularLower);
	const Vector3f& GetAngularLowerLimit() const;
	void SetAngularUpperLimit(const Vector3f& angularUpper);
	const Vector3f& GetAngularUpperLimit() const;
	bool IsLimited(unsigned idx) const;

	/// set the type of the motor (servo or not) (the motor has to be turned on for servo also)
	void SetEnableLinearServo(const std::array<int,3>& enable);
	const std::array<int,3>& GetEnableLinearServo() const;
	void SetEnableAngularServo(const std::array<int,3>& enable);
	const std::array<int,3>& GetEnableAngularServo() const;
	void SetEnableServo(unsigned idx, bool enable);

	/// set the type of the motor (servo or not) (the motor has to be turned on for servo also)
	void SetEnableLinearMotor(const std::array<int, 3>& enable);
	const std::array<int, 3>& GetEnableLinearMotor() const;
	void SetEnableAngularMotor(const std::array<int, 3>& enable);
	const std::array<int, 3>& GetEnableAngularMotor() const;
	void SetEnableMotor(unsigned idx, bool on);

	void SetLinearMaxMotorForce(const Vector3f& force);
	const Vector3f& GetLinearMaxMotorForce() const;
	void SetAngularMaxMotorForce(const Vector3f& force);
	const Vector3f& GetAngularMaxMotorForce() const;
	void SetMaxMotorForce(unsigned idx, float force);

	void SetLinearBounce(const Vector3f& bounce);
	const Vector3f& GetLinearBounce() const;
	void SetAngularBounce(const Vector3f& bounce);
	const Vector3f& GetAngularBounce() const;
	void SetBounce(unsigned idx, float bounce);

	void SetEnableLinearSpring(const std::array<int,3>& enable);
	const std::array<int,3>& GetEnableLinearSpring() const;
	void SetEnableAngularSpring(const std::array<int,3>& enable);
	const std::array<int,3>& GetEnableAngularSpring() const;
	void SetEnableSpring(int idx, bool on);

	/// if limitIfNeeded is true the system will automatically limit the stiffness in necessary 
	/// situations where otherwise the spring would move unrealistically too widely
	void SetLinearStiffness(const Vector3f& stiffness);
	const Vector3f& GetLinearStiffness() const;
	void SetAngularStiffness(const Vector3f& stiffness);
	const Vector3f& GetAngularStiffness() const;
	void SetStiffness(unsigned idx, float stiffness, bool limitIfNeeded = true);
	void SetLimitLinearStiffness(const std::array<int,3>& limit);
	const std::array<int,3>& GetLimitLinearStiffness() const;
	void SetLimitAngularStiffness(const std::array<int,3>& limit);
	const std::array<int,3>& GetLimitAngularStiffness() const;

	/// if limitIfNeeded is true the system will automatically limit the damping in necessary situations 
	/// where otherwise the spring would blow up
	void SetLinearDamping(const Vector3f& damping);
	const Vector3f& GetLinearDamping() const;
	void SetAngularDamping(const Vector3f& damping);
	const Vector3f& GetAngularDamping() const;
	void SetDamping(unsigned idx, float damping, bool limitIfNeeded = true);
	void SetLimitLinearDamping(const std::array<int,3>& limit);
	const std::array<int,3>& GetLimitLinearDamping() const;
	void SetLimitAngularDamping(const std::array<int,3>& limit);
	const std::array<int,3>& GetLimitAngularDamping() const;

	/// Return constraint error reduction parameter.
	void SetLinearERPs(const Vector3f &erps);
	const Vector3f& GetLinearERPs() const { return linearErps_; }
	void SetAngularERPs(const Vector3f &erps);
	const Vector3f& GetAngularERPs() const { return angularErps_; }
	/// Return constraint force mixing parameter.
	void SetLinearCFMs(const Vector3f &cfms);
	const Vector3f& GetLinearCFMs() const { return linearCfms_; }
	void SetAngularCFMs(const Vector3f &cfms);
	const Vector3f& GetAngularCFMs() const { return angularCfms_; }

	/// Return constraint error reduction parameter.
	void SetLinearStopERPs(const Vector3f &erps);
	const Vector3f& GetLinearStopERPs() const { return linearStopErps_; }
	void SetAngularStopERPs(const Vector3f &erps);
	const Vector3f& GetAngularStopERPs() const { return angularStopErps_; }

	/// Return constraint force mixing parameter.
	void SetLinearStopCFMs(const Vector3f &cfms);
	const Vector3f& GetLinearStopCFMs() const { return linearStopCfms_; }
	void SetAngularStopCFMs(const Vector3f &cfms);
	const Vector3f& GetAngularStopCFMs() const { return angularStopCfms_; }

	/// set the current constraint position/orientation as an equilibrium point for all DOF
	void SetEquilibriumPoint();
	/// set the current constraint position/orientation as an equilibrium point for given DOF
	void SetEquilibriumPoint(int idx);
	void SetEquilibriumPoint(int idx, float val);

protected:
	/// Create the constraint.
	virtual void CreateConstraint();
	/// Apply high and low constraint limits.
	virtual void ApplyLimits();

	void SetLimits();
	void SetServo();
	void SetEnableMotor();
	void SetMaxMotorForce();
	void SetBounce();
	void SetEnableSpring();
	void SetStiffness();
	void SetDamping();
	void SetERPs();
	void SetCFMs();
	void SetStopERPs();
	void SetStopCFMs();

	inline bool EqualZero(int val) const { return (val == 0); }

protected:
	Vector3f     linearLowerLimit_;
	Vector3f     linearUpperLimit_;
	Vector3f     angularLowerLimit_;
	Vector3f     angularUpperLimit_;

	std::array<int,3>  linearServoEnable_;
	std::array<int,3>  angularServoEnable_;

	std::array<int,3>  linearMotorEnable_;
	std::array<int,3>  angularMotorEnable_;
	Vector3f     linearMotorForce_;
	Vector3f     angularMotorForce_;

	Vector3f     linearBounceForce_;
	Vector3f     angularBounceForce_;

	std::array<int,3>  linearSpringEnable_;
	std::array<int,3>  angularSpringEnable_;

	Vector3f     linearStiffness_;
	Vector3f     angularStiffness_;
	std::array<int,3>  linearStiffnessLimit_;
	std::array<int,3>  angularStiffnessLimit_;

	Vector3f     linearDamping_;
	Vector3f     angularDamping_;
	std::array<int,3>  linearDampingLimit_;
	std::array<int,3>  angularDampingLimit_;

	Vector3f     linearErps_;
	Vector3f     angularErps_;
	Vector3f     linearCfms_;
	Vector3f     angularCfms_;

	Vector3f     linearStopErps_;
	Vector3f     angularStopErps_;
	Vector3f     linearStopCfms_;
	Vector3f     angularStopCfms_;
};