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
	bool checkConnection(const Cell& origin, const Cell& target, ViewDirection viewDirection, float radius = 100.0f, float tolerance = 30.0f);
	void changeFacingDirection(const SpriteEntity& target);
	void startMove();
	bool raycast(const Cell& target);

private:

	std::string m_characterId;
	float m_radius;
	bool m_move;
};