#include "MovingPlatform.h"

MovingPlatform::MovingPlatform(const float& dt, const float& fdt) : m_dt(dt), m_fdt(fdt) {


	b2BodyDef platformBodyDef;
	platformBodyDef.type = b2_kinematicBody;
	platformBodyDef.position.Set(platformPosition.x, platformPosition.y);
	platformBodyDef.angle = 0.0f * PI / 180;
	//blockBodyDef.userData = block;
	platformBody = Globals::world->CreateBody(&platformBodyDef);

	// Create block shape
	b2PolygonShape platformShape;
	platformShape.SetAsBox(50.0f, 5.0f);
	// Create shape definition and add to body
	b2FixtureDef platformShapeDef;
	platformShapeDef.shape = &platformShape;
	platformShapeDef.density = 10.0;
	platformShapeDef.friction = 0.0;
	platformShapeDef.restitution = 0.1f;
	platformShapeDef.userData.pointer = 3;
	platformBody->CreateFixture(&platformShapeDef);

	directionToFinish_ = (finishPosition_ - initialPosition_);
	directionToFinish_.Normalize();
}

void MovingPlatform::fixedUpdate() {

	b2Vec2 platformPos = platformBody->GetTransform().p;
	b2Vec2 newPos = platformPos;

	// move platform
	if (platformState_ == PLATFORM_STATE_MOVETO_FINISH) {
		b2Vec2 curDistance = finishPosition_ - platformPos;
		b2Vec2 curDirection = (1.0f / curDistance.Length()) * curDistance;

		float dist = curDistance.Length();
		float dotd = b2Dot(directionToFinish_, curDirection);


		if (dotd > 0.0f) {
			// slow down near the end
			if (dist < 1.0f) {
				curLiftSpeed_ *= 0.01f;
			}
			curLiftSpeed_ = Clamp(curLiftSpeed_, minLiftSpeed_, maxLiftSpeed_);
			newPos += curLiftSpeed_ * curDirection;
			platformBody->SetLinearVelocity(m_speed * curLiftSpeed_ * curDirection);

		}else {
			newPos = finishPosition_;
			curLiftSpeed_ = maxLiftSpeed_;
			platformState_ = PLATFORM_STATE_MOVETO_START;
			platformBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}

	}else if (platformState_ == PLATFORM_STATE_MOVETO_START) {
		b2Vec2 curDistance = initialPosition_ - platformPos;
		b2Vec2 curDirection = (1.0f / curDistance.Length()) * curDistance;
		float dist = curDistance.Length();
		float dotd = b2Dot(directionToFinish_, curDirection);

		if (dotd < 0.0f){
			// slow down near the end
			if (dist < 1.0f) {
				curLiftSpeed_ *= 0.01f;
			}

			curLiftSpeed_ = Clamp(curLiftSpeed_, minLiftSpeed_, maxLiftSpeed_);
			newPos += curLiftSpeed_ * curDirection;

			platformBody->SetLinearVelocity(m_speed * curLiftSpeed_ * curDirection);

		}else {
			newPos = initialPosition_;
			curLiftSpeed_ = maxLiftSpeed_;
			platformState_ = PLATFORM_STATE_MOVETO_FINISH;
			platformBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
		}
	}
}

void MovingPlatform::render() {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	b2Vec2 position = platformBody->GetPosition();
	b2PolygonShape *boxShape = static_cast<b2PolygonShape*>(platformBody->GetFixtureList()->GetShape());

	b2Vec2 v1 = boxShape->m_vertices[0];
	b2Vec2 v2 = boxShape->m_vertices[1];
	b2Vec2 v3 = boxShape->m_vertices[2];
	b2Vec2 v4 = boxShape->m_vertices[3];

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);

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
	glPopMatrix();
	glLoadIdentity();
}