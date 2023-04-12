#pragma once
#include "engine/Vector.h"

class DirectionalLight {

public:
	DirectionalLight();
	~DirectionalLight();

	void configure(const Vector3f& ambient, const Vector3f& diffuse, const Vector3f& specular);

	void setDirection(const Vector3f& direction) { m_direction = direction; }
	void setColour(const Vector3f& colour)	 { m_colour = colour; }

	const Vector3f& getDirection()		 { return m_direction; }
	const Vector3f& getAmbient()			 { return m_ambient; }
	const Vector3f& getDiffuse()			 { return m_diffuse; }
	const Vector3f& getSpecular()		 { return m_specular; }
	const Vector3f& getColour()			 { return m_colour; }

private:
	Vector3f m_direction;
	Vector3f m_ambient;
	Vector3f m_diffuse;
	Vector3f m_specular;
	Vector3f m_colour;
};