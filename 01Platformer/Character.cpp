#include "Character.h"
#include "stb\stb_image.h"

Character::Character() {
	m_quadShader = new Shader("shader/quad2.vs", "shader/quad2.fs");
	m_sizeX = (float)(TILE_SIZE_X / 2);
	m_sizeY = (float)(TILE_SIZE_Y / 2);

	createBuffer();
	loadTileset();
	transform = Matrix4f::IDENTITY;
	frame = Matrix4f::IDENTITY;

	position[0] = 150.0f;
	position[1] = RESOLUTION_Y - 250;

	prevPosition[0] = 150.0f;
	prevPosition[1] = RESOLUTION_Y - 250;

	speed[0] = 0;
	speed[1] = 0;

	GoesLeft = false;
	GoesRight = false;

	forward = true;
	dead = false;
}

void Character::createBuffer() {
	m_vertex.push_back(-1.0 * m_sizeX); m_vertex.push_back(-1.0 * m_sizeY); m_vertex.push_back(0.0); m_vertex.push_back(0.0 * m_sizeTex); m_vertex.push_back(0.0 * m_sizeTex);
	m_vertex.push_back(-1.0 * m_sizeX); m_vertex.push_back(1.0 * m_sizeY); m_vertex.push_back(0.0); m_vertex.push_back(0.0 * m_sizeTex); m_vertex.push_back(1.0 * m_sizeTex);
	m_vertex.push_back(1.0 * m_sizeX); m_vertex.push_back(1.0 * m_sizeY); m_vertex.push_back(0.0); m_vertex.push_back(1.0 * m_sizeTex); m_vertex.push_back(1.0 * m_sizeTex);
	m_vertex.push_back(1.0 * m_sizeX); m_vertex.push_back(-1.0 * m_sizeY); m_vertex.push_back(0.0); m_vertex.push_back(1.0 * m_sizeTex); m_vertex.push_back(0.0 * m_sizeTex);

	static const GLushort index[] = {
		0, 1, 2,
		0, 2, 3
	};

	unsigned int quadVBO, indexQuad;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertex.size() * sizeof(float), &m_vertex[0], GL_STATIC_DRAW);

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	//Texture Coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	//Indices
	glGenBuffers(1, &indexQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

Matrix4f Character::calcTileFrameTransform(int id) {
	Matrix4f scale = Matrix4f::Scale(tileScaleX, tileScaleY, 0.0f);
	Matrix4f trans = Matrix4f::Translate((double)(id % tileCountX), (double)(id / tileCountX), 0.0f);
	return trans * scale;
}

void Character::loadTileset() {
	stbi_set_flip_vertically_on_load(true);
	int width, height, numCompontents;
	unsigned char* imageData = stbi_load("characters.png", &width, &height, &numCompontents, 4);

	tileCountX = width / MAP_TILE_DIM_X;
	tileCountY = height / MAP_TILE_DIM_Y;
	tileScaleX = 1.0 / (double)(tileCountX);
	tileScaleY = 1.0 / (double)(tileCountY);

	totalFrames = tileCountX * tileCountY;

	/*for (int i = 0; i < totalFrames; i++) {
		tileFrameTransforms.push_back(calcTileFrameTransform(i));
	}*/

	totalFrames = tileCountX * tileCountY;
	for (int i = totalFrames - 1, j = tileCountX - 1; i >= 0; i--, j -= 2) {
		tileFrameTransforms.push_back(calcTileFrameTransform(i - j));
		if (i % tileCountX == 0 && i < (totalFrames - 1)) {
			j = (tileCountX + 1);
		}
	}


	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);
}

void Character::update(float elapsedTime) {

	prevPosition = position;
	position[0] += speed[0] * elapsedTime;
	//position[0] = position[0] + 2.0f;

	if (position[0] < CHARACTER_WIDTH / 2){
		position[0] = CHARACTER_WIDTH / 2;
		speed[0] = 0;
	}

	if (position[0] > LEVEL_WIDTH* TILE_SIZE - CHARACTER_WIDTH / 2){
		position[0] = LEVEL_WIDTH * TILE_SIZE - CHARACTER_WIDTH / 2;
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
	speed[1] += 1500 * elapsedTime;

	// set forward, to be used for drawing the character in the right direction
	if (speed[0] > 0)
		forward = true;
	if (speed[0] < 0)
		forward = false;

	//std::cout << position[0] << std::endl;
	//std::cout << speed[0] << std::endl;
	//std::cout << GoesRight << std::endl;
}

void Character::render2() {

	// if the character moves on ground
	if (position[0] != prevPosition[0]) {
		step = (int)position[0] % 100 / 25;
		
	}

	glUseProgram(m_quadShader->m_program);
	m_quadShader->loadMatrix("transform", Matrix4f::Translate(position[0] -  WIDTH * 0.5f, 0.5 * HEIGHT - position[1], 0.0f) * Matrix4f::Scale(WIDTH1PX, HEIGHT1PX, 0.0f));
	m_quadShader->loadMatrix("frame", tileFrameTransforms[step]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

Shader*  Character::getShader() const {
	return m_quadShader;
}