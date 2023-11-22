#include "CharacterController_cs.h"

CharacterControllerCS::CharacterControllerCS(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	m_position = b2Vec2(200.0f, 600.0f);

	b2BodyDef playerDef;
	playerDef.fixedRotation = true;
	playerDef.type = b2_kinematicBody;
	playerDef.position = m_position;

	m_body = Globals::world->CreateBody(&playerDef);

	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(m_size.x * 0.5 , m_size.y * 0.5);

	b2FixtureDef playerFixture;
	playerFixture.shape = &boundingBox;
	playerFixture.friction = 0.0f;
	playerFixture.density = 1.0f;
	playerFixture.userData.pointer = 1;

	m_body->CreateFixture(&playerFixture);
	//m_body->SetUserData(this);

	m_horizontalRaySpacing = (m_size.y - m_skinWidth * 2) / (m_horizontalRayCount - 1);
	m_verticalRaySpacing = (m_size.x - m_skinWidth * 2) / (m_verticalRayCount - 1);

	m_gravity = (2 * m_jumpHeight) / (m_timeToJumpApex * m_timeToJumpApex);
	m_jumpVelocity = m_gravity * m_timeToJumpApex;

#if SHAPE_BOX

	for (int i = 0; i < m_horizontalRayCount; i++) {
		positionsLeft.push_back(b2Vec2(m_skinWidth - m_size.x * 0.5, m_skinWidth - m_size.y * 0.5) + (i * m_horizontalRaySpacing) * b2Vec2(0.0f, 1.0f));
		positionsRight.push_back(b2Vec2(m_size.x * 0.5 - m_skinWidth, m_skinWidth - m_size.y * 0.5) + (i * m_horizontalRaySpacing) * b2Vec2(0.0f, 1.0f));
	}

	for (int i = 0; i < m_verticalRayCount; i++) {
		positionsBottom.push_back(b2Vec2(m_skinWidth - m_size.x * 0.5, m_skinWidth - m_size.y * 0.5) + (i * m_verticalRaySpacing) * b2Vec2(1.0f, 0.0f));
		positionsTop.push_back(b2Vec2(m_skinWidth - m_size.x * 0.5, m_size.y * 0.5 - m_skinWidth) + (i * m_verticalRaySpacing) * b2Vec2(1.0f, 0.0f));
	}

	for (int i = 0; i < m_verticalRayCount * 0.25; i++) {
		{
			float theta = PI * float(i) / float(m_verticalRayCount * 0.25);
			float x = m_size.x * 0.5 * cosf(theta);
			float y = m_size.x * 0.5 * sinf(theta);
			positionsTop.push_back(b2Vec2(x, y));
		}

		{
			float theta = PI * (float(i) / float(m_verticalRayCount * 0.25) + 1.0f);

			float x = m_size.x * 0.5 * cosf(theta);
			float y = m_size.x * 0.5 * sinf(theta);
			positionsBottom.push_back(b2Vec2(x, y));
		}

	}

#elif SHAPE_CIRCLE
	for (int i = 0; i < m_horizontalRayCount; i++) {
		{
			float theta = PI * (float(i) / float(m_horizontalRayCount) + 0.5f);
			float x = m_size.x * 0.5 * cosf(theta);
			float y = m_size.x * 0.5 * sinf(theta);
			positionsLeft.push_back(b2Vec2(x, y));
		}

		{
			float theta = PI * float(i) / float(m_horizontalRayCount) + PI;

			float x = m_size.x * 0.5 * cosf(theta);
			float y = m_size.x * 0.5 * sinf(theta);
			positionsRight.push_back(b2Vec2(x, y));
		}
	}

	for (int i = 0; i < m_verticalRayCount; i++) {
		{
			float theta = PI * float(i) / float(m_verticalRayCount);
			float x = m_size.x * 0.5 * cosf(theta);
			float y = m_size.x * 0.5 * sinf(theta);
			positionsTop.push_back(b2Vec2(x, y));
		}

		{
			float theta = PI * (float(i) / float(m_verticalRayCount) + 1.0f);

			float x = m_size.x * 0.5 * cosf(theta);
			float y = m_size.x * 0.5 * sinf(theta);
			positionsBottom.push_back(b2Vec2(x, y));
		}
	}
#endif
}

