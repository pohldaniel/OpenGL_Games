#pragma once
#include "engine/Vector.h"

class SpotLight {

public:
	SpotLight();
	~SpotLight();

	void configure(const Vector3f& diffuse, const Vector3f& specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff);
	void setPosition(const Vector3f& position) { m_position = position; }
	void setDirection(const Vector3f& direction) { m_direction = direction; }
	void setDiffuse(const Vector3f& diffuse) { m_diffuse = diffuse; }
	void setSpecular(const Vector3f& specular) { m_specular = specular; }

	const Vector3f& getPosition()		{ return m_position; }
	const Vector3f& getDirection()		{ return m_direction; }
	const Vector3f& getDiffuse()		{ return m_diffuse; }
	const Vector3f& getSpecular()		{ return m_specular; }


	float& getCutOff()				{ return m_cutOff; }
	float& getOuterCutOff()			{ return m_outerCutOff; }
	float& getConstant()			{ return m_constant; }
	float& getLinear()				{ return m_linear; }
	float& getQuadratic()			{ return m_quadratic; }

private:
	Vector3f m_position;
	Vector3f m_direction;
	Vector3f m_diffuse;
	Vector3f m_specular;

	float m_cutOff;
	float m_outerCutOff;
	float m_constant;
	float m_linear;
	float m_quadratic;
};