#include "GameObject.h"
#include "IsoObject.h"
#include "GameMapCell.h"
#include "GameConstants.h"

const unsigned int GameObject::COLOR_FOW = 0x555555FF;
const unsigned int GameObject::COLOR_VIS = 0xFFFFFFFF;

unsigned int GameObject::counter = 0;

GameObject::GameObject(GameObjectType type, int rows, int cols)
	: mIsoObj(new IsoObject(rows, cols))
	, mOnValuesChanged([]() {})
	, mObjId(++counter)
	, mType(type){
	// default colors to mark objects that haven't set any
	mObjColors.push_back(0xFFFFFFFF);
	mObjColors.push_back(0xFF00FFFF);
}

GameObject::~GameObject() { delete mIsoObj; }

void GameObject::SetSelected(bool val){

	// same value -> nothing to do
	if (val == mSelected)
		return;

	mSelected = val;	
	UpdateGraphics();	
}

void GameObject::SetVisible(bool val){
	mVisible = val;

	UpdateGraphics();
}

void GameObject::SetLinked(bool val){
	if (val == mLinked)
		return;

	mLinked = val;

	OnLinkedChanged();
}

void GameObject::SetCell(const GameMapCell * cell){
	mCell = cell;

	mIsoObj->SetRow(cell->row);
	mIsoObj->SetCol(cell->col);
}

int GameObject::GetRow0() const { return mCell->row; }
int GameObject::GetCol0() const { return mCell->col; }
int GameObject::GetRow1() const { return 1 + mCell->row - mRows; }
int GameObject::GetCol1() const { return 1 + mCell->col - mCols; }

/*void GameObject::SetOwner(Player * owner){
	// setting same owner again -> nothing to do
	if (owner == mOwner)
		return;

	mOwner = owner;

	UpdateGraphics();
}*/

void GameObject::SetHealth(float val){
	const float oldH = mHealth;

	mHealth = val;

	if (mHealth > mMaxHealth)
		mHealth = mMaxHealth;
	else if (mHealth < 0.f)
		mHealth = 0.f;

	const float minDelta = 0.01f;
	const float diff = std::fabs(mHealth - oldH);

	if (diff > minDelta)
		mOnValuesChanged();
}

inline void GameObject::SumHealth(float val){
	SetHealth(mHealth + val);
}

void GameObject::SetEnergy(float val){
	const float oldEn = mEnergy;

	mEnergy = val;

	if (mEnergy > mMaxEnergy)
		mEnergy = mMaxEnergy;
	else if (mEnergy < 0.f)
		mEnergy = 0.f;

	const float minDelta = 0.01f;
	const float diff = std::fabs(mEnergy - oldEn);

	if (diff > minDelta)
		mOnValuesChanged();
}

void GameObject::SumEnergy(float val){
	SetEnergy(mEnergy + val);
}

void GameObject::SetActiveActionToDefault() { mActiveAction = IDLE; }

void GameObject::Update(float) { }

void GameObject::OnLinkedChanged() { }

void GameObject::SetDefaultColors(){
	// clear current colors
	mObjColors.clear();

	// assign new colors based on faction
	//Player * p = GetOwner();
	//const PlayerFaction faction = p != nullptr ? p->GetFaction() : NO_FACTION;

	const PlayerFaction faction = FACTION_1;

	switch (faction)
	{
	case FACTION_1:
		mObjColors.push_back(0xd9938cff);
		mObjColors.push_back(0xcc6f66ff);
		mObjColors.push_back(0xc04a3fff);
		mObjColors.push_back(0xcc4133ff);
		mObjColors.push_back(0x9a3b32ff);
		mObjColors.push_back(0x86332cff);
		break;

	case FACTION_2:
		mObjColors.push_back(0x8cd992ff);
		mObjColors.push_back(0x66cc6eff);
		mObjColors.push_back(0x3fc04aff);
		mObjColors.push_back(0x33cc40ff);
		mObjColors.push_back(0x329a3bff);
		mObjColors.push_back(0x2c8633ff);
		break;

	case FACTION_3:
		mObjColors.push_back(0x8cccd9ff);
		mObjColors.push_back(0x66bbccff);
		mObjColors.push_back(0x3faac0ff);
		mObjColors.push_back(0x33b2ccff);
		mObjColors.push_back(0x32899aff);
		mObjColors.push_back(0x2c7786ff);
		break;

		// NO_FACTION
	default:
		mObjColors.push_back(0xccccccff);
		mObjColors.push_back(0xb2b2b2ff);
		mObjColors.push_back(0x999999ff);
		mObjColors.push_back(0x808080ff);
		mObjColors.push_back(0x666666ff);
		mObjColors.push_back(0x595959ff);
		break;
	}
}

