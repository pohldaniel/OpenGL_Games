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
#include "Movement.h"
#include "Entity.h"
#include "Map.h"
#include "Renderer.h"

//***************
// eMovementPlanner::eMovementPlanner
//***************
eMovementPlanner::eMovementPlanner(Entity& _entity, float movementSpeed) : entity(_entity), maxMoveSpeed(movementSpeed), goalRange(movementSpeed){

	pathingState = PATHTYPE_COMPASS;
	moveState = MOVETYPE_GOAL;
	knownMap.SetCellSize(32, 32);
	knownMap.ResetAllCells();

	currentTile = &knownMap.Index(entity.m_position);
	previousTile = currentTile;
	StopMoving();
}

//******************
// eMovementPlanner::StopMoving
//******************
void eMovementPlanner::StopMoving() {
	wallSide = nullptr;
	entity.m_velocity.set(0.0f, 0.0f);
	moving = false;
}

//***************
// eMovementPlanner::Update
// selects and updates a pathfinding type (eg: waypoint+obstacle avoid, A* optimal path, wall follow, Area awareness, raw compass?, etc)
//***************
void eMovementPlanner::Update() {

	bool wasStopped = false;

	// only pathfind with a waypoint
	if (currentWaypoint != nullptr) {
		wasStopped = !moving;
		Move();
		UpdateKnownMap();

		// drop a trail waypoint (but never in a deadend that stopped the entity last frame)
		if (moving && !wasStopped && moveState != MOVETYPE_TRAIL && lastTrailTile != currentTile) {
			trail.PushFront(entity.m_position);
			lastTrailTile = currentTile;
		}

		// check if goal is close enough to stop
		if (entity.m_position.compare(*currentWaypoint, goalRange)) {
			StopMoving();
			UpdateWaypoint(true);
		}

		// finalize the move
		moving = !entity.m_velocity.zero();
	}
}

//***************
// eMovementPlanner::Move
// sets the velocity based on path predictions
// FIXME: name this something better
//***************
void eMovementPlanner::Move() {
	switch (pathingState) {
	case PATHTYPE_COMPASS: CompassFollow(); return;
	//case PATHTYPE_WALL: WallFollow(); return;
	}
}


