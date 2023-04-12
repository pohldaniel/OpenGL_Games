#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() {

}

DirectionalLight::~DirectionalLight() {

}

void DirectionalLight::configure(const Vector3f& ambient, const Vector3f& diffuse, const Vector3f& specular) {
	m_ambient = ambient;
	m_diffuse = diffuse;
	m_specular = specular;
}