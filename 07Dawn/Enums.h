#pragma once
namespace Enums {
	enum TileClassificationType {
		FLOOR,
		ENVIRONMENT,
		SHADOW,
		COLLISION,
		NPC,
		COUNT
	};

	enum AdjacencyType {
		RIGHT,
		LEFT,
		TOP,
		BOTTOM
	};

	enum Direction {
		STOP = 0,
		N = 1,
		NE = 2,
		E = 3,
		SE = 4,
		S = 5,
		SW = 6,
		W = 7,
		NW = 8
	};

	enum ActivityType {
		Walking,
		Casting,
		Attacking,
		Shooting,
		Dying,
		Count
	};

	enum Attitude {
		FRIENDLY,
		NEUTRAL,
		HOSTILE
	};

	enum CharacterClass {
		NOCLASS,
		ANYCLASS,
		Liche,
		Ranger,
		Warrior
	};

	enum InteractionType {
		Quest,
		Shop,
		Zone
	};
}
