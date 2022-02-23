#include "Level.h"
#include "Character.h"

Level::Level() {
	m_levelShader = new Shader("shader/quad_array.vs", "shader/quad_array.fs");
	m_spriteSheet = new Spritesheet("tileset.png", MAP_TILE_WIDTH, MAP_TILE_HEIGHT, true, true);
	m_levelQuad = new Quad(xScale, yScale);

	m_clouds = new Texture("clouds.png", true);
	m_cloudShader = new Shader("shader/quad.vs", "shader/quad.fs");
	m_cloudQuad = new Quad(1.0f , 1.0f);

	loadLevel();
}

void Level::render() {

	double start_x = Globals::offset;
	while (start_x > WIDTH2) {
		start_x -= WIDTH2;
	}

	glUseProgram(m_cloudShader->m_program);	
	m_cloudShader->loadMatrix("u_transform", Matrix4f::Translate(m_transform,  - (start_x / WIDTH), 0.0f, 0.0f) );
	m_cloudQuad->render(m_clouds->getTexture());	

	m_cloudShader->loadMatrix("u_transform", Matrix4f::Translate(m_transform, 2.0f - (start_x / WIDTH), 0.0f, 0.0f));
	m_cloudQuad->render(m_clouds->getTexture());
	glUseProgram(0);

	glUseProgram(m_levelShader->m_program);
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {

			//if ((x + 1) * TILE_WIDTH >= offset && x * TILE_WIDTH < offset + WIDTH){

				if (levelMatrix[y][x] != -1) {
					m_levelShader->loadMatrix("u_transform", Matrix4f::Translate(m_transform, (x + 0.5f - (Globals::offset / CHARACTER_TILE_WIDTH)) * xTrans - 1.0f, 1.0f - yTrans * (y + 0.5f), 0.0f));
					m_levelShader->loadInt("u_layer", levelMatrix[y][x]);
					m_levelQuad->render(m_spriteSheet->getAtlas());
				}

			//}

		}
	}
	glUseProgram(0);		
}

void Level::loadLevel() {

	// Default nothing map
	for (int j = 0; j < LEVEL_HEIGHT; j++){
		for (int i = 0; i < LEVEL_WIDTH; i++){
			levelMatrix[j][i] = -1;
			levelSolids[j][i] = false;
			levelCollectible[j][i] = false;
		}
	}

	// Load map from file
	std::ifstream myfile("level.txt");
	if (myfile.is_open()) {
		for (int j = 0; j < LEVEL_HEIGHT; j++) {
			for (int i = 0; i < LEVEL_WIDTH; i++) {
				int tmp;
				myfile >> tmp;
				levelMatrix[j][i] = tmp;

				levelSolids[j][i] = tileIsSolid(levelMatrix[j][i]);
				levelCollectible[j][i] = tileIsCollectible(levelMatrix[j][i]);
			}
		}
	}
}

bool Level::tileIsSolid(int tileNo){

	for (int i = 0; i < 34; i++){
		if (solids[i] == tileNo){
			return true;
		}
	}
	return false;
}

bool Level::tileIsCollectible(int tileNo){

	for (int i = 0; i < 8; i++){
		if (collectibles[i] == tileNo){
			return true;
		}
	}
	return false;
}

CollisionDistances Level::characterCollides(Character *character) {
	// this returns collision details between the character and the tiles
	int noCollisions = 0;
	CollisionDistances collisions[9];

	CollisionDistances cummulatedCollision;
	cummulatedCollision.top = 0;
	cummulatedCollision.bottom = 0;
	cummulatedCollision.left = 0;
	cummulatedCollision.right = 0;
	
	double charTop = character->getPosition()[1] - CHARACTER_HEIGHT;
	double charBottom = character->getPosition()[1];
	double charLeft = character->getPosition()[0] - CHARACTER_WIDTH / 2 + 3;
	double charRight = character->getPosition()[0] + CHARACTER_WIDTH / 2 - 4;

	// only check the tiles that the character is close to
	int startX = (int)(charLeft / TILE_WIDTH);
	if (startX < 0)
		startX = 0;
	int startY = (int)(charTop / TILE_WIDTH);
	if (startY < 0)
		startY = 0;
	int endX = (int)(charRight / TILE_WIDTH);
	if (endX > LEVEL_WIDTH - 1)
		endX = LEVEL_WIDTH - 1;
	int endY = (int)(charBottom / TILE_WIDTH);
	if (endY > LEVEL_HEIGHT - 1)
		endY = LEVEL_HEIGHT - 1;

	for (int i = startY; i <= endY; i++)
	{
		for (int j = startX; j <= endX; j++)
		{
			if (levelSolids[i][j])
			{
				double tileTop = i * TILE_WIDTH;
				double tileBottom = (i + 1) * TILE_WIDTH - 1;
				double tileLeft = j * TILE_WIDTH;
				double tileRight = (j + 1) * TILE_WIDTH - 1;

				// If it's a collision
				if (charTop < tileBottom && charBottom > tileTop && charRight > tileLeft && charLeft < tileRight)
				{
					// add this collision to the list
					collisions[noCollisions].top = abs(charTop - tileBottom);
					collisions[noCollisions].bottom = abs(charBottom - tileTop);
					collisions[noCollisions].left = abs(charLeft - tileRight);
					collisions[noCollisions].right = abs(charRight - tileLeft);

					collisions[noCollisions].keepSmallest();

					// add the collision details to the cummulated collision
					cummulatedCollision.top += collisions[noCollisions].top;
					cummulatedCollision.bottom += collisions[noCollisions].bottom;
					cummulatedCollision.left += collisions[noCollisions].left;
					cummulatedCollision.right += collisions[noCollisions].right;

					noCollisions++;
				}
			}
		}
	}

	// whichever side collides the most, that side is taken into consideration
	cummulatedCollision.keepLargest();
	if (cummulatedCollision.top != 0)
	{
		cummulatedCollision.top = 0;
		for (int i = 0; i < noCollisions; i++)
		{
			cummulatedCollision.top = fmax(cummulatedCollision.top, collisions[i].top);
		}
	}
	if (cummulatedCollision.bottom != 0)
	{
		cummulatedCollision.bottom = 0;
		for (int i = 0; i < noCollisions; i++)
		{
			cummulatedCollision.bottom = fmax(cummulatedCollision.bottom, collisions[i].bottom);
		}
	}
	if (cummulatedCollision.left != 0)
	{
		cummulatedCollision.left = 0;
		for (int i = 0; i < noCollisions; i++)
		{
			cummulatedCollision.left = fmax(cummulatedCollision.left, collisions[i].left);
		}
	}
	if (cummulatedCollision.right != 0)
	{
		cummulatedCollision.right = 0;
		for (int i = 0; i < noCollisions; i++)
		{
			cummulatedCollision.right = fmax(cummulatedCollision.right, collisions[i].right);
		}
	}

	return cummulatedCollision;
}

