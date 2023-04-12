#include "PointLight.h"

PointLight::PointLight()
{}

PointLight::~PointLight()
{}

void PointLight::configure(const Vector3f& ambient, const Vector3f& diffuse, const Vector3f& specular, float constant, float linear, float quadratic) {

	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
	m_constant = constant;
	m_linear = linear;
	m_quadratic = quadratic;
}
