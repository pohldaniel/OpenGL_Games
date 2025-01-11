#include "Navigable.h"

//extern const char* NAVIGATION_CATEGORY;

Navigable::Navigable(SceneNodeLC* node) : m_node(node), recursive_(true){

}

Navigable::~Navigable(){

}

/*void Navigable::RegisterObject(Context* context)
{
	context->RegisterFactory<Navigable>(NAVIGATION_CATEGORY);

	URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
	URHO3D_ATTRIBUTE("Recursive", bool, recursive_, true, AM_DEFAULT);
}*/

void Navigable::SetRecursive(bool enable){
	recursive_ = enable;
}