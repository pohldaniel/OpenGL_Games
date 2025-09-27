#include "Weapon.h"
#include "Utils.h"
#include "Physics.h"
#include "Player.h"
#include "Constants.h"

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

void Weapon::init(GLchar* path, const Shader* shader) {
	m_mesh.loadModel(path);	
	m_shader = shader;
	m_muzzle = RenderableObject("quad", "muzzle", "muzzleFlash");
}

void Weapon::configure(int maxAmmo, float fireRate, float reloadTime, int damage) {
	m_maxAmmo = maxAmmo;
	m_fireRate = fireRate;
	m_maxReloadTimer = reloadTime;
	m_currFireRateTime = m_fireRate;
	m_ammoCount = m_maxAmmo;
	m_damage = damage;
}

void Weapon::restartWeapon() {
	m_ammoCount = m_maxAmmo;
}

void Weapon::update(float dt) {

	if (m_currFireRateTime > m_fireRate) {
		m_currFireRateTime = m_fireRate;
	}else {
		m_currFireRateTime += 0.79f * dt;
	}
}

void Weapon::fire(Weapon* weapon, Camera& camera, float dt, bool& firing, bool& reloading) {
	m_animator.PlayIdleFPS(this, camera, dt);

	if (m_currFireRateTime > m_fireRate) {

		Physics::GetInstance().CastRay();
		m_animator.PlayFireFPS(weapon, camera, dt);
		Globals::soundManager.get("player").playStacked("res/Audio/AR_Fired.wav");
		--m_ammoCount;

		if (m_ammoCount <= 0) {
			m_ammoCount = 0;
			firing = false;
			reloading = true;
		}

		// Play muzzle flash effect (render textured quad in eye space in front of weapon)
		m_muzzle.setScale(Utils::GetInstance().RandomNumBetweenTwo(2.0f, 2.5f), Utils::GetInstance().RandomNumBetweenTwo(2.0f, 2.5f), 1.0f);
		m_muzzle.setOrientation(Vector3f(0.0f, 0.0f, 1.0f), Utils::GetInstance().RandomNumBetweenTwo(1.0f, 360.0f) * _180_ON_PI);
		m_muzzle.setPosition(0.9f, -1.4f, -6.5f);
		m_muzzle.draw(camera);

		m_currFireRateTime = 0.0f;
	}
}

void Weapon::reload(Weapon* weapon, Camera& camera, float dt, bool& reloading) {
	m_animator.PlayReloadFPS(weapon, camera, dt);
	m_currReloadTime += 0.4f * dt;

	if (m_currReloadTime >= m_maxReloadTimer) {
		m_ammoCount = m_maxAmmo;
		m_currReloadTime = 0.0f;
		reloading = false;
	}
}

void Weapon::draw(Camera& camera, const Matrix4f& model, bool bDrawRelativeToCamera) {
	
	m_shader->use();
	m_shader->loadMatrix("model", model);
	m_shader->loadVector("lightPos", Vector3f(camera.getPosition()[0], camera.getPosition()[1] + 5.0f, camera.getPosition()[2]));
	m_shader->loadVector("viewPos", camera.getPosition());
	m_shader->loadBool("EnableSpotlight", false);

	if (Player::GetInstance().GetSpotLight() != nullptr && false) {
		m_shader->loadVector("spotlight.position", Player::GetInstance().GetSpotLight()->getPosition());
		m_shader->loadVector("spotlight.direction", Player::GetInstance().GetSpotLight()->getDirection());
		m_shader->loadVector("spotlight.diffuse", Player::GetInstance().GetSpotLight()->getDiffuse());
		m_shader->loadVector("spotlight.specular", Player::GetInstance().GetSpotLight()->getSpecular());
		m_shader->loadFloat("spotlight.constant", Player::GetInstance().GetSpotLight()->getConstant());
		m_shader->loadFloat("spotlight.linear", Player::GetInstance().GetSpotLight()->getLinear());
		m_shader->loadFloat("spotlight.quadratic", Player::GetInstance().GetSpotLight()->getQuadratic());
		m_shader->loadFloat("spotlight.cutOff", cosf(Player::GetInstance().GetSpotLight()->getCutOff() * PI_ON_180));
		m_shader->loadFloat("spotlight.outerCutOff", cosf(Player::GetInstance().GetSpotLight()->getOuterCutOff()* PI_ON_180));
	}

	m_shader->loadMatrix("projection", camera.getPerspectiveMatrix());
	m_shader->loadMatrix("view", bDrawRelativeToCamera ? Matrix4f::IDENTITY : camera.getViewMatrix());
	m_shader->loadInt("texture_diffuse1", 0);

	m_mesh.getMesh()->getMaterial().bind(1u);
	m_mesh.drawRaw();
	m_mesh.getMesh()->getMaterial().unbind(1u);

	m_shader->unuse();
}