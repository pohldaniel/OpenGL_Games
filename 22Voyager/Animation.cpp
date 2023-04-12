#include "Animation.h"
#include "Player.h"

const float WEAPON_ROTATION_190_DEG = 3.31613f;

Animation::Animation() :
	m_defWeaponRotation(WEAPON_ROTATION_190_DEG),
	m_defWeaponZOffset(-2.5f),
	m_originalWeaponZOffset(-2.5f),
	m_sprintWeaponZOffset(-2.5f),
	m_defWeaponYOffset(-2.0f),
	m_idleYOffset(-2.0f),
	m_weaponMoveForward(false),
	m_weaponMoveUp(true),
	m_swapping(false)
{}

Animation::~Animation()
{}

void Animation::SetWeaponZOffset(float zOffset)
{
	m_originalWeaponZOffset = zOffset;
	m_defWeaponZOffset = zOffset;
}

void Animation::PlaySprintFPS(Model& weapon, Camera& camera, float dt)
{
	const float MAX_ROTATION = 4.5f;
	const float MAX_Y_OFFSET = -2.0f;
	const float MIN_Y_OFFSET = -2.5f;

	m_defWeaponRotation += 7.0f * dt;

	if (m_defWeaponRotation >= MAX_ROTATION)
		m_defWeaponRotation = MAX_ROTATION;

	if (m_weaponMoveUp)
	{
		m_defWeaponYOffset += 1.0f * dt;

		if (m_defWeaponYOffset >= MAX_Y_OFFSET)
		{
			m_weaponMoveUp = false;
			m_defWeaponYOffset = MAX_Y_OFFSET;
		}
	}
	else
	{
		m_defWeaponYOffset -= 1.0f * dt;

		if (m_defWeaponYOffset <= MIN_Y_OFFSET)
		{
			m_weaponMoveUp = true;
			m_defWeaponYOffset = MIN_Y_OFFSET;
		}
	}

	weapon.Draw(camera, glm::vec3(1.7f, m_defWeaponYOffset, m_sprintWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
}

void Animation::PlayWalkFPS(Model& weapon, Camera& camera, float dt)
{
	const float MAX_Z_OFFSET = m_originalWeaponZOffset - 0.4f;
	const float MIN_Z_OFFSET = m_originalWeaponZOffset;
	m_defWeaponYOffset = -2.5f;

	// Rotate the weapon back to 190 degrees if it isn't at 190 degrees 
	if (m_defWeaponRotation >= WEAPON_ROTATION_190_DEG)
	{
		m_defWeaponRotation -= 7.0f * dt;

		if (m_defWeaponRotation <= WEAPON_ROTATION_190_DEG)
			m_defWeaponRotation = WEAPON_ROTATION_190_DEG;
	}

	if (m_defWeaponZOffset <= MAX_Z_OFFSET)
		m_weaponMoveForward = false;
	else if (m_defWeaponZOffset >= MIN_Z_OFFSET)
		m_weaponMoveForward = true;

	if (m_weaponMoveForward)
	{
		m_defWeaponZOffset -= 1.0f * dt;

		// Prevent weapon from going too far into the world
		if (m_defWeaponZOffset <= MAX_Z_OFFSET)
			m_defWeaponZOffset = MAX_Z_OFFSET;
	}
	else
	{
		m_defWeaponZOffset += 1.0f * dt;

		// Prevent weapon from going too far behind the view
		if (m_defWeaponZOffset >= MIN_Z_OFFSET)
			m_defWeaponZOffset = MIN_Z_OFFSET;
	}

	weapon.Draw(camera, glm::vec3(1.7f, -2.0f, m_defWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
}

void Animation::PlayIdleFPS(Model& weapon, AssimpModel& model, Camera& camera, float dt)
{
	// Bring weapon back to original Z position
	if (m_defWeaponZOffset <= m_originalWeaponZOffset)
	{
		m_defWeaponZOffset += 1.5f * dt;

		if (m_defWeaponZOffset >= m_originalWeaponZOffset)
			m_defWeaponZOffset = m_originalWeaponZOffset;
	}
	else
	{
		m_defWeaponZOffset -= 1.5f * dt;

		if (m_defWeaponZOffset <= m_originalWeaponZOffset)
			m_defWeaponZOffset = m_originalWeaponZOffset;
	}

	// Rotate the weapon back to 190 degrees if it isn't at 190 degrees 
	if (m_defWeaponRotation >= WEAPON_ROTATION_190_DEG)
	{
		m_defWeaponRotation -= 7.0f * dt;

		if (m_defWeaponRotation <= WEAPON_ROTATION_190_DEG)
			m_defWeaponRotation = WEAPON_ROTATION_190_DEG;
	}

	const float MAX_Y_OFFSET = -2.0f;
	const float MIN_Y_OFFSET = -2.1f;

	// Simulate breathing
	if (m_weaponMoveUp)
	{
		m_idleYOffset += 0.05f * dt;

		if (m_idleYOffset >= MAX_Y_OFFSET)
		{
			m_weaponMoveUp = false;
			m_idleYOffset = MAX_Y_OFFSET;
		}
	}
	else
	{
		m_idleYOffset -= 0.05f * dt;

		if (m_idleYOffset <= MIN_Y_OFFSET)
		{
			m_weaponMoveUp = true;
			m_idleYOffset = MIN_Y_OFFSET;
		}
	}

	//weapon.Draw(camera, glm::vec3(1.7f, m_idleYOffset, m_defWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
	

	Player::GetInstance().m_shader.ActivateProgram();

	glm::mat4 _model(1.0f);
	glm::mat4 translation = glm::translate(glm::vec3(1.7f, m_idleYOffset, m_defWeaponZOffset));
	glm::mat4 rotation = glm::rotate(m_defWeaponRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 scaleMat = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

	if (true)
	{
		Matrix4f _invView = camera.getInvViewMatrix();

		glm::mat4 invViewMat;


		invViewMat[0][0] = _invView[0][0]; invViewMat[0][1] = _invView[0][1]; invViewMat[0][2] = _invView[0][2]; invViewMat[0][3] = _invView[0][3];
		invViewMat[1][0] = _invView[1][0]; invViewMat[1][1] = _invView[1][1]; invViewMat[1][2] = _invView[1][2]; invViewMat[1][3] = _invView[1][3];
		invViewMat[2][0] = _invView[2][0]; invViewMat[2][1] = _invView[2][1]; invViewMat[2][2] = _invView[2][2]; invViewMat[2][3] = _invView[2][3];
		invViewMat[3][0] = _invView[3][0]; invViewMat[3][1] = _invView[3][1]; invViewMat[3][2] = _invView[3][2]; invViewMat[3][3] = _invView[3][3];

		_model = invViewMat * translation * rotation * scaleMat;
	}
	else
	{
		_model = translation * rotation * scaleMat;
	}

	Player::GetInstance().m_shader.SetMat4("model", _model);
	Player::GetInstance().m_shader.loadVector("lightPos", Vector3f(camera.getPosition()[0], camera.getPosition()[1] + 5.0f, camera.getPosition()[2]));
	Player::GetInstance().m_shader.loadVector("viewPos", camera.getPosition());

	Player::GetInstance().m_shader.SetBool("EnableSpotlight", false);

	if (Player::GetInstance().GetSpotLight() != nullptr && false){
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
	Player::GetInstance().m_shader.loadMatrix("view", camera.getViewMatrix());

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	/*for (unsigned int i = 0; i < m_textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = m_textures[i].m_type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		glUniform1i(glGetUniformLocation(shaderProgram.GetShaderProgram(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].m_id);
	}*/

	model.drawRaw();

	// Unbind textures 
	/*for (GLuint i = 0; i < m_textures.size(); ++i){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}*/
	Player::GetInstance().m_shader.DeactivateProgram();

}

void Animation::PlayFireFPS(Model& weapon, Camera& camera, float dt)
{
	const float MAX_Z_OFFSET = -2.5f;
	const float MIN_Z_OFFSET = -1.5f;
	m_defWeaponYOffset = -2.5f;

	// Rotate the weapon back to 190 degrees if it isn't at 190 degrees 
	if (m_defWeaponRotation >= WEAPON_ROTATION_190_DEG)
	{
		m_defWeaponRotation -= 7.0f * dt;

		if (m_defWeaponRotation <= WEAPON_ROTATION_190_DEG)
			m_defWeaponRotation = WEAPON_ROTATION_190_DEG;
	}

	if (m_defWeaponZOffset <= MAX_Z_OFFSET)
		m_weaponMoveForward = false;
	else if (m_defWeaponZOffset >= MIN_Z_OFFSET)
		m_weaponMoveForward = true;

	if (m_weaponMoveForward)
	{
		m_defWeaponZOffset -= 10.0f * dt;

		// Prevent weapon from going too far into the world
		if (m_defWeaponZOffset <= MAX_Z_OFFSET)
			m_defWeaponZOffset = MAX_Z_OFFSET;
	}
	else
	{
		m_defWeaponZOffset += 10.0f * dt;

		// Prevent weapon from going too far behind the view
		if (m_defWeaponZOffset >= MIN_Z_OFFSET)
			m_defWeaponZOffset = MIN_Z_OFFSET;
	}

	weapon.Draw(camera, glm::vec3(1.7f, m_idleYOffset, m_defWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
}

void Animation::PlayReloadFPS(Model& weapon, Camera& camera, float dt)
{
	const float MAX_ROTATION = 4.0f;
	const float MAX_Y_OFFSET = -2.2f;
	const float MIN_Y_OFFSET = -2.3f;

	m_defWeaponRotation += 5.0f * dt;

	if (m_defWeaponRotation >= MAX_ROTATION)
		m_defWeaponRotation = MAX_ROTATION;

	if (m_weaponMoveUp)
	{
		m_defWeaponYOffset += 0.2f * dt;

		if (m_defWeaponYOffset >= MAX_Y_OFFSET)
		{
			m_weaponMoveUp = false;
			m_defWeaponYOffset = MAX_Y_OFFSET;
		}
	}
	else
	{
		m_defWeaponYOffset -= 0.2f * dt;

		if (m_defWeaponYOffset <= MIN_Y_OFFSET)
		{
			m_weaponMoveUp = true;
			m_defWeaponYOffset = MIN_Y_OFFSET;
		}
	}

	weapon.Draw(camera, glm::vec3(1.7f, m_defWeaponYOffset, m_defWeaponZOffset), glm::vec3(-0.2f, 1.0f, -0.2f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
}

void Animation::PlaySwapTwoWeapons(Model& weaponA, Model& weaponB, Camera& camera, float dt, bool& swapped)
{
	const float MAX_ROTATION = 4.5f;
	const float MAX_Y_OFFSET = -2.0f;
	const float MIN_Y_OFFSET = -2.5f;

	if (!m_swapping)
	{
		m_defWeaponRotation += 7.0f * dt;

		if (m_defWeaponRotation >= MAX_ROTATION)
			m_defWeaponRotation = MAX_ROTATION;

		m_defWeaponYOffset -= 7.0f * dt;
		weaponA.Draw(camera, glm::vec3(1.7f, m_defWeaponYOffset, m_defWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);

		if (m_defWeaponYOffset < -6.0f)
		{
			m_swapping = true;
		}
	}
	else if (m_swapping)
	{
		// Bring weapon back to original Z position
		if (m_defWeaponZOffset <= m_originalWeaponZOffset)
		{
			m_defWeaponZOffset += 1.5f * dt;

			if (m_defWeaponZOffset >= m_originalWeaponZOffset)
				m_defWeaponZOffset = m_originalWeaponZOffset;
		}
		else
		{
			m_defWeaponZOffset -= 1.5f * dt;

			if (m_defWeaponZOffset <= m_originalWeaponZOffset)
				m_defWeaponZOffset = m_originalWeaponZOffset;
		}

		if (m_defWeaponYOffset < -2.0f)
		{
			// Rotate the weapon back to 190 degrees if it isn't at 190 degrees 
			if (m_defWeaponRotation >= WEAPON_ROTATION_190_DEG)
			{
				m_defWeaponRotation -= 7.0f * dt;

				if (m_defWeaponRotation <= WEAPON_ROTATION_190_DEG)
					m_defWeaponRotation = WEAPON_ROTATION_190_DEG;
			}

			m_defWeaponYOffset += 7.0f * dt;
			weaponB.Draw(camera, glm::vec3(1.7f, m_defWeaponYOffset, m_defWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
		}
		else
		{
			swapped = true;
			m_swapping = false;
		}
	}
}

void Animation::PlayFreezeFPS(Model& weapon, Camera& camera, float dt)
{
	//weapon.Draw(camera, glm::vec3(1.7f, -2.0f, m_defWeaponZOffset), glm::vec3(0.0f, 1.0f, 0.0f), m_defWeaponRotation, glm::vec3(1.0f, 1.0f, 1.0f), true);
}