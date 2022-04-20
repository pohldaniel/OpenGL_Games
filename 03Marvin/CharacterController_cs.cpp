#include "CharacterController_cs.h"

CharacterControllerCS::CharacterControllerCS(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	m_postion = b2Vec2(200.0f, 600.0f);

	b2BodyDef playerDef;
	playerDef.fixedRotation = true;
	playerDef.type = b2_kinematicBody;
	playerDef.position = m_postion;

	m_body = Globals::world->CreateBody(&playerDef);

	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(15.0f, 15.0f);

	//boundingBox.RayCast

	b2FixtureDef playerFixture;
	playerFixture.shape = &boundingBox;
	playerFixture.friction = 0.0f;
	playerFixture.density = 1.0f;
	playerFixture.userData.pointer = 1;
	m_body->CreateFixture(&playerFixture);

	m_horizontalRaySpacing = (30.0f - m_skinWidth * 2) / (m_horizontalRayCount - 1);
	m_verticalRaySpacing = (30.0f - m_skinWidth * 2) / (m_verticalRayCount - 1);

	m_gravity = (2 * m_jumpHeight) / (m_timeToJumpApex * m_timeToJumpApex);
	m_jumpVelocity = m_gravity * m_timeToJumpApex;
}

CharacterControllerCS::~CharacterControllerCS() {

}

void CharacterControllerCS::render() {


#if DEBUGCOLLISION
	Matrix4f transProj = Globals::projection.transpose();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&transProj[0][0]);
	//if (m_callback.m_hit){
	/*m_target = m_callback.m_point;
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3f(m_postionD.x, m_postionD.y, 0.0f);
	glVertex3f(m_targetD.x, m_targetD.y, 0.0f);
	glEnd();*/

	//}else {
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(m_postionD.x, m_postionD.y, 0.0f);
	glVertex3f(m_targetD.x, m_targetD.y, 0.0f);
	glEnd();
	//}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	b2Vec2 position = m_body->GetPosition();
	b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(m_body->GetFixtureList()->GetShape());

	b2Vec2 v1 = boxShape->m_vertices[0];
	b2Vec2 v2 = boxShape->m_vertices[1];
	b2Vec2 v3 = boxShape->m_vertices[2];
	b2Vec2 v4 = boxShape->m_vertices[3];

	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);

	//left bottom corner
	float xpos = position.x + v1.x;
	float ypos = position.y + v1.y;
	float w = v2.x - v1.x;
	float h = v4.y - v1.y;

	glVertex3f(xpos, ypos, 0.0f);
	glVertex3f(xpos, (ypos + h), 0.0f);
	glVertex3f(xpos + w, (ypos + h), 0.0f);
	glVertex3f(xpos + w, ypos, 0.0f);
	glEnd();
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

		updateRaycastOrigins(position);

		b2Vec2 normal;
		normal.SetZero();

		float directionX = sgn(currentDirection.x);
		float distance = currentDirection.Length();
		b2Vec2 direction = distance == 0.0f ? b2Vec2(0.0f, 0.0f) : (1.0f / distance) * currentDirection;


		for (int i = 0; i < m_horizontalRayCount; i++) {
			b2Vec2 rayOrigin = (directionX == -1) ? raycastOrigins.bottomLeft : raycastOrigins.bottomRight;
			rayOrigin += (i * m_horizontalRaySpacing) * b2Vec2(0.0f, 1.0f);
			b2Vec2 target = rayOrigin + ((distance + m_skinWidth) *  direction);
						
			m_callback.reset();
			Globals::world->RayCast(&m_callback, rayOrigin, target);
			if (m_callback.m_hit) {
				float adjustedDistance = m_callback.m_fraction * distance;
				if (adjustedDistance < distance) {
					distance = adjustedDistance;
					normal = m_callback.m_normal;					
				}
			}
		}

		float normalDotUp = b2Dot(normal, b2Vec2(0.0f, 1.0f));
		b2Vec2 verticalDirection = normalDotUp >= 0 ? b2Vec2(0.0f, 1.0f) : -b2Vec2(0.0f, 1.0f);
		float slopeAngle = std::acosf(b2Dot(normal, verticalDirection)) * _180_ON_PI;
		
		bool applyCollisionResponse = false;

		if (normalDotUp >= 0){
			if (slopeAngle <= m_maxClimbAngle){
				applyCollisionResponse = true;
				collisions.flags |= CollisionInfoCS::CollisionFlags::SlightPoly;
			}
		}
		
		if (b2Dot(b2Vec2(1.0f, 0.0f), normal) < 0){
			collisions.flags |= CollisionInfoCS::CollisionFlags::Front;
		}else{
			collisions.flags |= CollisionInfoCS::CollisionFlags::Back;
		}

		currentDirection.Normalize();
		currentPosition = distance > m_skinWidth ? currentPosition + (distance -  m_skinWidth) * currentDirection : currentPosition;

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

