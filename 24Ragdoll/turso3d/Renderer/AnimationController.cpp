#include "AnimationController.h"

AnimationController::AnimationController()
{
	RegisterSubsystem(this);
}

AnimationController::~AnimationController()
{
	RemoveSubsystem(this);
}