#include "Base.h"
#include "IsoObject.h"
#include <iostream>

Base::Base(int rows, int cols) : Structure(GameObjectType::OBJ_BASE, rows, cols){
	SetVisibilityLevel(1);
}

void Base::UpdateGraphics(){
	
	SetImage();
	//SetDefaultColors();

	IsoObject * isoObj = GetIsoObject();
	isoObj->setSize(Vector2f(288.0f, 163.0f));
	isoObj->setOrigin(Vector2f(TILE_WIDTH, 0.0f));
	isoObj->m_spriteSheet = Globals::spritesheetManager.getAssetPointer("structures");
}

void Base::SetImage(){
	IsoObject * isoObj = GetIsoObject();

	/*if (IsVisible())
		isoObj->SetColor(COLOR_VIS);
	else
		isoObj->SetColor(COLOR_FOW);

	const Player * owner = GetOwner();

	// avoid to set an image when there's no owner set
	if (nullptr == owner)
		return;

	const unsigned int faction = owner->GetFaction();*/
	isoObj->m_currentFrame = static_cast<int>(IsSelected());

	//const unsigned int texInd = SpriteIdStructures::ID_STRUCT_BASE_L1_F1 + (faction * NUM_BASE_SPRITES_PER_FAC) + static_cast<int>(IsSelected());

	/*auto * tm = sgl::graphic::TextureManager::Instance();
	sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

	isoObj->SetTexture(tex);*/
}