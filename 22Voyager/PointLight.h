#pragma once
#include "engine/Vector.h"

class PointLight {

public:
	PointLight();
	~PointLight();

	void configure(const Vector3f& ambient, const Vector3f& diffuse, const Vector3f& specular, float constant, float linear, float quadratic);

	void setPosition(const Vector3f& position)		{ m_position = position; }
	void setColour(const Vector3f& colour)	{ m_colour = colour; }

	const Vector3f& getPosition()	{ return m_position; }
	const Vector3f& getColour()		{ return m_colour; }
	const Vector3f& getAmbient()	{ return m_ambient; }
	const Vector3f& getDiffuse()	{ return m_diffuse; }
	const Vector3f& getSpecular()	{ return m_specular; }
	float getConstant()				{ return m_constant; }
	float getLinear()				{ return m_linear; }
	float getQuadratic()			{ return m_quadratic; }

private:
	Vector3f m_position;
	Vector3f m_ambient;
	Vector3f m_diffuse;
	Vector3f m_specular;
	Vector3f m_colour;

	float m_constant;
	float m_linear;
	float m_quadratic;
};