CharacterControllerCS::~CharacterControllerCS() {

}

void CharacterControllerCS::render() {


#if DEBUGCOLLISION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&Globals::projection[0][0]);
	//if (m_callback.m_hit){
	/*m_target = m_callback.m_point;
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3f(m_postionD.x, m_postionD.y, 0.0f);
	glVertex3f(m_targetD.x, m_targetD.y, 0.0f);
	glEnd();

	}else {
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(m_postionD.x, m_postionD.y, 0.0f);
	glVertex3f(m_targetD.x, m_targetD.y, 0.0f);
	glEnd();
	}*/

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	#if SHAPE_BOX
		b2Vec2 position = m_body->GetPosition();
		b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(m_body->GetFixtureList()->GetShape());

		b2Vec2 v1 = boxShape->m_vertices[0];
		b2Vec2 v2 = boxShape->m_vertices[1];
		b2Vec2 v3 = boxShape->m_vertices[2];
		b2Vec2 v4 = boxShape->m_vertices[3];

		glBegin(GL_QUADS);
		glColor3f(1, 0, 0);

		float xpos = position.x + v1.x;
		float ypos = position.y + v1.y;
		float w = v2.x - v1.x;
		float h = v4.y - v1.y;

		glVertex3f(xpos, ypos, 0.0f);
		glVertex3f(xpos, (ypos + h), 0.0f);
		glVertex3f(xpos + w, (ypos + h), 0.0f);
		glVertex3f(xpos + w, ypos, 0.0f);
		glEnd();

	#elif SHAPE_CIRCLE
		b2Vec2 position = m_body->GetPosition();
		int segments = m_verticalRayCount;

		glBegin(GL_LINE_LOOP);
		glColor3f(1, 0, 0);
		for (int ii = 0; ii < segments; ii++){
			float theta = 2 * PI * (float(ii) / float(segments));

			float x = 15 * cosf(theta);
			float y = 15 * sinf(theta);

			glVertex2f(x + position.x, y + position.y);//output vertex
		}

		/*float theta = PI * float(segments + 0.5) / float(segments);//get the current angle

		float x = 15 * cosf(theta);//calculate the x component
		float y = 15 * sinf(theta);//calculate the y component

		glVertex2f(x + position.x, y + position.y);//output */

		glEnd();
	#endif
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#endif
}

void CharacterControllerCS::update() {

}

b2Vec2 CharacterControllerCS::collisionResponse(b2Vec2 currentPosition, b2Vec2 initialTarget, b2Vec2 hitNormal, float friction, float bounciness) {

	b2Vec2 direction = initialTarget - currentPosition;
	float remainingDistance = direction.Length();
	b2Vec2 reflected = reflect(direction, hitNormal);
	b2Vec2 projection = b2Dot(reflected, hitNormal) * hitNormal;
	b2Vec2 tangent = reflected - projection;
	//---------------------------------------------------------------------------------------------------------
	// Compute the new target position using a linear model for the friction and bounciness. Notice that for
	// the friction part that we have (1 - friction). This is to ensure that 0 friction corresponds to no
	// resistance and 1 to max resistance. This also means that:
	// friction < 0 => higher velocity
	// friction > 1 => reversed velocity 
	//---------------------------------------------------------------------------------------------------------

	projection.Normalize();
	tangent.Normalize();
	b2Vec2 targetPosition = currentPosition + (bounciness * remainingDistance * projection) + ((1 - friction) * remainingDistance * tangent);
	return targetPosition;
}