//******************
// eMovementPlanner::CompassFollow
// Determines the optimal movement vector to reach the current waypoint
//******************
void eMovementPlanner::CompassFollow() {
	decision_t	waypoint;				// from the collisionModel::origin to the next waypoint
	decision_t	test;					// vector tested for optimal travel decision
	decision_t	best;					// optimal movement
	float		rotationAngle;			// cumulative amount the testVector has rotated in its search
	float		maxRotation;			// disallow vectors that backtrack if already moving
	float		weight;					// net bias for a decision about a test
	float		bestWeight;				// highest net result of all modifications to validSteps
	float		bias[4] = { 2.0f, 1.0f, 1.05f, 1.1f };	// { waypoint, left, right, forward }
	//auto &		ownerCollisionModel = owner->CollisionModel();

	if (!moving) {
		test.vector = Vector2f(0.0f, 0.0f);
		maxRotation = 360.0f;
	} else {
		CheckWalls(bias);
		test = right;		// counter-clockwise sweep of 180 degree arc from right to left in the forward direction

							//		if (moveState == MOVETYPE_GOAL)
		maxRotation = 180.0f;
		//		else // moveState == MOVETYPE_TRAIL
		//			maxRotation = 360.0f;				// FIXME/BUG: occasionally causes semi-permanent stuck-in-place jitter

		// stuck in a corner (look for the quickest and most waypoint-oriented way out)
		if ((forward.stepRatio == 0 && right.stepRatio == 0) || (forward.stepRatio == 0 && left.stepRatio == 0))
			currentTile->value = VISITED_TILE;
	}

	waypoint.vector = *currentWaypoint - entity.m_position;
	waypoint.vector.normalize();

	bestWeight = 0.0f;
	rotationAngle = 0.0f;
	while (rotationAngle < maxRotation) {
		
		// check how clear the path is starting one step along it
		// and head straight for the waypoint if the test.vector path crosses extremely near it
		if (CheckVectorPath(test)) {
			//std::cout << "-----------" << std::endl;
			if (CheckVectorPath(waypoint))
				forward = waypoint;
			else
				forward = test;

			// initilize the new left and right, and their validSteps that'll be used next frame
			CheckWalls(nullptr);
			//std::cout << "Velocity2: " << entity.m_velocity[0] << "  " << entity.m_velocity[1] << std::endl;
			entity.m_velocity = forward.vector * maxMoveSpeed;
			return;
		}

		// FIXME/BUG: trail waypoint orbits or cannot attain sometimes (bad corner, whatever)
		// SOMEWHAT fixed by putting a trail waypoint on each new tile (its never too far to navigate straight back to)
		// SOMEWHAT fixed by modulating speed based on waypoint proximity, except backtracking looked weird so that was removed
		// SOLUTION(?): count how many steps its been on the current tile, if limit exceeded, then set currentTile to VISITED_TILE
		// SOLUTION(?): modulate speed if lingering on a tile & in given range of latest trail waypoint

		// give the path a bias to help set priority
		weight = test.validSteps;
		weight += (Vector2f::Dot(test.vector,waypoint.vector)) * bias[0];
		if (moving) {
			weight += (Vector2f::Dot(test.vector,left.vector)) * bias[1];
			weight += (Vector2f::Dot(test.vector,right.vector)) * bias[2];
			weight += (Vector2f::Dot(test.vector,forward.vector)) * bias[3];
		}

		// more new tiles always beats better overall weight
		if (test.stepRatio > best.stepRatio) {
			bestWeight = weight;
			best = test;
		}
		else if (test.stepRatio == best.stepRatio && weight > bestWeight) {
			bestWeight = weight;
			best = test;
		}
		//test.vector = rotateCounterClockwiseZ * test.vector;
		rotationAngle += ROTATION_INCREMENT;
	}

	if (moveState == MOVETYPE_GOAL && best.stepRatio == 0) {	// deadlocked, begin deadend protocols (ie follow the trail now)
		StopMoving();
		moveState = MOVETYPE_TRAIL;
	} else if (moveState == MOVETYPE_TRAIL && best.stepRatio > 0) {
		StopMoving();
		moveState = MOVETYPE_GOAL;
	}else if (moveState == MOVETYPE_TRAIL && best.validSteps == 0) {
		StopMoving();
	}else {
		forward = best;
		// initilize the new left and right, and their validSteps that'll be used next frame
		CheckWalls(nullptr);
		entity.m_velocity = forward.vector * maxMoveSpeed;
	}
	// moveState may have changed, track the correct waypoint
	UpdateWaypoint();
}

//******************
// eMovementPlanner::CheckVectorPath
// determines the state of the entity's position for the next few frames
// returns true if a future position using along is near the waypoint
//******************
bool eMovementPlanner::CheckVectorPath(decision_t & along) {
	static const float maxSteps = 5.0f;										// how many future steps to test

	//auto & ownerCollisionModel = owner->CollisionModel();
	Vector2f& boundsCenter = entity.m_position;
	float castLength = maxMoveSpeed * maxSteps;
	float nearestFraction = 1.0f;
	float mapEdgeFraction = 1.0f;

	//auto & mapEdges = owner->GetMap()->EdgeColliders();
	/*for (size_t i = 0; i < mapEdges.size(); ++i) {
		float movingAway = mapEdges[i].second * along.vector;
		if (movingAway >= 0.0f)
			continue;
		//if (eCollision::MovingAABBAABBTest(ownerCollisionModel.AbsBounds(), along.vector, castLength, mapEdges[i].first, mapEdgeFraction) && mapEdgeFraction < nearestFraction)
		//	nearestFraction = mapEdgeFraction;
	}*/

	along.validSteps = 0.0f;
	//Collision_t collision;
	//if (ownerCollisionModel.FindApproachingCollision(along.vector, castLength, collision) && collision.fraction < nearestFraction) {
	if (false) {
		//nearestFraction = collision.fraction;
	}
	else {
		Vector2f endPoint = boundsCenter + along.vector * (nearestFraction * castLength);
		if (moveState == MOVETYPE_GOAL && endPoint.compare(*currentWaypoint, goalRange)) {
			along.validSteps = floor(nearestFraction * maxSteps);
			return true;
		}
	}
	along.validSteps = floor(nearestFraction * maxSteps);

	// DEBUG: eCollision::GetAreaCells using along.vector grabs more cells 
	// than eMovementPlanner will when updating knownMap, so it's not used here
	Vector2f futureCenter = boundsCenter;
	float newSteps = 0.0f;
	for (int i = 0; i < along.validSteps; ++i) {
		if (knownMap.Index(futureCenter).value == UNKNOWN_TILE)
			++newSteps;
		futureCenter += along.vector * maxMoveSpeed;
	}

	along.stepRatio = 0.0f;
	if (along.validSteps > 0.0f)
		along.stepRatio = newSteps / along.validSteps;

	return true;
}

