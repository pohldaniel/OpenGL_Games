#include "Character.h"

Character::Character() {
	m_shader = new Shader("shader/quad_trans.vs", "shader/quad_trans.fs");
	m_spriteSheet = new SpritesheetOld("characters.png", CHARACTER_TILE_WIDTH, CHARACTER_TILE_HEIGHT, true, true);
	m_quad = new Quad(xScale, yScale);

	transform = Matrix4f::IDENTITY;
	frame = Matrix4f::IDENTITY;

	position[0] = 150.0f;
	position[1] = HEIGHT - 250.0f;

	prevPosition[0] = 150.0f;
	prevPosition[1] = HEIGHT  - 250.0f;

	speed[0] = 0;
	speed[1] = 0;

	GoesLeft = false;
	GoesRight = false;

	forward = true;
	dead = false;
}

void Character::update(float elapsedTime) {

	prevPosition = position;
	position[0] += speed[0] * elapsedTime;
	//position[0] = position[0] + 2.0f;

	if (position[0] < CHARACTER_WIDTH / 2){
		position[0] = CHARACTER_WIDTH / 2;
		speed[0] = 0;
	}

	if (position[0] > LEVEL_WIDTH* TILE_WIDTH - CHARACTER_WIDTH / 2){
		position[0] = LEVEL_WIDTH * TILE_WIDTH - CHARACTER_WIDTH / 2;
		speed[0] = 0;
	}

	if (GoesLeft) // if left is pressed, accelerate left
	{
		speed[0] -= 1500 * elapsedTime;
		if (speed[0] < -300)
			speed[0] = -300;
	}
	if (GoesRight) // if right is pressed, accelerate right
	{

		speed[0] += 1500 * elapsedTime;
		if (speed[0] > 300)
			speed[0] = 300;
	}
	if (!GoesLeft && !GoesRight) // if no key is pressed, and we're on ground, create drag so the character will stop
	{
		if (speed[1] == 0)
		{
			if (speed[0] > 0)
			{
				speed[0] -= 1500 * elapsedTime;
				if (speed[0] < 10)
					speed[0] = 0;
			}
			if (speed[0] < 0)
			{
				speed[0] += 1500 * elapsedTime;
				if (speed[0] > -10)
					speed[0] = 0;
			}
		}
	}

	// update y position based on speed
	position[1] += speed[1] * elapsedTime;
	// apply gravity
	speed[1] += 0 * elapsedTime;

	// set forward, to be used for drawing the character in the right direction
	if (speed[0] > 0)
		forward = true;
	if (speed[0] < 0)
		forward = false;
}

void Character::render() {

	// if the character moves on ground
	if (position[0] != prevPosition[0]) {
		step = (int)position[0] % 100 / 25;		
	}

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::Translate((position[0]/ CHARACTER_TILE_WIDTH) * xTrans - 1.0f, -1.0f + yTrans * ((HEIGHT - position[1] ) / CHARACTER_TILE_HEIGHT + 0.5f ), 0.0f));
	m_shader->loadMatrix("u_frame", m_spriteSheet->getFrameTransform(step));
	 
	m_quad->render(m_spriteSheet->getTexture());
	glUseProgram(0);
}

Shader*  Character::getShader() const {
	return m_shader;
}