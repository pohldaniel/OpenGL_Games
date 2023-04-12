#include "SpotLight.h"

SpotLight::SpotLight() {

}

SpotLight::~SpotLight() {

}

void SpotLight::configure(const Vector3f& diffuse, const Vector3f& specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff) {

	m_diffuse = diffuse;
	m_specular = specular;
	m_constant = constant;
	m_linear = linear;
	m_quadratic = quadratic;
	m_cutOff = cutOff;
	m_outerCutOff = outerCutOff;
}