//**************
// eMovementPlanner::CheckWalls
// assigns the vectors perpendicular to the forward vector
// and checks if the range along them has significantly changed
//**************
void eMovementPlanner::CheckWalls(float * bias) {
	static const float stepIncreaseThreshold = 2.0f;		// updated validSteps must be larger than the oldValidSteps + this to warrant a bias modification

	float oldLeftSteps = left.validSteps;
	float oldRightSteps = right.validSteps;
	left.vector.set(-forward.vector[1], forward.vector[0]);	// forward rotated 90 degrees counter-clockwise
	right.vector.set(forward.vector[1], -forward.vector[0]);	// forward rotated 90 degrees clockwise

	CheckVectorPath(forward);
	CheckVectorPath(left);
	CheckVectorPath(right);

	if (bias == nullptr)
		return;

	// bias[0] == waypoint bias remains unchanged because if * test.stepRatio; or * waypoint.stepRatio; then trail-less backtracking occurs (bad)
	bias[1] *= (left.validSteps >= oldLeftSteps + stepIncreaseThreshold) * left.stepRatio;		// left path opened
	bias[2] *= (right.validSteps >= oldRightSteps + stepIncreaseThreshold) * right.stepRatio;	// right path opened
	bias[3] *= !(forward.validSteps == 0.0f) * forward.stepRatio;								// forward path not closed
}

//******************
// eMovementPlanner::AddUserWaypoint
// TODO: allow other entities to become waypoints (that move)
// use a separate eVec2 * target = &targetOrigin; to set currentWaypoint during MOVETYPE_GOAL
//******************
void eMovementPlanner::AddUserWaypoint(const Vector2f & waypoint) {
	/*static std::vector<Collision_t> collisions;		// FIXME(~): make this a private data member instead of per-fn, if more than one fn uses it
	collisions.clear();								// DEBUG: lazy clearing

	eBounds waypointBounds = owner->CollisionModel().LocalBounds() + waypoint;
	if (!eCollision::AABBContainsAABB(owner->GetMap()->AbsBounds(), waypointBounds) ||
		eCollision::BoxCast(owner->map, collisions, waypointBounds, vec2_zero, 0.0f))
		return;*/

	goals.PushFront(waypoint);
	UpdateWaypoint();
}

//******************
// eMovementPlanner::UpdateWaypoint
//******************
void eMovementPlanner::UpdateWaypoint(bool getNext) {
	switch (moveState) {
	case MOVETYPE_GOAL: {
		if (getNext && !goals.IsEmpty()) {
			goals.PopBack();
			trail.Clear();
		}
		CheckTrail();
		if (!goals.IsEmpty()) {
			currentWaypoint = &goals.Back()->Data();
			return;
		}
		currentWaypoint = nullptr;
		return;
	}
	case MOVETYPE_TRAIL: {
		if (getNext && !trail.IsEmpty())
			trail.PopFront();
		if (!CheckTrail()) {
			currentWaypoint = &trail.Front()->Data();
			return;
		}
		else if (!goals.IsEmpty()) {
			moveState = MOVETYPE_GOAL;
			currentWaypoint = &goals.Back()->Data();
			return;
		}
		currentWaypoint = nullptr;
		return;
	}
	default: {		// DEBUG: currently for PATHTYPE_WALL, dupicate code for case MOVETYPE_GOAL
					// TODO(?): have PATHTYPE_WALL do more than add to knownMap and trail waypoints, or separate knownMap entirely into PATHTYPE_COMPASS
		if (getNext && !goals.IsEmpty()) {
			goals.PopBack();
			trail.Clear();
		}
		CheckTrail();
		if (!goals.IsEmpty()) {
			currentWaypoint = &goals.Back()->Data();
			return;
		}
		currentWaypoint = nullptr;
		return;
	}
	}
}

//******************
// eMovementPlanner::ClearTrail
// resets the knownMap so on the next UpdateKnownMap
// all trail waypoints are removed
//******************
void eMovementPlanner::ClearTrail() {
	knownMap.ResetAllCells();
	lastTrailTile = nullptr;
}

