#pragma once

#include <vector>
#include <entt/locator/locator.hpp>
#include <glm/glm.hpp>
#include <Box2D/Box2D.h>
#include <Services/IRandom.h>

#include "Maths.h"

class Level;
class ShaderRo;
class IndexBuffer;

namespace cmpt {

	struct Primitive {
		Primitive(glm::vec4 color = glm::vec4(0), unsigned int vaID = 0, unsigned int type = 0, ShaderRo* shader = nullptr, unsigned int vertexCount = 0)
			: color(color), vaID(vaID), type(type), shader(shader), vertexCount(vertexCount) {}

		glm::vec4 color;
		unsigned int vaID;
		unsigned int vertexCount;
		unsigned int type;
		ShaderRo* shader;
	};

	struct Age {
		//Entity dies when age reaches lifespan
		Age(float lifespan)
			: age(0), lifespan(lifespan) {}

		float age;
		float lifespan;
	};

	struct Animated {
		//Keeps track of an animation's age (a.k.a. time elapsed since the beginning of the animation)
		//Goes along with an "animation-[something]" component
		Animated(float duration, bool bDestroyAfterAnimation = false)
			: age(0), duration(duration), bDestroyAfterAnimation(bDestroyAfterAnimation) {}

		float age;
		float duration;
		bool bDestroyAfterAnimation;
	};

	struct AnimationAlpha {
		//Entity is partially transparent
		//If bForward is true : entity is less and less transparent
		AnimationAlpha(bool bForward)
			: bForward(bForward) {}

		bool bForward;
	};

	struct AnimationDark {
		//Entity is darkened proportionaly to its animated.age
		//If bForward is true : entity brightens, otherwise it darkens
		AnimationDark(bool bForward)
			: bForward(bForward) {}

		bool bForward;
	};

	struct AnimationPixelsVanish {
		//Some pixels are not drawn. The number of undrawn pixels is proportional to the animated.age
		//If bForward is true : there is more and more pixels drawn, otherwise there is less and less
		AnimationPixelsVanish(bool bForward)
			: bForward(bForward) {}

		bool bForward;
	};

	struct AnimationScale {
		//Entity is scaled proportionaly to its animated.age
		//If bForward is true : entity grows, otherwise it shrinks
		AnimationScale(bool bForward)
			: bForward(bForward) {}

		bool bForward;
	};

	struct AttachedTo {
		//Entity will inherit proporties such as position and colour from the main entity
		AttachedTo(unsigned int mainEntity) : mainEntity(mainEntity) {}

		unsigned int mainEntity;
	};

	struct ConstrainedRotation {
		//Entity can only have an angle of k/nbAngles*TAU where 0 <= k < nbAngles
		ConstrainedRotation(int nbAngles) : nbAngles(nbAngles), angleStep(imaths::TAU / nbAngles), accumulatedAngle(0), angleIndex(0) {}

		int nbAngles;
		float angleStep;
		float accumulatedAngle;
		int angleIndex;
	};

	struct Direction {
		//Entity moves in the "dir" direction
		Direction(float angle) {
			dir = glm::vec2(cos(angle), sin(angle));
		}

		glm::vec2 dir;
	};

	struct EntityExplosionAssociation {
		//Allows us to know that an entity hasn't been already damaged by an explosion
		EntityExplosionAssociation(std::uint32_t entity, std::uint32_t explosion) : entity(entity), explosion(explosion) {}

		std::uint32_t entity;
		std::uint32_t explosion;
	};

	struct EntityOnTile {
		//Gives the id of the building constructed on the tile that owns this component
		EntityOnTile(unsigned int entityId) : entityId(entityId) {}

		unsigned int entityId;
	};

	struct GrowingCircle {
		//Renders the entity as a circle which radius grows over time
		//Entity must have an "age" component
		GrowingCircle(float growthSpeed, float maxRadius) : growthSpeed(growthSpeed), maxRadius(maxRadius) {}

		float growthSpeed;
		float maxRadius;
	};

	struct Health {
		Health(float maxHealth) : current(maxHealth), max(maxHealth) {}

		float current;
		float max;
	};

