/*
===========================================================================

Engine of Evil GPL Source Code
Copyright (C) 2016-2017 Thomas Matthew Freehill 

This file is part of the Engine of Evil GPL game engine source code. 

The Engine of Evil (EOE) Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

EOE Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with EOE Source Code.  If not, see <http://www.gnu.org/licenses/>.


If you have questions concerning this license, you may contact Thomas Freehill at tom.freehill26@gmail.com

===========================================================================
*/
#ifndef EVIL_ANIMATION_STATE_H
#define EVIL_ANIMATION_STATE_H

#include <memory>
#include "StateNode.h"

class eAnimationController;
class Animation;
//******************************
//		eAnimationState
// used by eAnimationController
// as part of its state machine
// only controls one animation
// see also: eBlendState
//*******************************
class eAnimationState : public eStateNode {
public:

	friend class eAnimationController;		// sole access to Update

public:

	eAnimationState(const std::string & name, const std::shared_ptr<Animation> & animation,  float speed = 1.0f,
		eAnimationController* stateMachine = nullptr);

	Tag GetTag() const override { return Tag::ANIMATIONSTATE; }

private:

	virtual void							Update() override;

private:

	std::shared_ptr<Animation>				animation;				// which animation this state plays
};

#endif /* EVIL_ANIMATION_STATE_H */