//******************
// eMovementPlanner::CheckTrail
// returns false if the entity should fresh-start goal pathfinding
//******************
bool eMovementPlanner::CheckTrail() {
	if (trail.IsEmpty()) {
		knownMap.ResetAllCells();
		lastTrailTile = nullptr;
		return true;
	}
	return false;
}

//******************
// eMovementPlanner::UpdateKnownMap
// marks the currentTile as VISITED_TILE, clears out un-needed trail waypoints,
// and resets tiles around the current goal waypoint to UNKNOWN_TILE
//******************
void eMovementPlanner::UpdateKnownMap() {
	eTileKnowledge * checkTile;
	int row, column;
	int startRow, startCol;
	int endRow, endCol;
	int tileResetRange;		// size of the box around the goal to set tiles to UNKNOWN_TILE

							// mark the tile to help future movement decisions
							// DEBUG: only needs to be more than **half-way** onto a new tile
							// to set the currentTile as previousTile and VISITED_TILE,
							// instead of completely off the tile (via a full absBounds check against the eGridCell bounds)
	checkTile = &knownMap.Index(entity.m_position);
	if (checkTile != currentTile) {
		previousTile = currentTile;
		previousTile->value = VISITED_TILE;
		currentTile = checkTile;
	}

	tileResetRange = 0;
	if (!CheckTrail()) {

		// solid-box of tiles at the tileResetRange centered on the current goal waypoint to reset the knownMap
		if (!goals.IsEmpty()) {
			//tileResetRange = (int)((goals.Back()->Data() - owner->CollisionModel().Center()).Length() / (knownMap.CellWidth() * 2));

			// find the knownMap row and column of the current goal waypoint
			knownMap.Index(goals.Back()->Data(), row, column);

			// set initial bounding box top-left and bottom-right indexes within knownMap
			startRow = row - (tileResetRange / 2);
			startCol = column - (tileResetRange / 2);
			endRow = row + (tileResetRange / 2);
			endCol = column + (tileResetRange / 2);

			// snap bounding box rows & columns within range of the tileMap area
			if (startRow < 0)
				startRow = 0;
			if (startCol < 0)
				startCol = 0;

			if (endRow >= knownMap.Rows())
				endRow = knownMap.Rows() - 1;
			if (endCol >= knownMap.Columns())
				endCol = knownMap.Columns() - 1;

			// reset tiles within the bounding box
			row = startRow;
			column = startCol;
			while (row <= endRow) {
				knownMap.Index(row, column).value = UNKNOWN_TILE;

				column++;
				if (column > endCol) {
					column = startCol;
					row++;
				}
			}
		}
	}

	// pop all trail waypoints that no longer fall on VISITED_TILEs
	while (!trail.IsEmpty()) {
		if (knownMap.Index(trail.Back()->Data()).value == UNKNOWN_TILE)
			trail.PopBack();
		else
			break;
	}
}

//******************
// eMovementPlanner::DebugDraw
//******************
void eMovementPlanner::DebugDraw() {
	DrawGoalWaypoints();
	DrawKnownMap();
	DrawTrailWaypoints();
}

//******************
// eMovementPlanner::DrawGoalWaypoints
//******************
void eMovementPlanner::DrawGoalWaypoints() {
	for (auto iterator = goals.Back(); iterator != nullptr; iterator = iterator->Next()) {
		Vector2f goalPoint = iterator->Data();
		float x = floorf(goalPoint[0] + 0.5f);
		float y = floorf(goalPoint[1] + 0.5f);

		Renderer::Get().drawIsometricRect(x, y, Vector4f(10.0f, 10.0f, 0.0f, 0.0f), Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

//******************
// eMovementPlanner::DrawTrailWaypoints
//******************
void eMovementPlanner::DrawTrailWaypoints() {
	for (auto iterator = trail.Front(); iterator != nullptr; iterator = iterator->Prev()) {
		Vector2f trailPoint = iterator->Data();
		float x = floorf(trailPoint[0] + 0.5f);
		float y = floorf(trailPoint[1] + 0.5f);

		Renderer::Get().drawIsometricRect(x, y, Vector4f(5.0f, 5.0f, 0.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
	}
}

//******************
// eMovementPlanner::DrawKnownMap
// TODO: check collision/draw layers, and draw debug rects over visited (and visible) tiles instead of entire cells.
//******************
void eMovementPlanner::DrawKnownMap() const {
	

}