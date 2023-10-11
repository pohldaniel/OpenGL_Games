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
#include "AnimationController.h"
#include "AnimationState.h"
#include "BlendState.h"
#include "Sort.h"
#include <iostream>

//************
// eAnimationController::eAnimationController
// copy ctor needs to allocate new state unique_ptrs
// because animationStates is a std::vector<std::unique_ptr>
//************
eAnimationController::eAnimationController(const eAnimationController & other)
	: transitionsHash(other.transitionsHash),
	statesHash(other.statesHash),
	stateTransitions(other.stateTransitions),
	floatParamsHash(other.floatParamsHash),
	intParamsHash(other.intParamsHash),
	boolParamsHash(other.boolParamsHash),
	triggerParamsHash(other.triggerParamsHash),
	floatParameters(other.floatParameters),
	intParameters(other.intParameters),
	boolParameters(other.boolParameters),
	triggerParameters(other.triggerParameters),
	currentState(other.currentState),
	paused(other.paused) {
	for (auto & state : other.animationStates) {
		if (state->GetTag() == ANIMATIONSTATE) {
			animationStates.emplace_back(std::make_unique<eAnimationState>(*static_cast<eAnimationState *>(state.get())));
		}
		else if (state->GetTag() == BLENDSTATE) {
			animationStates.emplace_back(std::make_unique<eBlendState>(*static_cast<eBlendState *>(state.get())));
		}
	}
}

template<class type>
bool CompareUtility(const type & lhs, COMPARE_ENUM compare, const type & rhs) {
	switch (compare) {
	case COMPARE_ENUM::LESS:			return lhs <  rhs;
	case COMPARE_ENUM::LESS_EQUAL:		return lhs <= rhs;
	case COMPARE_ENUM::GREATER:			return lhs >  rhs;
	case COMPARE_ENUM::GREATER_EQUAL:	return lhs >= rhs;
	case COMPARE_ENUM::EQUAL:			return lhs == rhs;
	case COMPARE_ENUM::NOT_EQUAL:		return lhs != rhs;
	default:							return false;		// invalid comparator
	}
}

//************
// eAnimationController::CheckTransitionConditions
// all conditions must be met for the transition to trigger
//************
bool eAnimationController::CheckTransitionConditions(const eStateTransition & transition) {
	if (animationStates[currentState]->GetNormalizedTime() < transition.exitTime)
		return false;
	
	bool updateState = true;
	for (auto & conditionTuple : transition.floatConditions) {
		updateState = CompareUtility<float>(floatParameters[std::get<0>(conditionTuple)],std::get<COMPARE_ENUM>(conditionTuple),std::get<2>(conditionTuple));

		if (!updateState)
			return false;
	}
	
	for (auto & conditionTuple : transition.intConditions) {
		updateState = CompareUtility<int>(intParameters[std::get<0>(conditionTuple)],
			std::get<COMPARE_ENUM>(conditionTuple),
			std::get<2>(conditionTuple)
			);
		if (!updateState)
			return false;
	}

	for (auto & conditionPair : transition.boolConditions) {
		updateState = (boolParameters[conditionPair.first] == conditionPair.second);
	}

	static std::vector<size_t> resetTriggers;					// static to reduce dynamic allocations
	resetTriggers.clear();										// lazy clearing
	for (size_t i = 0; i < transition.triggerConditions.size(); ++i) {
		const auto & conditionPair = transition.triggerConditions[i];
		updateState = (triggerParameters[conditionPair.first] == conditionPair.second);
		if (updateState)
			resetTriggers.emplace_back(i);
		else
			return false;
	}

	currentState = transition.toState;
	animationStates[currentState]->SetNormalizedTime(transition.offset);
	for (auto & triggerIndex : resetTriggers)					// reset only consumed triggers
		triggerParameters[triggerIndex] = false;

	return true;
}