b2Vec2 CharacterControllerCS::moveHorizontal(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations) {
	b2Vec2 currentPosition = position;
	b2Vec2 currentDirection = direction;
	b2Vec2 targetPosition = currentPosition + currentDirection;

	int i = 0;
	while (i < maxIterations) {

		bool hit = false;
		b2Vec2 normal;
		normal.SetZero();

		float directionX = sgn(currentDirection.x);
		float distance = currentDirection.Length();
		b2Vec2 direction = distance == 0.0f ? b2Vec2(0.0f, 0.0f) : (1.0f / distance) * currentDirection;

		std::vector<b2Vec2> rayOrigins;

		if ((directionX == -1)) {
			std::transform(positionsLeft.cbegin(), positionsLeft.cend(), std::back_inserter(rayOrigins), [&position](const b2Vec2& v){ return v + position; });
		}else {
			std::transform(positionsRight.cbegin(), positionsRight.cend(), std::back_inserter(rayOrigins), [&position](const b2Vec2& v) -> b2Vec2 {  return v + position; });
		}

		for (int i = 0; i <rayOrigins.size(); i++) {
			b2Vec2 rayOrigin = rayOrigins[i];
			b2Vec2 target = rayOrigin + ((distance + m_skinWidth) *  direction);

			m_callback.reset();
			Globals::world->RayCast(&m_callback, rayOrigin, target);
			if (m_callback.m_hit) {
				float adjustedDistance = m_callback.m_fraction * distance;
				if (adjustedDistance < distance) {
					distance = adjustedDistance;
					normal = m_callback.m_normal;
					hit = true;					
				}
			}
		}

		if (!hit) {
			currentDirection.Normalize();
			currentPosition = distance > m_skinWidth ? currentPosition + (distance - m_skinWidth) * currentDirection : currentPosition;
			break;
		}

		float normalDotUp = b2Dot(normal, b2Vec2(0.0f, 1.0f));
		b2Vec2 verticalDirection = normalDotUp >= 0 ? b2Vec2(0.0f, 1.0f) : -b2Vec2(0.0f, 1.0f);
		float slopeAngle = std::acosf(b2Dot(normal, verticalDirection)) * _180_ON_PI;

		bool applyCollisionResponse = false;

		if (normalDotUp >= 0) {
			if (slopeAngle <= m_maxClimbAngle) {
				applyCollisionResponse = true;
				collisions.flags |= CollisionInfoCS::CollisionFlags::SlightPoly;
			}
		}

		if (b2Dot(b2Vec2(1.0f, 0.0f), normal) < 0) {
			collisions.flags |= CollisionInfoCS::CollisionFlags::Front;
		}else {
			collisions.flags |= CollisionInfoCS::CollisionFlags::Back;
		}

		currentDirection.Normalize();
		currentPosition = distance > m_skinWidth ? currentPosition + (distance - m_skinWidth) * currentDirection : currentPosition;

		targetPosition = applyCollisionResponse ? collisionResponse(currentPosition, targetPosition, normal, 0, 0.0) : currentPosition;
		currentDirection = targetPosition - currentPosition;

		if (currentDirection.LengthSquared() < 0.0001f) {
			break;
		}

		position = currentPosition;
		i++;
	}
	position = currentPosition;
	return currentPosition;
}

