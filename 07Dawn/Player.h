#pragma once
#include "Character.h"
#include "TilesetManager.h"

class Player : public Character {

public:
	Player();
	void draw() override;
	void update(float deltaTime) override;
	
	

	int getWidth() const override;
	int getHeight() const override;
	unsigned short getNumActivityTextures(Enums::ActivityType activity)override;

	void setCharacterType(std::string characterType);
	Vector3f getPosition();
	void setTicketForItemTooltip();
	void setTicketForSpellTooltip();

	static Player& Get();
	~Player();

	static Player s_instance;

	const CharacterType* m_characterType;

	void init(int x, int y);

	unsigned char  movementSpeed;
	unsigned int ticketForItemTooltip;
	unsigned int ticketForSpellTooltip;

	float m_duration = 0.01f;
	float m_currentspeed = 1.0f;
	float m_inverseSpeed = 0.8f;

	void Move(float deltaTime);
	void Animate(float deltaTime);
	void processInput();
	
};