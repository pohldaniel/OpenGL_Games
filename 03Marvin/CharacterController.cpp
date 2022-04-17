#include "CharacterController.h"

CharacterController::CharacterController(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {
	m_postion = b2Vec2(200.0f, 600.0f);

	b2BodyDef playerDef;
	playerDef.fixedRotation = true;
	playerDef.type = b2_kinematicBody;
	playerDef.position= m_postion;

	m_body = Globals::world->CreateBody(&playerDef);

	b2PolygonShape boundingBox;
	boundingBox.SetAsBox(15.0f, 15.0f);
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

CharacterController::~CharacterController() {

}

void CharacterController::render() {


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

void CharacterController::fixedUpdate() {
	
}

void CharacterController::horizontalCollision(b2Vec2& velocity) {
	
	float directionX = sgn(velocity.x);
	float rayLength = abs(velocity.x) + m_skinWidth;
	for (int i = 0; i < m_horizontalRayCount; i++) {
		b2Vec2 rayOrigin = (directionX == -1) ? raycastOrigins.bottomLeft : raycastOrigins.bottomRight;
		rayOrigin += (i * m_horizontalRaySpacing ) * b2Vec2(0.0f, 1.0f);
		b2Vec2 target = rayOrigin +  (directionX * b2Vec2(rayLength, 0.0f));
		m_callback.reset();
		Globals::world->RayCast(&m_callback, rayOrigin, target);
		if (m_callback.m_hit) {
			float slopeAngle = std::acosf(b2Dot(m_callback.m_normal, b2Vec2(0.0f, 1.0f))) * _180_ON_PI;
			if (i == 0 && slopeAngle <= m_maxClimbAngle) {
				if (collisions.descendingSlope) {
					collisions.descendingSlope = false;
					velocity = collisions.velocityOld;
				}
				float distanceToSlopeStart = 0.0f;
				if (slopeAngle != collisions.slopeAngleOld) {
					distanceToSlopeStart = m_callback.m_fraction * rayLength - m_skinWidth;
					velocity.x -= distanceToSlopeStart * directionX;
				}
				climbSlope(velocity, slopeAngle);
				velocity.x += distanceToSlopeStart * directionX;
			}

			//just use the first ray on slopes
			if (!collisions.climbingSlope || slopeAngle > m_maxClimbAngle) {
				velocity.x = (m_callback.m_fraction * rayLength - m_skinWidth)* directionX;
				rayLength = m_callback.m_fraction * rayLength;

				if (collisions.climbingSlope) {
					velocity.y = std::tanf(collisions.slopeAngle * PI_ON_180) * sgn(velocity.x);
				}

				collisions.left = directionX == -1;
				collisions.right = directionX == 1;
			}
		}
	}
}

void CharacterController::verticalCollision(b2Vec2& velocity) {
	float directionY = sgn(velocity.y);
	float rayLength = abs(velocity.y) + m_skinWidth;
	for (int i = 0; i < m_verticalRayCount; i++) {
		b2Vec2 rayOrigin = (directionY == -1) ? raycastOrigins.bottomLeft : raycastOrigins.topLeft;
		rayOrigin += (i * m_verticalRaySpacing + velocity.x) * b2Vec2(1.0f, 0.0f);
		b2Vec2 target = rayOrigin + (directionY *b2Vec2(0.0f, rayLength));
		m_callback.reset();
		Globals::world->RayCast(&m_callback, rayOrigin, target);
		if (m_callback.m_hit) {
			velocity.y = (m_callback.m_fraction * rayLength - m_skinWidth)* directionY;
			rayLength = m_callback.m_fraction * rayLength;

			if (collisions.climbingSlope) {
				velocity.x = velocity.y * (1.0f / (std::tanf(collisions.slopeAngle * PI_ON_180))) * sgn(velocity.x);
			}

			collisions.below = directionY == -1;
			collisions.above = directionY == 1;
		}
	}

	if (collisions.climbingSlope) {
		float directionX = sgn(velocity.x);
		rayLength = std::abs(velocity.x) + m_skinWidth;
		b2Vec2 rayOrigin = ((directionY == -1) ? raycastOrigins.bottomLeft : raycastOrigins.bottomRight) + velocity.y * b2Vec2(0.0f, 1.0f) ;

		m_callback.reset();
		Globals::world->RayCast(&m_callback, rayOrigin, rayOrigin + rayLength * directionX * b2Vec2(1.0f, 0.0f));
		if (m_callback.m_hit) {
			float slopeAngle = std::acosf(b2Dot(m_callback.m_normal, b2Vec2(0.0f, 1.0f))) * _180_ON_PI;
			if (slopeAngle != collisions.slopeAngle) {
				velocity.x = (m_callback.m_fraction * rayLength - m_skinWidth)* directionX;
				collisions.slopeAngle = slopeAngle;
			}
		}
	}
}

void CharacterController::updateRaycastOrigins() {
	raycastOrigins.bottomLeft = m_postion + b2Vec2(m_skinWidth - 15.0f, m_skinWidth - 15.0f);
	raycastOrigins.bottomRight = m_postion + b2Vec2(15.0f - m_skinWidth, m_skinWidth - 15.0f);

	raycastOrigins.topLeft = m_postion + b2Vec2(m_skinWidth - 15.0f, 15.0f - m_skinWidth);
	raycastOrigins.topRight = m_postion + b2Vec2(15.0f - m_skinWidth, 15.0f - m_skinWidth);
}


void CharacterController::move(b2Vec2 velocity) {
	updateRaycastOrigins();
	collisions.reset();
	collisions.velocityOld = velocity;


	//if (velocity.y < 0.0f) {
		descendSlope(velocity);
	//}

	if(velocity.x != 0.0f)
		horizontalCollision(velocity);

	verticalCollision(velocity);

	m_postion = m_postion + velocity;
	m_body->SetTransform(m_postion, 0.0f);
}

void CharacterController::climbSlope(b2Vec2& velocity, float slopeAngle) {
	float moveDistance = std::abs(velocity.x);
	float climbVelocitY = std::sinf(slopeAngle * PI_ON_180) * moveDistance;

	//jumping on slope
	if (velocity.y <= climbVelocitY) {
		velocity.y = std::sinf(slopeAngle * PI_ON_180) * moveDistance;
		velocity.x = std::cosf(slopeAngle * PI_ON_180) * moveDistance * sgn(velocity.x);
		collisions.below = true;
		collisions.climbingSlope = true;
		collisions.slopeAngle = slopeAngle;
	}	
}

void CharacterController::descendSlope(b2Vec2& velocity) {
	float directionX = sgn(velocity.x);
	b2Vec2 rayOrigin = (directionX == -1) ? raycastOrigins.bottomRight : raycastOrigins.bottomLeft;
	
	m_callback.reset();
	Globals::world->RayCast(&m_callback, rayOrigin, rayOrigin + (float)HEIGHT * b2Vec2(0.0f, -1.0f));
	if (m_callback.m_hit) {
		float slopeAngle = std::acosf(b2Dot(m_callback.m_normal, b2Vec2(0.0f, 1.0f))) * _180_ON_PI;
		if (slopeAngle != 0.0f && slopeAngle <= m_maxDescendAngle) {
			if (sgn(m_callback.m_normal.x) == directionX) {
				

				if (m_callback.m_fraction * (float)HEIGHT - m_skinWidth <= std::tanf(slopeAngle * PI_ON_180) * abs(velocity.x)) {
					float moveDistance = abs(velocity.x);
					float descendVelocitY = std::sinf(slopeAngle * PI_ON_180) * moveDistance;
					velocity.x = std::cosf(slopeAngle * PI_ON_180) * moveDistance * sgn(velocity.x);
					velocity.y -= descendVelocitY;

					collisions.slopeAngle = slopeAngle;
					collisions.descendingSlope = true;
					collisions.below = true;
				}
			}
		}
	}
}

///////////////////////////////////////
void CharacterController::update() {
	

	updateVelocity();
	
	move(m_dt * m_velocity);
}

void CharacterController::updateVelocity() {

	if (collisions.above || collisions.below) {
		m_velocity.y = 0.0f;
	}else {
		m_velocity.y -= m_gravity * m_dt;
	}

	

	if (Globals::CONTROLLS & Globals::KEY_A || Globals::CONTROLLS & Globals::KEY_D) {
		if (Globals::CONTROLLS & Globals::KEY_A) {
			m_velocity.x = -m_movementSpeed;
		}

		if (Globals::CONTROLLS & Globals::KEY_D) {
			m_velocity.x = +m_movementSpeed;
		}
	}else {
		m_velocity.x = 0.0f;
	}	

	if (Globals::CONTROLLS & Globals::KEY_W && collisions.below) {
		m_velocity.y = m_jumpVelocity;
	}
}