//************
// eAnimationController::Update
// updates the current state of animation
// must be unpaused to fully animate
//************
void eAnimationController::Update() {
	if (paused)
		return;

	for (auto & transition : stateTransitions) {
		if (!transition.anyState || CheckTransitionConditions(transition))
			break;
	}

	// DEBUG: currentState transition checks are still allowed even if an anyState transition has triggered
	// FIXME(performance): the same transition will get checked twice if its also an anyState and a transition hasn't occured yet
	const int hashkey = animationStates[currentState]->nameHash;
	for (int i = transitionsHash.First(hashkey); i != -1; i = transitionsHash.Next(i)) {
		if ((stateTransitions[i].fromState == currentState) &&
			CheckTransitionConditions(stateTransitions[i])) {
			break;
		}
	}
	animationStates[currentState]->Update();
}

//**************
// eAnimationController::InitHashIndexes
// minimizes memory footprint, and hash collisions, and number of dynamic allocation calls
//**************
void eAnimationController::Init(int numStates, int numTransitions, int numInts, int numFloats, int numBools, int numTriggers) {
	statesHash.ClearAndResize(numStates);
	transitionsHash.ClearAndResize(numTransitions);
	intParamsHash.ClearAndResize(numInts);
	floatParamsHash.ClearAndResize(numFloats);
	boolParamsHash.ClearAndResize(numBools);
	triggerParamsHash.ClearAndResize(numTriggers);
	animationStates.reserve(numStates);
	stateTransitions.reserve(numTransitions);
	intParameters.reserve(numInts);
	floatParameters.reserve(numFloats);
	boolParameters.reserve(numBools);
	triggerParameters.reserve(numTriggers);
}

//***********************
// eAnimationController::AddAnimationState
// does not modify any states if newState::name already exists, and returns false
// otherwise adds the new state to *this and returns true
//***********************
bool eAnimationController::AddAnimationState(std::unique_ptr<eStateNode> && newState) {
	if (statesHash.First(newState->nameHash) > -1)
		return false;

	statesHash.Add(newState->nameHash, animationStates.size());
	animationStates.emplace_back(std::move(newState));
	return true;
}

//***********************
// eAnimationController::AddTransition
// DEBUG: always adds the transition, even if it has the same fromState, or fromState::nameHash
//***********************
void eAnimationController::AddTransition(eStateTransition && newTransition) {
	stateTransitions.emplace_back(std::move(newTransition));
}

//***********************
// eAnimationController::SortAndHashTransitions
// transitions that occur from anyState are arranged so their conditions are checked first
//***********************
void eAnimationController::SortAndHashTransitions() {
	QuickSort(stateTransitions.data(),
		stateTransitions.size(),
		[](auto && a, auto && b) {
		if (a.anyState && !b.anyState) return -1;
		else if (!a.anyState && b.anyState) return 1;
		return 0;
	});

	for (size_t i = 0; i < stateTransitions.size(); ++i) {
		const int hashKey = animationStates[stateTransitions[i].fromState]->nameHash;
		transitionsHash.Add(hashKey, i);
	}
}

//***********************
// eAnimationController::AddFloatParameter
// does not modify any parameters if name already exists, and returns false
// otherwise constructs the new parameter in-place and returns true
//***********************
bool eAnimationController::AddFloatParameter(const std::string & name, float initialValue) {
	const int hashKey = floatParamsHash.GetHashKey(name);
	if (floatParamsHash.First(hashKey) > -1)
		return false;

	floatParamsHash.Add(hashKey, floatParameters.size());
	floatParameters.emplace_back(initialValue);
	return true;
}

//***********************
// eAnimationController::AddIntParameter
// does not modify any parameters if name already exists, and returns false
// otherwise constructs the new parameter in-place and returns true
//*********************** 
bool eAnimationController::AddIntParameter(const std::string & name, int initialValue) {
	const int hashKey = intParamsHash.GetHashKey(name);
	if (intParamsHash.First(hashKey) > -1)
		return false;

	intParamsHash.Add(hashKey, intParameters.size());
	intParameters.emplace_back(initialValue);
	return true;
}

//***********************
// eAnimationController::AddBoolarameter
// does not modify any parameters if name already exists, and returns false
// otherwise constructs the new parameter in-place and returns true
//***********************
bool eAnimationController::AddBoolParameter(const std::string & name, bool initialValue) {
	const int hashKey = boolParamsHash.GetHashKey(name);
	if (boolParamsHash.First(hashKey) > -1)
		return false;

	boolParamsHash.Add(hashKey, boolParameters.size());
	boolParameters.emplace_back(initialValue);
	return true;
}