b2Vec2 CharacterControllerCS::moveVertical(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations, bool reset) {

	b2Vec2 currentPosition = position;
	b2Vec2 currentDirection = direction;
	b2Vec2 targetPosition = currentPosition + currentDirection;

	int iterations = 0;
	while (iterations < maxIterations) {

		b2Vec2 normal;
		normal.SetZero();
		bool hit = false;

		float directionY = sgn(currentDirection.y);
		float distance = currentDirection.Length();
		b2Vec2 direction = distance == 0.0f ? b2Vec2(0.0f, 0.0f) : (1.0f / distance) * currentDirection;

		std::vector<b2Vec2> rayOrigins;

		if ((directionY == -1)) {
			std::transform(positionsBottom.cbegin(), positionsBottom.cend(), std::back_inserter(rayOrigins), [&position](const b2Vec2& v) { return v + position; });
		}else {
			std::transform(positionsTop.cbegin(), positionsTop.cend(), std::back_inserter(rayOrigins), [&position](const b2Vec2& v) -> b2Vec2 {  return v + position; });
		}

		for (int i = 0; i < rayOrigins.size(); i++) {
			b2Vec2 rayOrigin = rayOrigins[i];
			b2Vec2 target = rayOrigin + (distance + m_skinWidth) * direction;

			m_callback.reset();
			Globals::world->RayCast(&m_callback, rayOrigin, target);
			if (m_callback.m_hit) {
				float adjustedDistance = m_callback.m_fraction * distance;
				if (adjustedDistance < distance) {
					distance = adjustedDistance;

					normal = m_callback.m_normal;
					
					if (m_callback.m_platformVer) {
						if (abs((m_position.y - m_size.y * 0.5) - m_callback.m_body->GetFixtureList()->GetAABB(0).upperBound.y) > 0.5f && m_callback.m_body->GetLinearVelocity().y > 0.0f && m_velocity.x == 0.0f) {
							currentPosition.y = m_callback.m_body->GetFixtureList()->GetAABB(0).upperBound.y + m_size.y * 0.5;
							currentDirection.SetZero();
						}

						if (abs(m_callback.m_body->GetFixtureList()->GetAABB(0).lowerBound.y - (m_position.y + m_size.y * 0.5)) > 0.0f && m_callback.m_body->GetLinearVelocity().y != 0.0f && m_velocity.y > 0.0f) {
							currentPosition.y = m_callback.m_body->GetFixtureList()->GetAABB(0).lowerBound.y - m_size.y * 0.5;
							currentDirection.SetZero();
							m_velocityParent = m_callback.m_body->GetLinearVelocity();
						}

						float normalDotUp = b2Dot(m_callback.m_normal, b2Vec2(0.0f, 1.0f));
						if (normalDotUp > 0.0) {
							collisions.flags |= CollisionInfoCS::CollisionFlags::PlatformTop;
						}else {
							collisions.flags |= CollisionInfoCS::CollisionFlags::PlatformBottom;
						}
						collisions.flags |= CollisionInfoCS::CollisionFlags::Platform;
						m_parentBody = m_callback.m_body;
					}
					hit = true;
				}
			}
		}

		if (collisions.platformToSlight) {
			m_parentBody = nullptr;
		}
	
		if (!hit) {
			if ( !collisions.guardPlatform) {
				m_parentBody = reset ? nullptr : m_parentBody;
			}

			currentDirection.Normalize();
			currentPosition = distance > m_skinWidth ? currentPosition + (distance - m_skinWidth) * currentDirection : currentPosition;
			break;
		}

		float normalDotUp = b2Dot(normal, b2Vec2(0.0f, 1.0f));
		b2Vec2 verticalDirection = normalDotUp >= 0 ? b2Vec2(0.0f, 1.0f) : -b2Vec2(0.0f, 1.0f);
		float slopeAngle = std::acosf(b2Dot(normal, verticalDirection)) * _180_ON_PI;

		bool applyCollisionResponse = false;

		if (normalDotUp == 1.0f) {
			collisions.flags |= CollisionInfoCS::CollisionFlags::Bottom;
		}else if (normalDotUp >= 0) {

			if (slopeAngle > m_maxClimbAngle) {
				applyCollisionResponse = true;
				collisions.flags |= CollisionInfoCS::CollisionFlags::SteepPoly;
			}
			else {
				collisions.flags |= CollisionInfoCS::CollisionFlags::SlightPoly;
				collisions.slopeAngle = slopeAngle;
			}

		}else {				
			collisions.flags |= CollisionInfoCS::CollisionFlags::Top;
		}

		currentDirection.Normalize();
		currentPosition = distance > m_skinWidth ? currentPosition + (distance - m_skinWidth) * currentDirection : currentPosition;

		targetPosition = applyCollisionResponse ? collisionResponse(currentPosition, targetPosition, normal, 0.0, 0.0) : currentPosition;
		currentDirection = targetPosition - currentPosition;

		if (currentDirection.LengthSquared() < 0.0001f) {
			break;
		}

		position = currentPosition;
		iterations++;
	}

	position = currentPosition;
	return currentPosition;
}

