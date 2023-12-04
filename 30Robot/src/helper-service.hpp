#pragma once

#include "i-random.hpp"
#include "i-helper.hpp"
#include "Level/level.hpp"

class Game; // Forward declaration

class HelperService : public IHelper {
public:
	HelperService();

	glm::vec4 getColour(std::uint32_t entityId) override;
	float getAlpha(std::uint32_t entityId) override;
	glm::vec4 blend(glm::vec4 col1, glm::vec4 col2) override;
	glm::vec2 getPosition(std::uint32_t entityId) override;
	glm::vec2 getPositionTowerTip(std::uint32_t entityId) override;
	glm::vec2 getScale(std::uint32_t entityId) override;
	float getVelocityMultiplier(std::uint32_t entityId) override;
	glm::vec2 getScreenShake() override;
	void updateScreenShake(float deltatime) override;

	bool mouseIsOn(std::uint32_t entityId) override;

	std::uint32_t getTile(unsigned int x, unsigned int y) override;

	std::uint32_t getTileFromProjCoord(float x, float y) override;

	std::uint32_t getEntityOnTileFromProjCoord(float x, float y) override;

	void setRegistry(entt::DefaultRegistry* registry) override;
	void setEmitter(EventEmitter* emitter) override;
	void setLevel(Level* level) override;

private:
	glm::vec2 m_screenShake;
	bool m_screenShaking;
	void startScreenShake(float duration);
	float m_screenShakeTimeRemmaining;

private:
	entt::DefaultRegistry* m_registry;
	EventEmitter* m_emitter;
	Level* m_level;
};