//***********************
// eAnimationController::AddTriggerParameter
// does not modify any parameters if name already exists, and returns false
// otherwise constructs the new parameter in-place and returns true
//***********************
bool eAnimationController::AddTriggerParameter(const std::string & name, bool initialValue) {
	const int hashKey = triggerParamsHash.GetHashKey(name);
	if (triggerParamsHash.First(hashKey) > -1)
		return false;

	triggerParamsHash.Add(hashKey, triggerParameters.size());
	triggerParameters.emplace_back(initialValue);
	return true;
}

void eAnimationController::load(const std::string & name) {
	std::ifstream read(name);

	char buffer[128];
	memset(buffer, 0, sizeof(buffer));
	read.getline(buffer, sizeof(buffer), '\n');

	memset(buffer, 0, sizeof(buffer));
	read.getline(buffer, sizeof(buffer), '\n');

	memset(buffer, 0, sizeof(buffer));
	read.getline(buffer, sizeof(buffer), '\n');							// initial state name

	int numStateNodes = 0;												// controller configuration
	int numTransitions = 0;
	int numIntParams = 0;
	int numFloatParams = 0;
	int numBoolParams = 0;
	int numTriggerParams = 0;
	read >> numStateNodes >> numTransitions >> numIntParams >> numFloatParams >> numBoolParams >> numTriggerParams;

	statesHash.ClearAndResize(numStateNodes);
	transitionsHash.ClearAndResize(numTransitions);
	intParamsHash.ClearAndResize(numIntParams);
	floatParamsHash.ClearAndResize(numFloatParams);
	boolParamsHash.ClearAndResize(numBoolParams);
	triggerParamsHash.ClearAndResize(numTriggerParams);
	animationStates.reserve(numStateNodes);
	stateTransitions.reserve(numTransitions);
	intParameters.reserve(numIntParams);
	floatParameters.reserve(numFloatParams);
	boolParameters.reserve(numBoolParams);
	triggerParameters.reserve(numTriggerParams);

	enum class LoadState {
		CONTROLLER_PARAMETERS,
		ANIMATION_STATES,
		BLEND_STATES,
		STATE_TRANSITIONS,
		FINISHED
	};
	LoadState loadState = LoadState::CONTROLLER_PARAMETERS;
	int defaultFloatNameHash = 0;
	bool firstFloatNameHashSaved = false;

	// DEBUG: always put a major-section's closing brace '}' on a new line below the last entry
	read.ignore(std::numeric_limits<std::streamsize>::max(), '{');		// jump to Controller_Parameters {\n
	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	while (read.peek() != '}') {
		memset(buffer, 0, sizeof(buffer));
		read.getline(buffer, sizeof(buffer), ' ');						// parameter type
		std::string parameterType(buffer);
		
		memset(buffer, 0, sizeof(buffer));
		read.getline(buffer, sizeof(buffer), ' ');						// parameter name
		std::string parameterName(buffer);
		
		// TODO: log an error if the same parameter attempts to load twice (based on its nameHash)
		if (parameterType == "int") {									// parameter initial value
			int initialIntValue = 0;
			read >> initialIntValue;
			AddIntParameter(parameterName, initialIntValue);
		}
		else if (parameterType == "float") {
			float initialFloatValue = 0.0f;
			read >> initialFloatValue;
			AddFloatParameter(parameterName, initialFloatValue);
			if (!firstFloatNameHashSaved) {								// saved first hashkey to use for blendState default parameters, if needed
				firstFloatNameHashSaved = true;
				defaultFloatNameHash = floatParamsHash.GetHashKey(parameterName);
			}
		}
		else if (parameterType == "bool") {
			bool initialBoolValue = false;
			read >> initialBoolValue;
			AddBoolParameter(parameterName, initialBoolValue);
		}
		else if (parameterType == "trigger") {
			bool initialTriggerValue = false;
			read >> initialTriggerValue;
			AddTriggerParameter(parameterName, initialTriggerValue);
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		
	}

	loadState = LoadState::ANIMATION_STATES;

	while (!read.eof() && loadState != LoadState::FINISHED) {
		read.ignore(std::numeric_limits<std::streamsize>::max(), '{');
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		switch (loadState) {
		case LoadState::ANIMATION_STATES: {
			while (read.peek() != '}') {
				/*memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// state name
				std::string stateName(buffer);
				
				memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// animation name

				auto & animation = game->GetAnimationManager().GetByFilename(buffer);
				
				float stateSpeed = 0.0f;
				read >> stateSpeed;										// state speed
				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				AddAnimationState(std::make_unique<eAnimationState>(stateName, animation, stateSpeed));*/
			}
			loadState = LoadState::BLEND_STATES;
			break;
		}

		case LoadState::BLEND_STATES: {
			while (read.peek() != '}') {								// adding blend states
				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip past implicit "{\n"

				memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// state name
				std::string stateName(buffer);
				
				int numAnimations = 0;									// state configuration
				int animationBlendMode = 0;
				float stateSpeed = 0.0f;
				read >> numAnimations >> animationBlendMode >> stateSpeed;
				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				
				AnimationBlendMode blendMode;
				switch (animationBlendMode) {
				case 1: blendMode = AnimationBlendMode::SIMPLE_1D; break;
				case 2: blendMode = AnimationBlendMode::FREEFORM_2D; break;
				default: blendMode = AnimationBlendMode::SIMPLE_1D; break;
				}

				memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// x-axis blending parameter name
				std::string xBlendParameterName(buffer);


				// DEBUG: .ectrl format demands that if one blend state exists, then at least one float param exists
				int xBlendParameterHash = floatParamsHash.GetHashKey(xBlendParameterName);
				int xBlendParameterIndex = GetFloatParameterIndex(xBlendParameterName);
				if (xBlendParameterIndex < 0)							// invalid parameter name, use default
					xBlendParameterHash = defaultFloatNameHash;


				int yBlendParameterHash = defaultFloatNameHash;
				if (blendMode == AnimationBlendMode::FREEFORM_2D) {
					memset(buffer, 0, sizeof(buffer));
					read.getline(buffer, sizeof(buffer), '\n');			// y-axis blending parameter name
					std::string yBlendParameterName(buffer);					

					// DEBUG: .ectrl format demands if blendMode == FREEFORM_2D that two parameters be listed
					int yBlendParameterIndex = GetFloatParameterIndex(yBlendParameterName);
					yBlendParameterHash = floatParamsHash.GetHashKey(yBlendParameterName);
					if (yBlendParameterIndex < 0)
						yBlendParameterHash = defaultFloatNameHash;
				}


				// TODO: if no floats values are listed (just an animationName\n) then default values to 
				// (1.0f / numAnimations) * currentAnimationLoadedCount so they are evenly distributed
				// OR: add a "distribute" boolean at the top of the blend state file-definition
				// to indicate how to affect/ignore the values in the file
				auto & newBlendState = std::make_unique<eBlendState>(stateName, numAnimations, xBlendParameterHash, yBlendParameterHash, blendMode, stateSpeed, this);
				while (read.peek() != '}') {							// adding blend nodes
					read.getline(buffer, sizeof(buffer), ' ');			// animation name
					std::string animationName(buffer);

					float nodeValue_X = 0.0f;
					float nodeValue_Y = 0.0f;
					read >> nodeValue_X;
					if (blendMode == AnimationBlendMode::FREEFORM_2D)
						read >> nodeValue_Y;

					read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					if (!newBlendState->AddBlendNode(animationName, nodeValue_X, nodeValue_Y))
						return;									// bad animation name	// TODO: just log an error and let the error_animation play
				}

				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	// skip past blend state delimiter "}\n"
				newBlendState->Init();
				// FIXME(?): blendNodesHash ClearAndResize to numAnimations may cause too many collisions
				AddAnimationState(std::move(newBlendState));
			}
			loadState = LoadState::STATE_TRANSITIONS;
			break;
		}

		case LoadState::STATE_TRANSITIONS: {
			while (read.peek() != '}') {								// adding blend states
				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip past implicit "{\n"

				memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// transition name
				std::string transitionName(buffer);
				
				memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// from state
				std::string fromState(buffer);
				
				// fromState is allow to be invalid for anyState == true
				int fromStateIndex = GetStateIndex(fromState);

				memset(buffer, 0, sizeof(buffer));
				read.getline(buffer, sizeof(buffer), ' ');				// to state
				std::string toState(buffer);				

				// skip this transition if its toState is invalid
				int toStateIndex = GetStateIndex(toState);
				if (toStateIndex < 0) {
					read.ignore(std::numeric_limits<std::streamsize>::max(), '}');
					read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				float exitTime = 0.0f;									// when to check conditions (in normalizedTime)
				float offset = 0.0f;									// where to start toState (in normalizedTime)
				bool anyState = false;									// transition occurs from any eStateNode
				read >> anyState >> exitTime >> offset;
				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

				// skip this transition if its fromState is invalid
				if (!anyState && fromStateIndex < 0) {
					read.ignore(std::numeric_limits<std::streamsize>::max(), '}');
					read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				if (offset < 0.0f)
					offset = 0.0f;

				eStateTransition newTransition(transitionName, anyState, fromStateIndex, toStateIndex, exitTime, offset);
				while (read.peek() != '}') {							// adding transition conditions
					read.getline(buffer, sizeof(buffer), ' ');			// condition type (from named controller param's type)
					std::string conditionType(buffer);
	
					read.getline(buffer, sizeof(buffer), ' ');			// controller parameter name
					std::string controllerParameterName(buffer);
	
					read.getline(buffer, sizeof(buffer), ' ');			// comparision operator name
					std::string compareOperatorName(buffer);

					// DEBUG: compareEnum will be ignored for bools and triggers because they're always COMPARE_ENUM::EQUAL
					COMPARE_ENUM compareEnum;
					if (compareOperatorName == "greater")			compareEnum = COMPARE_ENUM::GREATER;
					else if (compareOperatorName == "greaterEqual") compareEnum = COMPARE_ENUM::GREATER_EQUAL;
					else if (compareOperatorName == "less")			compareEnum = COMPARE_ENUM::LESS;
					else if (compareOperatorName == "lessEqual")	compareEnum = COMPARE_ENUM::LESS_EQUAL;
					else if (compareOperatorName == "equal")		compareEnum = COMPARE_ENUM::EQUAL;
					else if (compareOperatorName == "notEqual")		compareEnum = COMPARE_ENUM::NOT_EQUAL;

					if (conditionType == "int") {						// condition value, and validating parameter names

						const int controllerIntIndex = GetIntParameterIndex(controllerParameterName);
						if (controllerIntIndex >= 0) {
							int intValue = 0;
							read >> intValue;
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

							newTransition.AddIntCondition(controllerIntIndex, compareEnum, intValue);
						}
						else {
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						}

					}
					else if (conditionType == "float") {

						const int controllerFloatIndex = GetFloatParameterIndex(controllerParameterName);
						if (controllerFloatIndex >= 0) {
							float floatValue = 0.0f;
							read >> floatValue;
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		
							newTransition.AddFloatCondition(controllerFloatIndex, compareEnum, floatValue);
						}
						else {
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						}

					}
					else if (conditionType == "bool") {

						const int controllerBoolIndex = GetBoolParameterIndex(controllerParameterName);
						if (controllerBoolIndex >= 0) {
							bool boolValue = false;
							read >> boolValue;
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						

							newTransition.AddBoolCondition(controllerBoolIndex, boolValue);
						}
						else {
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
						}

					}
					else if (conditionType == "trigger") {

						const int controllerTriggerIndex = GetBoolParameterIndex(controllerParameterName);
						if (controllerTriggerIndex >= 0)
							newTransition.AddTriggerCondition(controllerTriggerIndex);
						else
							read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					}
				}

				read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	// skip past blend state delimiter "}\n"
				AddTransition(std::move(newTransition));
			}
			SortAndHashTransitions();
			loadState = LoadState::FINISHED;
			break;
		}
		}
	}

	read.close();
}