b2Vec2 CharacterControllerCS::moveVertical(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations) {
	
	b2Vec2 currentPosition = position;
	b2Vec2 currentDirection = direction;
	b2Vec2 targetPosition = currentPosition + currentDirection;

	int iterations = 0;
	while (iterations < maxIterations) {

		updateRaycastOrigins(position);

		b2Vec2 normal;
		normal.SetZero();

		float directionY = sgn(currentDirection.y);
		float distance = currentDirection.Length();

		b2Vec2 direction = distance == 0.0f ? b2Vec2(0.0f, 0.0f) : (1.0f / distance) * currentDirection;

		for (int i = 0; i < m_verticalRayCount; i++) {
			b2Vec2 rayOrigin = (directionY == -1) ? raycastOrigins.bottomLeft : raycastOrigins.topLeft;
			rayOrigin += (i * m_verticalRaySpacing ) * b2Vec2(1.0f, 0.0f);
			b2Vec2 target = rayOrigin + (distance + m_skinWidth) * direction;

			m_callback.reset();
			Globals::world->RayCast(&m_callback, rayOrigin, target);
			if (m_callback.m_hit) {
				float adjustedDistance = m_callback.m_fraction * distance;
				if (adjustedDistance < distance) {
					distance = adjustedDistance;
					normal = m_callback.m_normal;					
				}
			}
		}

		float normalDotUp = b2Dot(normal, b2Vec2(0.0f, 1.0f));
		b2Vec2 verticalDirection = normalDotUp >= 0 ? b2Vec2(0.0f, 1.0f) : -b2Vec2(0.0f, 1.0f);
		float slopeAngle = std::acosf(b2Dot(normal, verticalDirection)) * _180_ON_PI;

		bool applyCollisionResponse = false;

		if (normalDotUp == 1.0f) {
			
			collisions.flags |= CollisionInfoCS::CollisionFlags::Bottom;
		}else if (normalDotUp >= 0) {		
			if (normal.x != 0.0f && normal.y != 0.0f) {
				if (slopeAngle > m_maxClimbAngle) {
					applyCollisionResponse = true;
					collisions.flags |= CollisionInfoCS::CollisionFlags::SteepPoly;
				}else {
					collisions.flags |= CollisionInfoCS::CollisionFlags::SlightPoly;
					collisions.slopeAngle = slopeAngle;
				}
			}	
		}else {
			if (slopeAngle > m_maxClimbAngle){
				applyCollisionResponse = true;
				collisions.flags |= CollisionInfoCS::CollisionFlags::Top;
			}			
		}

		/*if (collisions.applyCollisionResponse) {
			collisions.applyCollisionResponse = false;
			applyCollisionResponse = true;
		}*/

		if (currentDirection.y < 0.0f && !(collisions.flags & CollisionInfoCS::CollisionFlags::Bottom) && !(collisions.flags & CollisionInfoCS::CollisionFlags::SteepPoly)) {
			collisions.flags |= CollisionInfoCS::CollisionFlags::Jumping;
		}
				
		currentDirection.Normalize();
		currentPosition = distance > m_skinWidth ? currentPosition + (distance - m_skinWidth) * currentDirection : currentPosition;

		targetPosition = applyCollisionResponse ? collisionResponse(currentPosition, targetPosition, normal, 0.0, 0.0) : currentPosition;
		currentDirection = targetPosition - currentPosition;

		if (currentDirection.LengthSquared() < 0.0001f){
			break;
		}

		position = currentPosition;
		iterations++;
	}
	position = currentPosition;
	return currentPosition;
}

void CharacterControllerCS::updateRaycastOrigins(b2Vec2 position) {
	raycastOrigins.bottomLeft = position + b2Vec2(m_skinWidth - 15.0f, m_skinWidth - 15.0f);
	raycastOrigins.bottomRight = position + b2Vec2(15.0f - m_skinWidth, m_skinWidth - 15.0f);

	raycastOrigins.topLeft = position + b2Vec2(m_skinWidth - 15.0f, 15.0f - m_skinWidth);
	raycastOrigins.topRight = position + b2Vec2(15.0f - m_skinWidth, 15.0f - m_skinWidth);
}


void CharacterControllerCS::move(b2Vec2 velocity) {	
	

	b2Vec2 vertVector = b2Dot(b2Vec2(0.0f, 1.0f) , velocity) * b2Vec2(0.0f, 1.0f);
	b2Vec2 sideVector = velocity - vertVector;

	m_postion = moveHorizontal(m_postion, sideVector, 5);
	m_postion = moveVertical(m_postion, vertVector, 5);

	m_body->SetTransform(m_postion, 0.0f);
}

///////////////////////////////////////
void CharacterControllerCS::fixedUpdate() {
	updateVelocity();
	collisions.reset();

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

	if (collisions.flags & CollisionInfoCS::CollisionFlags::SlightPoly) {
		if (Globals::CONTROLLS & Globals::KEY_W) {
			m_velocity.y = m_jumpVelocity;			
			collisions.flags &= ~CollisionInfoCS::CollisionFlags::SlightPoly;
		}else {
			if (collisions.slopeAngle > 45.0f) {
				m_velocity.y = -(std::tanf(collisions.slopeAngle * PI_ON_180))* m_movementSpeed;
			}else {
				m_velocity.y = -m_movementSpeed;
			}	
		}
	}else if (collisions.flags & CollisionInfoCS::CollisionFlags::SteepPoly) {	
		if (Globals::CONTROLLS & Globals::KEY_W) {
			m_velocity.y = m_jumpVelocity;
		}else {
			if (collisions.wasJumping) {
				m_velocity.y = 0.0f;
			}
			m_velocity.y -= m_gravity * m_fdt;
		}
		m_velocity.x = 0.0f;
	}else{	
		//no poly and the last hit was a slight poly means the raycasting misses the ground.
		//to hold the line invert the velocity
		if (collisions.wasSlight && !isGrounded()) {
			m_velocity.x = -m_velocity.x;			
		}
		
		if (isGrounded()) {
			m_velocity.y = 0.0f;

			if (Globals::CONTROLLS & Globals::KEY_W) {
				m_velocity.y = m_jumpVelocity;
			}

		}else {
			m_velocity.y -= m_gravity * m_fdt;
		}
	}
}