#include "Character.h"

Character::Character() {
	m_shader = new Shader("shader/quad_trans.vs", "shader/quad_trans.fs");
	m_spriteSheet = new SpritesheetOld("characters.png", CHARACTER_TILE_WIDTH, CHARACTER_TILE_HEIGHT, true, true);
	m_quad = new Quad(xScale, yScale);

	m_position[0] = 150.0f;
	m_position[1] = HEIGHT - 250.0f;

	m_speed[0] = 0;
	m_speed[1] = 0;

	goesLeft = false;
	goesRight = false;

	forward = true;
	dead = false;
}

void Character::update(float elapsedTime) {

	if (!dead) // We do all this logic only if it's not dead
	{
		// save previous position
		m_prevPosition = m_position;

		// update x position based on speed, but make sure we don't go outside the level
		m_position[0] += m_speed[0] * elapsedTime;
		if (m_position[0] < CHARACTER_WIDTH / 2)
		{
			m_position[0] = CHARACTER_WIDTH / 2;
			m_speed[0] = 0;
		}
		if (m_position[0] > LEVEL_WIDTH* TILE_WIDTH - CHARACTER_WIDTH / 2)
		{
			m_position[0] = LEVEL_WIDTH * TILE_WIDTH - CHARACTER_WIDTH / 2;
			m_speed[0] = 0;
		}
		if (goesLeft) // if left is pressed, accelerate left
		{
			m_speed[0] -= 1500 * elapsedTime;
			if (m_speed[0] < -300)
				m_speed[0] = -300;
		}
		if (goesRight) // if right is pressed, accelerate right
		{
			m_speed[0] += 1500 * elapsedTime;
			if (m_speed[0] > 300)
				m_speed[0] = 300;
		}
		if (!goesLeft && !goesRight) // if no key is pressed, and we're on ground, create drag so the character will stop
		{
			if (m_speed[1] == 0)
			{
				if (m_speed[0] > 0)
				{
					m_speed[0] -= 1500 * elapsedTime;
					if (m_speed[0] < 10)
						m_speed[0] = 0;
				}
				if (m_speed[0] < 0)
				{
					m_speed[0] += 1500 * elapsedTime;
					if (m_speed[0] > -10)
						m_speed[0] = 0;
				}
			}
		}

		// update y position based on speed
		m_position[1] += m_speed[1] * elapsedTime;
		// apply gravity
		m_speed[1] += 1500 * elapsedTime;

		// set forward, to be used for drawing the character in the right direction
		if (m_speed[0] > 0)
			forward = true;
		if (m_speed[0] < 0)
			forward = false;
	}else{
		// if it's dead, just slowly float up
		m_position[1] -= 200 * elapsedTime;
	}
}

void Character::stopFalling(double collisionSize){
	// this is called when we have a bottom collision and we need to stop falling
	if (!dead){
		m_speed[1] = 0;
		m_position[1] -= collisionSize;
	}
}

void Character::jump(bool fullJump){
	// this is called when the user presses the jump key
	if (!dead){
		if (fullJump)
			m_speed[1] = -700;
		else
			m_speed[1] = -400;
	}
}

void Character::bounceTop(){
	// this is called when we have a top collision and we need to bounce back
	if (!dead){
		m_speed[1] = abs(m_speed[1]);
	}
}

void Character::stopMovingLeft(double collisionSize){
	// this is called when we have a left collision
	if (!dead){
		m_speed[0] = 0;
		m_position[0] += collisionSize;
	}
}

void Character::stopMovingRight(double collisionSize){
	// this is called when we have a right collision
	if (!dead){
		m_speed[0] = 0;
		m_position[0] -= collisionSize;
	}
}

bool Character::isDead(){
	// returns true if the character is dead
	return dead;
}

void Character::die(){
	// kills the character
	dead = true;
}

void Character::reset(){
	// resets the character at the beginning of the level
	m_position[0] = 150;
	m_position[1] = HEIGHT - 250;

	m_speed[0] = 0;
	m_speed[1] = 0;

	goesLeft = false;
	goesRight = false;

	forward = true;
	dead = false;
}

void Character::render() {

	// if the character moves on ground
	if (m_position[0] != m_prevPosition[0]) {
		step = (int)m_position[0] % 100 / 25;
	}
	
	//Matrix4f transform = Matrix4f::Translate((position[0] / CHARACTER_TILE_WIDTH) * xTrans - 1.0f, -1.0f + yTrans * ((HEIGHT - position[1]) / CHARACTER_TILE_HEIGHT + 0.5f), 0.0f);

	/*std::cout << transform[0][0] << "  " << transform[0][1] << "  " << transform[0][2] << "  " << transform[0][3] << std::endl;
	std::cout << transform[1][0] << "  " << transform[1][1] << "  " << transform[1][2] << "  " << transform[1][3] << std::endl;
	std::cout << transform[2][0] << "  " << transform[2][1] << "  " << transform[2][2] << "  " << transform[2][3] << std::endl;
	std::cout << transform[3][0] << "  " << transform[3][1] << "  " << transform[3][2] << "  " << transform[3][3] << std::endl;
	std::cout << "-------------------" << std::endl;*/

	//Matrix4f m_transform = Matrix4f::Translate(m_transform, (position[0] / CHARACTER_TILE_WIDTH) * xTrans - 1.0f, -1.0f + yTrans * ((HEIGHT - position[1]) / CHARACTER_TILE_HEIGHT + 0.5f), 0.0f);
	//m_transform.translate((position[0] / CHARACTER_TILE_WIDTH) * xTrans - 1.0f, -1.0f + yTrans * ((HEIGHT - position[1]) / CHARACTER_TILE_HEIGHT + 0.5f), 0.0f);

	/*std::cout << m_transform[0][0] << "  " << m_transform[0][1] << "  " << m_transform[0][2] << "  " << m_transform[0][3] << std::endl;
	std::cout << m_transform[1][0] << "  " << m_transform[1][1] << "  " << m_transform[1][2] << "  " << m_transform[1][3] << std::endl;
	std::cout << m_transform[2][0] << "  " << m_transform[2][1] << "  " << m_transform[2][2] << "  " << m_transform[2][3] << std::endl;
	std::cout << m_transform[3][0] << "  " << m_transform[3][1] << "  " << m_transform[3][2] << "  " << m_transform[3][3] << std::endl;
	std::cout << "-------------------" << std::endl;*/
	//std::cout << "Offstet: " << offset << std::endl;

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::Translate(m_transform, (m_position[0] / CHARACTER_TILE_WIDTH) * xTrans - 1.0f, -1.0f + yTrans * ((HEIGHT - m_position[1]) / CHARACTER_TILE_HEIGHT + 0.5f), 0.0f));
	m_shader->loadMatrix("u_frame", m_spriteSheet->getFrameTransform(step));
	 
	m_quad->render(m_spriteSheet->getTexture());
	glUseProgram(0);
}