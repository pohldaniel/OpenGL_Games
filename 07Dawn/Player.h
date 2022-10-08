#pragma once
#include "Character.h"
#include "TilesetManager.h"

class Player : public Character {

public:
	Player();
	void draw() override;
	void Move(float deltaTime) override;
	Enums::Direction GetDirection() override;
	Enums::Direction GetDirectionRNG() override;
	int getWidth() const override;
	int getHeight() const override;
	unsigned short getNumActivityTextures(Enums::ActivityType activity)override;

	void setCharacterType(std::string characterType);

	static Player& Get();
	~Player();

	static Player s_instance;

	const CharacterType* m_characterType;
};