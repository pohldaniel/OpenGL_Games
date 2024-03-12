#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include "AnimationManager.h"

AnimationManager AnimationManager::s_instance;
std::vector<Utils::Animation> AnimationManager::animations;

Enums::anim_attack_type convertAttackType(const std::string& str){
	if (str == "neutral") return Enums::neutral;
	else if (str == "normalattack") return Enums::normalattack;
	else if (str == "reversed") return Enums::reversed;
	else if (str == "reversal") return Enums::reversal;

	assert(!"The default case of convertAttackType was reached.");
	return Enums::attack_type_count;
}

Enums::anim_height_type convertHeightType(const std::string& str) {
	if (str == "lowheight") return Enums::lowheight;
	else if (str == "middleheight") return Enums::middleheight;
	else if (str == "highheight") return Enums::highheight;

	assert(!"The default case of convertHeightType was reached.");
	return Enums::height_type_count;
}

AnimationManager::AnimationManager() : m_loadAnimations(true)  {

}

void AnimationManager::loadAll(std::string path) {
	if (!m_loadAnimations)
		return;

	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << path << std::endl;
	}
	Utils::SolidIO solidConverter;
	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);
	for (rapidjson::Value::ConstValueIterator animation = doc["animations"].Begin(); animation != doc["animations"].End(); ++animation) {
		animations.resize(animations.size() + 1);
		solidConverter.loadAnimation(animation->GetObject()["file"].GetString(), convertHeightType(animation->GetObject()["height"].GetString()), convertAttackType(animation->GetObject()["attack"].GetString()), animations.back());
	}
	file.close();

	m_loadAnimations = false;
}  

Utils::Animation& AnimationManager::getAnimation(Enums::animation_type type) {
	return animations[type];
}

AnimationManager& AnimationManager::Get() {
	return s_instance;
}