int Level::pickUpCollectibles(Character* character) {
	// this method checks for collisions with coins and returns the number
	int noCollisions = 0;

	double charTop = character->getPosition()[1] - CHARACTER_HEIGHT;
	double charBottom = character->getPosition()[1];
	double charLeft = character->getPosition()[0] - CHARACTER_WIDTH / 2 + 3;
	double charRight = character->getPosition()[0] + CHARACTER_WIDTH / 2 - 4;

	int startX = (int)(charLeft / TILE_WIDTH);
	if (startX < 0)
		startX = 0;
	int startY = (int)(charTop / TILE_WIDTH);
	if (startY < 0)
		startY = 0;
	int endX = (int)(charRight / TILE_WIDTH);
	if (endX > LEVEL_WIDTH - 1)
		endX = LEVEL_WIDTH - 1;
	int endY = (int)(charBottom / TILE_WIDTH);
	if (endY > LEVEL_HEIGHT - 1)
		endY = LEVEL_HEIGHT - 1;

	for (int i = startY; i <= endY; i++){
		for (int j = startX; j <= endX; j++){

			if (levelCollectible[i][j]){
				double tileTop = i * TILE_WIDTH;
				double tileBottom = (i + 1) * TILE_WIDTH - 1;
				double tileLeft = j * TILE_WIDTH;
				double tileRight = (j + 1) * TILE_WIDTH - 1;

				// If it's a collision
				if (charTop < tileBottom && charBottom > tileTop && charRight > tileLeft && charLeft < tileRight){
					noCollisions++;
					levelMatrix[i][j] = -1; // remove the coin by resetting the tile position
					levelCollectible[i][j] = false;
				}
			}
		}
	}

	return noCollisions;
}

bool Level::levelExit(Character* character)
{
	// returns true if the character collides with the level exit portal
	int noCollisions = 0;

	double charTop = character->getPosition()[1] - CHARACTER_HEIGHT;
	double charBottom = character->getPosition()[1];
	double charLeft = character->getPosition()[0] - CHARACTER_WIDTH / 2 + 3;
	double charRight = character->getPosition()[0] + CHARACTER_WIDTH / 2 - 4;

	int startX = (int)(charLeft / TILE_WIDTH);
	if (startX < 0)
		startX = 0;
	int startY = (int)(charTop / TILE_WIDTH);
	if (startY < 0)
		startY = 0;
	int endX = (int)(charRight / TILE_WIDTH);
	if (endX > LEVEL_WIDTH - 1)
		endX = LEVEL_WIDTH - 1;
	int endY = (int)(charBottom / TILE_WIDTH);
	if (endY > LEVEL_HEIGHT - 1)
		endY = LEVEL_HEIGHT - 1;

	for (int i = startY; i <= endY; i++){
		for (int j = startX; j <= endX; j++){
			if (levelMatrix[i][j] == 11){
				double tileTop = i * TILE_WIDTH;
				double tileBottom = (i + 1) * TILE_WIDTH - 1;
				double tileLeft = j * TILE_WIDTH;
				double tileRight = (j + 1) * TILE_WIDTH - 1;

				// If it's a collision
				if (charTop < tileBottom && charBottom > tileTop&& charRight > tileLeft&& charLeft < tileRight){
					return true;
				}
			}
		}
	}

	return false;
}

Level::~Level() {
	delete m_levelQuad;
	delete m_levelShader;
	delete m_spriteSheet;

	delete m_clouds;
	delete m_cloudShader;
	delete m_cloudQuad;
}