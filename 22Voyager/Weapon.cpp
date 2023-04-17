#include "Weapon.h"
#include "Utils.h"
#include "Physics.h"
#include "Player.h"

Weapon::Weapon() :
	m_maxAmmo(0),
	m_ammoCount(0),
	m_currFireRateTime(0.0f),
	m_fireRate(0.0f),
	m_currReloadTime(0.0f),
	m_maxReloadTimer(0.0f) {
}

Weapon::~Weapon(){

}

void Weapon::init(GLchar* path) {
	m_mesh.loadModel(path);
	m_muzzle = new RenderableObject(Vector3f(2.0f, -2.5f, -2.5f), Vector3f(1.0f, 1.0f, 1.0f), "quad", "muzzle", "muzzleFlash");
}

void Weapon::Configure(int maxAmmo, float fireRate, float reloadTime, int damage) {
	m_maxAmmo = maxAmmo;
	m_fireRate = fireRate;
	m_maxReloadTimer = reloadTime;
	m_currFireRateTime = m_fireRate;
	m_ammoCount = m_maxAmmo;
	m_damage = damage;
}

void Weapon::RestartWeapon() {
	m_ammoCount = m_maxAmmo;
}

void Weapon::Update(float dt) {

	if (m_currFireRateTime > m_fireRate) {

		m_currFireRateTime = m_fireRate;

	}else {
		m_currFireRateTime += 0.79f * dt;
	}
}

void Weapon::Fire(Weapon* weapon, Camera& camera, float dt, bool& firing, bool& reloading) {
	m_animator.PlayIdleFPS(this, camera, dt);

	if (m_currFireRateTime > m_fireRate) {

		Physics::GetInstance().CastRay();
		m_animator.PlayFireFPS(weapon, camera, dt);

		--m_ammoCount;

		if (m_ammoCount <= 0) {
			m_ammoCount = 0;
			firing = false;
			reloading = true;
		}

		// Play muzzle flash effect (render textured quad in eye space in front of weapon)
		m_transform.reset();
		m_transform.scale(Utils::GetInstance().RandomNumBetweenTwo(2.0f, 2.5f), Utils::GetInstance().RandomNumBetweenTwo(2.0f, 2.5f), 1.0f);
		m_transform.rotate(Vector3f(0.0f, 0.0f, 1.0f), Utils::GetInstance().RandomNumBetweenTwo(1.0f, 360.0f) * _180_ON_PI);
		m_transform.translate(0.9f, -1.4f, -6.5f);

		m_muzzle->draw(camera, m_transform.getTransformationMatrix());

		m_currFireRateTime = 0.0f;
	}
}

void Weapon::Reload(Weapon* weapon, Camera& camera, float dt, bool& reloading) {
	m_animator.PlayReloadFPS(weapon, camera, dt);
	m_currReloadTime += 0.4f * dt;

	if (m_currReloadTime >= m_maxReloadTimer) {
		m_ammoCount = m_maxAmmo;
		m_currReloadTime = 0.0f;
		reloading = false;
	}
}

void Weapon::draw(Camera& camera, const Matrix4f& model, bool bDrawRelativeToCamera) {
	
	Player::GetInstance().m_shader.ActivateProgram();
	Player::GetInstance().m_shader.loadMatrix("model", model);
	Player::GetInstance().m_shader.loadVector("lightPos", Vector3f(camera.getPosition()[0], camera.getPosition()[1] + 5.0f, camera.getPosition()[2]));
	Player::GetInstance().m_shader.loadVector("viewPos", camera.getPosition());

	Player::GetInstance().m_shader.SetBool("EnableSpotlight", false);

	if (Player::GetInstance().GetSpotLight() != nullptr && false) {
		Player::GetInstance().m_shader.loadVector("spotlight.position", Player::GetInstance().GetSpotLight()->getPosition());
		Player::GetInstance().m_shader.loadVector("spotlight.direction", Player::GetInstance().GetSpotLight()->getDirection());
		Player::GetInstance().m_shader.loadVector("spotlight.diffuse", Player::GetInstance().GetSpotLight()->getDiffuse());
		Player::GetInstance().m_shader.loadVector("spotlight.specular", Player::GetInstance().GetSpotLight()->getSpecular());
		Player::GetInstance().m_shader.SetFloat("spotlight.constant", Player::GetInstance().GetSpotLight()->getConstant());
		Player::GetInstance().m_shader.SetFloat("spotlight.linear", Player::GetInstance().GetSpotLight()->getLinear());
		Player::GetInstance().m_shader.SetFloat("spotlight.quadratic", Player::GetInstance().GetSpotLight()->getQuadratic());
		Player::GetInstance().m_shader.SetFloat("spotlight.cutOff", glm::cos(glm::radians(Player::GetInstance().GetSpotLight()->getCutOff())));
		Player::GetInstance().m_shader.SetFloat("spotlight.outerCutOff", glm::cos(glm::radians(Player::GetInstance().GetSpotLight()->getOuterCutOff())));
	}

	Player::GetInstance().m_shader.loadMatrix("projection", camera.getPerspectiveMatrix());
	Player::GetInstance().m_shader.loadMatrix("view", bDrawRelativeToCamera ? Matrix4f::IDENTITY : camera.getViewMatrix());
	Player::GetInstance().m_shader.loadInt("texture_diffuse1", 0);

	Material::GetMaterials()[m_materialIndex].bind(1);
	m_mesh.drawRaw();
	Material::GetMaterials()[m_materialIndex].unbind(1);

	Player::GetInstance().m_shader.DeactivateProgram();
}