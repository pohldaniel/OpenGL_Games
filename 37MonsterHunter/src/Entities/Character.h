#pragma once

#include <Entities/SpriteEntity.h>

class Character : public SpriteEntity {

public:

	Character(Cell& cell);
	virtual ~Character();

	void update(float dt) override;

	void setCharacterId(const std::string& characterId);
	void setRadius(float radius);
	const std::string& getCharacterId();
	void checkConnection(const SpriteEntity& target, float tolerance = 30.0f);

private:

	std::string m_characterId;
	float m_radius;
};