void CharacterControllerCS::move(b2Vec2 velocity) {
	b2Vec2 vertVector = b2Dot(b2Vec2(0.0f, 1.0f), velocity) * b2Vec2(0.0f, 1.0f);
	b2Vec2 sideVector = (velocity - vertVector);

	if (m_parentBody != NULL) {

		b2Vec2 positionHor = m_position;
		b2Vec2 positionVer = m_position;
		b2Vec2 position = m_position;

		//during the collision passes this pointer is maybe set to null
		b2Vec2 _velocity = m_fdt * m_parentBody->GetLinearVelocity();
		b2Vec2 _position = m_parentBody->GetPosition();
		float top = m_parentBody->GetFixtureList()->GetAABB(0).upperBound.y;
		float left = m_parentBody->GetFixtureList()->GetAABB(0).lowerBound.x;
		float right = m_parentBody->GetFixtureList()->GetAABB(0).upperBound.x;

		//first get the collision flags from the platform
		position = moveHorizontal(position, b2Vec2(sideVector.x + _velocity.x, 0.0f), 5);

		if (_velocity.y < 0.0f) {
			position = moveVertical(position, b2Vec2(0.0f, _velocity.y), 5, false);
		}

		if (sideVector.x != 0) {
			positionHor = moveHorizontal(m_position, b2Vec2(sideVector.x + _velocity.x, 0.0f), 5);
		}

		if ((collisions.flags & CollisionInfoCS::CollisionFlags::Back) || (collisions.flags & CollisionInfoCS::CollisionFlags::Front)) {


			m_position = positionHor;
			if ((collisions.flags & CollisionInfoCS::CollisionFlags::SlightPoly)) {

				m_callback.resetBody();
				m_parentBody = nullptr;
				collisions.guardPlatform = 0;
				collisions.flags &= ~CollisionInfoCS::CollisionFlags::Platform;

			}else {

				if (!collisions.wasSlope) {
					m_position.y += _velocity.y;
				}else {
					m_position = moveVertical(m_position, vertVector, 5);
				}
			}
		}else {
			collisions.guardPlatform = 1;
			m_position.y += _velocity.y;
			m_position.x += _velocity.x;

			if (((m_position.x + m_size.x * 0.5) < left)
				|| ((m_position.x - m_size.x * 0.5) > right)) {
				collisions.flags &= ~CollisionInfoCS::CollisionFlags::Platform;
				m_callback.resetBody();
				m_parentBody = nullptr;
				collisions.guardPlatform = 0;
			}

			if ((collisions.flags & CollisionInfoCS::CollisionFlags::SlightPoly)) {
				m_callback.resetBody();
				m_parentBody = nullptr;
				collisions.guardPlatform = 0;
				collisions.flags &= ~CollisionInfoCS::CollisionFlags::Platform;
				collisions.platformToSlight = 30;
				transitions |= CharacterTransition::PLATFORM_TO_SLIGHT_SLOPE;
				m_position.y += 5.0f;
			}

			if ((collisions.flags & CollisionInfoCS::CollisionFlags::SteepPoly)) {
				m_callback.resetBody();
				m_parentBody = nullptr;
				collisions.guardPlatform = 0;
				collisions.flags &= ~CollisionInfoCS::CollisionFlags::Platform;
				transitions |= CharacterTransition::PLATFORM_TO_STEEP_SLOPE;
			}

			m_position = moveHorizontal(m_position, sideVector, 5);
		}
	}else {

		if (!collisions.guardPlatform) {
			m_callback.resetBody();
			m_parentBody = nullptr;
			collisions.guardPlatform = 0;
			collisions.flags &= ~CollisionInfoCS::CollisionFlags::Platform;
		}

		m_position = moveHorizontal(m_position, sideVector, 5);
		m_position = moveVertical(m_position, vertVector, 5);
	}

	m_body->SetTransform(m_position, 0.0f);
}