	struct HealthBar {
		HealthBar(glm::vec2 relativePos, Primitive background, Primitive bar) : relativePos(relativePos), background(background), bar(bar) {}

		glm::vec2 relativePos;
		Primitive background;
		Primitive bar;
	};

	struct Hitbox {
		Hitbox(float radius) : radius(radius) {
			radiusSq = radius * radius;
		}

		float radius;
		float radiusSq;
	};

	struct LookAtMouse {
		LookAtMouse(float angleOffset = 0) : angleOffset(angleOffset) {}
		float angleOffset;
	};

	struct MoveTowardsMouse {
		//Move away from its position, towards the mouse
		//Cannot go further than maxDist in the mouse direction
		MoveTowardsMouse(float maxDist) : maxDist(maxDist) {}
		float maxDist;
	};

	struct Pathfinding {
		//Move towards a node and pick a new node when it reaches its target
		Pathfinding(Level* level, int startNode) : level(level), currentTarget(startNode), previousNode(-1) {}
		int previousNode;
		int currentTarget;
		Level* level;
	};

	struct RigidBody {
		RigidBody(b2Body* body, b2FixtureDef* collider) : body(body), collider(collider) {}

		b2Body* body;
		b2FixtureDef* collider;
	};

	struct Shake {
		Shake() : offset(glm::vec2(0)) {}

		glm::vec2 offset;
	};

	struct ShootAt {

		int loadingTime;
		int timer;
		float range;

		ShootAt(int loadingTime) : loadingTime(loadingTime), timer(loadingTime - 1) {}
	};

	struct ShootLaser {
		ShootLaser(bool isActiv = false) : isActiv(isActiv) {
			IRandom& random = entt::ServiceLocator<IRandom>::ref();
			col = glm::vec3(122, 249 + random.random(-150, 0), 237);
		}
		bool isActiv;
		glm::vec3 col;
	};

	struct Sprite {
		Sprite(unsigned int textureID = 0, unsigned int vaID = 0, unsigned int target = 0, ShaderRo* shader = nullptr, IndexBuffer* ib = nullptr)
			: textureID(textureID), vaID(vaID), target(target), shader(shader), ib(ib) {}

		unsigned int textureID;
		unsigned int vaID;
		unsigned int target;
		ShaderRo* shader;
		IndexBuffer* ib;
	};

	struct SpriteAnimation {
		SpriteAnimation(unsigned int startTile, unsigned int endTile, float duration)
			: startTile(startTile), endTile(endTile), activeTile(startTile), age(0), duration(duration) {}

		unsigned int startTile;
		unsigned int endTile;
		unsigned int activeTile;

		float age;
		float duration;
	};

	struct Targeting {
		Targeting(unsigned int targetId, float targetingRange) : targetId(targetId), targetingRange(targetingRange) {}

		unsigned int targetId;
		float targetingRange;
	};

	struct TintColour {
		TintColour(glm::vec4 col, bool bOneTimeOnly = false) : col(col), bOneTimeOnly(bOneTimeOnly) {}

		glm::vec4 col;
		bool bOneTimeOnly;
	};

	struct Trajectory {
		Trajectory(std::vector<glm::vec2> traj) : traj(traj), currentTarget(1) {}
		std::vector<glm::vec2> traj;
		int currentTarget;
	};

	struct Transform {
		Transform(glm::vec2 position = glm::vec2(0.0f), int zIndex = 0, glm::vec2 scale = glm::vec2(1.0f), float rotation = 0.0f)
			: position(position), zIndex(zIndex), scale(scale), rotation(rotation) {}

		glm::vec2 position;
		glm::vec2 scale;
		float rotation;
		int zIndex;
	};

	struct Velocity {
		Velocity(float velocity)
			: velocity(velocity), velMultiplier(1), multiplierAge(0), multiplierLifespan(1) {}

		float velocity;
		float velMultiplier;
		float multiplierAge;
		float multiplierLifespan;
	};

	struct Wiggle {
		Wiggle(float amplitude) : amplitude(amplitude) {
			IRandom& randomService = entt::ServiceLocator<IRandom>::ref();
			noiseOffset = randomService.random(500);
		}

		float amplitude;
		float noiseOffset;
	};
}