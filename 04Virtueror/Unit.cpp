#include "Unit.h"
#include "ObjectData.h"
#include "Structure.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "IsoObject.h"

const char * Unit::TITLES[NUM_UNIT_TYPES] = {
	"WORKER",
	"SOLDIER",
	"GENERIC",
	"SCOUT"
};

const char * Unit::DESCRIPTIONS[NUM_UNIT_TYPES] = {
	"A basic worker unit.\nIt is specialized in construction and conquest.",
	"A basic soldier unit.\nUseful for defense and exploration.",
	"A slow, but versatile unit.",
	"A light and fast unit ideal for exploring, but not for fighting."
};

const float ACTION_COSTS[NUM_OBJ_ACTIONS] ={
	0.f,    // IDLE
	0.f,    // BUILD_UNIT
	2.f,    // MOVE
	5.f,    // CONQUER_CELL
	10.f,   // CONQUER_STRUCTURE
	1.f,    // ATTACK
	20.f,   // BUILD_STRUCTURE
	10.f,   // BUILD_WALL
	0.f     // TOGGLE_GATE
};

Unit::Unit(const ObjectData & data, int rows, int cols): GameObject(GameObjectType::OBJ_UNIT, rows, cols), mUnitType(static_cast<UnitType>(data.objType)), mStructToBuild(STRUCT_NULL){
	// SET STATS values in range [1-10]
	mStats.resize(NUM_UNIT_STATS);
	//mStats = data.stats;

	// set attack range converting attribute
	const int maxAttVal = 11;
	const int attRanges[maxAttVal] = { 0, 2, 3, 4, 5, 6, 8, 9, 10, 11, 13 };
	mAttackRange = attRanges[2];

	// TODO translate stats into actual values, ex.: speed = 5 -> SetSpeed(2.f)

	// SET CONCRETE ATTRIBUTES
	const float maxStatVal = 10.f;

	// set actual speed
	const float maxSpeed = 10.f;
	//const float speed = maxSpeed * static_cast<float>(mStats[OSTAT_SPEED]) / maxStatVal;
	SetSpeed(maxSpeed);

	SetVisibilityLevel(1);

	IsoObject * isoObj = GetIsoObject();
	isoObj->setSize(Vector2f(96.0f, 58.0f));
	isoObj->m_spriteSheet = Globals::spritesheetManager.getAssetPointer("units");

}

void Unit::IncreaseUnitLevel(){
	if (mLevel >= MAX_UNITS_LEVEL)
		return;

	++mLevel;
	SetImage();
}

bool Unit::SetAttackTarget(GameObject * obj){

	if (nullptr == obj || !IsTargetInRange(obj) || !obj->IsVisible() || obj == this)
		return false;

	mTarget = obj;

	return true;
}

void Unit::SetActiveActionToDefault() { SetActiveAction(MOVE); }

void Unit::Update(float delta)
{
	// UPDATE ENERGY
	// TODO recover energy based on attributes
	if (GetEnergy() < GetMaxEnergy())
		SumEnergy(0.1f);

	// ATTACKING OTHER OBJECTS
	if (mTarget)
	{
		mTimerAttack -= delta;

		// time to shoot!
		if (mTimerAttack < 0.f)
		{
			// target still alive -> shoot
			if (GetGameMap()->HasObject(mTarget))
				Shoot();
			// target destroyed -> clear pointer
			else{
				mTarget = nullptr;

				// mark attack action as completed
				//GetScreen()->SetObjectActionCompleted(this);
				SetCurrentAction(GameObjectActionId::IDLE);
			}

			mTimerAttack = mTimeAttack;
		}
	}
}

void Unit::ClearStructureToBuild() { 
	mStructToBuild = STRUCT_NULL; 
}

void Unit::ConsumeEnergy(GameObjectActionId action){
	if (action < NUM_OBJ_ACTIONS)
		SumEnergy(-ACTION_COSTS[action]);
}

bool Unit::HasEnergyForAction(GameObjectActionId action){

	if (action < NUM_OBJ_ACTIONS){
		const float diff = GetEnergy() - ACTION_COSTS[action];
		return diff >= 0.f;
	}else
		return false;
}

void Unit::UpdateGraphics(){
	SetImage();

	SetDefaultColors();
}

bool Unit::IsTargetInRange(GameObject * obj) const{
	for (int r = obj->GetRow1(); r <= obj->GetRow0(); ++r){
		for (int c = obj->GetCol1(); c <= obj->GetCol0(); ++c){
			if (std::abs(GetRow0() - r) <= mAttackRange && std::abs(GetCol0() - c) <= mAttackRange)
				return true;
		}
	}

	return false;
}

void Unit::SetImage(){
	IsoObject * isoObj = GetIsoObject();
	isoObj->m_currentFrame = 0;
}

void Unit::Shoot() {

}