///////////////////////////////////////
void CharacterControllerCS::fixedUpdate() {
	updateVelocity();
	collisions.reset();
	resetTransitions();

	move(m_fdt * m_velocity);
}

void CharacterControllerCS::updateVelocity() {

	if ((Globals::CONTROLLS & Globals::KEY_A || Globals::CONTROLLS & Globals::KEY_D)) {
		if (Globals::CONTROLLS & Globals::KEY_A) {
			m_velocity.x = -m_movementSpeed;
		}

		if (Globals::CONTROLLS & Globals::KEY_D) {
			m_velocity.x = +m_movementSpeed;
		}
	}else {
		m_velocity.x = 0.0f;
	}

	if (collisions.flags & CollisionInfoCS::CollisionFlags::PlatformBottom) {
		m_callback.resetBody();
		m_parentBody = nullptr;
		collisions.guardPlatform = 0;

		m_velocity.y = 0.0f;
		m_velocity.y += -abs(m_velocityParent.y);

	}else if (collisions.flags & CollisionInfoCS::CollisionFlags::Platform) {
		if (Globals::CONTROLLS & Globals::KEY_W) {
			collisions.flags &= ~CollisionInfoCS::CollisionFlags::Platform;
			m_callback.resetBody();
			m_parentBody = nullptr;
			collisions.guardPlatform = 0;
			m_velocity.y = m_jumpVelocity;
		}else {
			m_velocity.y = 0.0f;
		}

	}else if (collisions.flags & CollisionInfoCS::CollisionFlags::SlightPoly) {

		if (Globals::CONTROLLS & Globals::KEY_W) {
			m_velocity.y = m_jumpVelocity;
			collisions.flags &= ~CollisionInfoCS::CollisionFlags::SlightPoly;

		}else {
			if (collisions.slopeAngle > 45.0f) {
				m_velocity.y = -(std::tanf(collisions.slopeAngle * PI_ON_180)) * m_movementSpeed;
			}else {
				m_velocity.y = -m_movementSpeed;
			}
			
		}
	}else if (collisions.flags & CollisionInfoCS::CollisionFlags::SteepPoly) {

		if (Globals::CONTROLLS & Globals::KEY_W) {
			m_velocity.y = m_jumpVelocity;
			collisions.flags &= ~CollisionInfoCS::CollisionFlags::SteepPoly;
		}else {
			if (!collisions.wasSteep) {
				m_velocity.y = 0.0f;
			}
			m_velocity.y -= m_gravity * m_fdt;
		}

		if (!(collisions.flags & CollisionInfoCS::CollisionFlags::Bottom))
			m_velocity.x = 0.0f;
	}else {
		if (isGrounded()) {
			m_velocity.y = 0.0f;
			if (Globals::CONTROLLS & Globals::KEY_W) {
				m_velocity.y = m_jumpVelocity;
			}else {
				m_velocity.y -= m_gravity * m_fdt;
			}
			
		}else {
			//no poly and the last hit was a slight poly means the raycasting misses the ground.
			//to hold the line invert the velocity
			if (collisions.wasSlight) {
				m_velocity.x = -m_velocity.x * 0.8;
				m_velocity.y = m_velocity.y * 0.8f;
			}

			if (collisions.wasSteep) {
				m_velocity.y = m_velocity.y * 0.5f;
			}


			if (collisions.flags & CollisionInfoCS::CollisionFlags::Top) {
				m_velocity.y = 0.0f;
			}

			m_velocity.y -= m_gravity * m_fdt;			
		}		
	}
}