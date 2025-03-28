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
#ifndef EVIL_STATE_TRANSITION_H
#define EVIL_STATE_TRANSITION_H

#include <string>
#include <vector>
#include <tuple>

enum class COMPARE_ENUM {
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	EQUAL,
	NOT_EQUAL
};

//**************************************
//		eStateTransition
// used by eAnimationController to control
// the flow between its various eStateNodes
//***************************************
class eStateTransition {
public:

	// the only classes with access to transition values
	friend class eAnimationControllerManager;
	friend class eAnimationController;

public:

									eStateTransition(const std::string & name, 
													 bool anyState, 
													 int fromStateIndex, 
													 int toStateIndex,
													 float exitTime = 0.0f, 
													 float offset = 0.0f);

	
private:

	void							AddFloatCondition(int controllerFloatIndex, COMPARE_ENUM compare, float value);
	void							AddIntCondition(int controllerIntIndex, COMPARE_ENUM compare, int value);
	void							AddBoolCondition(int controllerBoolIndex, bool value);
	void							AddTriggerCondition(int controllerTriggerIndex);

private:

	std::string						name;
	int								nameHash;
	float							exitTime;			// currentState::normalizedTime to start checking conditions (if <= 0.0f, there MUST be at least one transition param)
	float							offset;				// the normalizedTime to start playing at in toState
	bool							anyState;			// all conditions checked regardless of eAnimationController::currentState (DEBUG: ie: ignores fromState)
	int								fromState;			// index within eAnimationController::animationStates this is attached to  (DEBUG: ignored if anyState == true)
	int								toState;			//   "     "           "                   "          this modifies eAnimationController::currentState to

	std::vector<std::tuple<int, COMPARE_ENUM, float>>	floatConditions;
	std::vector<std::tuple<int, COMPARE_ENUM, int>>		intConditions;		
	std::vector<std::pair<int, bool>>					boolConditions;		// values can be true or false
	std::vector<std::pair<int, bool>>					triggerConditions;  // all values are true
};

//*******************
// eStateTransition::eStateTransition
//*******************
inline eStateTransition::eStateTransition(const std::string & name, bool anyState, int fromStateIndex, int toStateIndex, float exitTime, float offset) 
	: name(name),
	  anyState(anyState),
	  fromState(fromStateIndex),
	  toState(toStateIndex),
	  exitTime(exitTime),
	  offset(offset) {
	nameHash = std::hash<std::string>()(name);
}

//*******************
// eStateTransition::AddFloatCondition
// takes the index within eAnimationController::floatParameters to compare
// to param value according to param compare
//*******************
inline void eStateTransition::AddFloatCondition(int controllerFloatIndex, COMPARE_ENUM compare, float value) {
	floatConditions.emplace_back(std::make_tuple(controllerFloatIndex, compare, value));
}

//*******************
// eStateTransition::AddIntCondition
// takes the index within eAnimationController::intParameters to compare
// to param value according to param compare
//*******************
inline void eStateTransition::AddIntCondition(int controllerIntIndex, COMPARE_ENUM compare, int value) {
	intConditions.emplace_back(std::make_tuple(controllerIntIndex, compare, value));
}

//*******************
// eStateTransition::AddBoolCondition
// takes the index within eAnimationController::boolParameters to compare
// to param value according to a "==" operation
//*******************
inline void eStateTransition::AddBoolCondition(int controllerBoolIndex, bool value) {
	boolConditions.emplace_back(std::make_pair(controllerBoolIndex, value));
}

//*******************
// eStateTransition::AddTriggerCondition
// takes the index within eAnimationController::triggerParameters to compare
// to "true" according to a "==" operation
//*******************
inline void eStateTransition::AddTriggerCondition(int controllerTriggerIndex) {
	triggerConditions.emplace_back(std::make_pair(controllerTriggerIndex, true));
}

#endif  /* EVIL_STATE_TRANSITION_H */