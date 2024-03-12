#include <GL/glew.h>
#include <engine/BuiltInShader.h>
#include "Model.h"
#include "AnimationManager.h"

Model::Model() : animator(m_skeleton) {

	Utils::SolidIO solidConverter;
	solidConverter.solidToBuffer("res/body/Body.solid", true, { 180.0f, 0.0f, 0.0f }, { 0.04f, 0.04f, 0.04f }, m_vertexBufferMap, m_indexBuffer);
	solidConverter.loadSkeleton("res/skeleton/BasicFigure", m_vertexBufferMap, m_vertexBuffer, m_skeleton, m_weights, m_boneIds);

	m_vertexNum = m_vertexBuffer.size() / 5;
	m_muscleNum = m_skeleton.m_muscles.size();

	m_drawCount = m_indexBuffer.size();

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * Utils::MAX_JOINTS, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * Utils::MAX_JOINTS);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_skinMatrices = new Matrix4f[m_muscleNum];
	resetPose();
	
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_muscleNum, m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(3, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_vertexNum * sizeof(float) * 5, &m_vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_weights.size() * sizeof(float) * 4, &m_weights.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, m_boneIds.size() * sizeof(std::array<unsigned int, 4>), &m_boneIds.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 4, GL_UNSIGNED_INT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexBuffer.size(), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	m_bindpose = true;
}

Model::~Model() {
	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo[0])
		glDeleteBuffers(1, &m_vbo[0]);

	if (m_vbo[1])
		glDeleteBuffers(1, &m_vbo[1]);

	if (m_vbo[2])
		glDeleteBuffers(1, &m_vbo[2]);

	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();

	m_weights.clear();
	m_weights.shrink_to_fit();

	m_boneIds.clear();
	m_boneIds.shrink_to_fit();

	m_vertexBufferMap.clear();
	m_vertexBufferMap.shrink_to_fit();
}

void Model::resetPose() {
	for (size_t i = 0; i < m_muscleNum; ++i) {
		m_skinMatrices[i] = m_skeleton.m_muscles[i].m_modelMatrixInitial;
	}
	m_bindpose = false;
	animator.frameCurrent = 0;
}

void Model::bindPose() {
	m_bindpose = true;
}

void Model::draw() {

	if (m_bindpose) {
		for (unsigned int j = 0; j < m_muscleNum; j++) {
			Vector3f mid = (m_skeleton.m_joints[m_skeleton.m_muscles[j].parentIndex1].position + m_skeleton.m_joints[m_skeleton.m_muscles[j].parentIndex2].position) * 0.5f;
			m_skinMatrices[j] = Matrix4f::Translate(mid) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f - m_skeleton.m_muscles[j].rotate1) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f - m_skeleton.m_muscles[j].rotate2) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -m_skeleton.m_muscles[j].rotate3);
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_muscleNum, m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::update(float dt) {
	animator.doAnimations(dt);
}
////////////////////////////////////////////////////////////////////////////////
Animator::Animator(Utils::Skeleton& skeleton): skeleton(skeleton) {
	//animTarget = Enums::bounceidleanim;
	//animCurrent = Enums::bounceidleanim;
	animTarget = Enums::runanim;
	animCurrent = Enums::runanim;
	howactive = Enums::typeactive;
	normalsupdatedelay = 0.0f;
	id = 0;
	landhard = false;
	crouchkeydown = false;
	crouchtogglekeydown = false;
	targetyaw = 0.0f;
	frameTarget = 1;
	target = 0.0f;
	numflipped = 0;
	feint = false;
	aitype = playercontrolled;
	escapednum = 0;
	reversaltrain = false;
	detail = 0;
	onfire = false;
	deathbleeding = 0.0f;
	yaw = 0.0f;
	transspeed = 0.0f;
	frameCurrent = 0;
	hasvictim = false;
	jumpkeydown = false;
	attackkeydown = false;
	hostile = 0;
	weaponactive = -1;
	num_weapons = 0;
	targetrot = 0.0f;

	weaponids[0] = -1;
	weaponids[1] = -1;
	weaponids[2] = -1;
	weaponids[3] = -1;
	superruntoggle = false;

	lastfeint = false;

	speed = 1.0f;
	velspeed = 1.0f;
	speedmult = 1.0f;
	scale = 1.0f;

	rot = 0.0f;
	oldrot = 0.0f;

	oldanimCurrent = 0;
	oldanimTarget = 0;
	oldframeCurrent = 0;
	oldframeTarget = 0;
	calcrot = false;
}

bool Animator::isIdle() {
	return Enums::animation_bits[animTarget] & Enums::ab_idle;
}

bool Animator::isFlip() {
	return Enums::animation_bits[animTarget] & Enums::ab_flip;
}

bool Animator::wasFlip() {
	return Enums::animation_bits[animCurrent] & Enums::ab_flip;
}

bool Animator::wasLanding() {
	return Enums::animation_bits[animCurrent] & Enums::ab_land;
}

bool Animator::isLanding() {
	return Enums::animation_bits[animTarget] & Enums::ab_land;
}

bool Animator::wasLandhard() {
	return Enums::animation_bits[animCurrent] & Enums::ab_landhard;
}

bool Animator::isLandhard() {
	return Enums::animation_bits[animTarget] & Enums::ab_landhard;
}

bool Animator::wasCrouch() {
	return Enums::animation_bits[animCurrent] & Enums::ab_crouch;
}

bool Animator::isCrouch() {
	return Enums::animation_bits[animTarget] & Enums::ab_crouch;
}

bool Animator::hasWeapon() {
	return (weaponactive != -1);
}

bool Animator::wasRun() {
	return Enums::animation_bits[animCurrent] & Enums::ab_run;
}

bool Animator::isRun() {
	return Enums::animation_bits[animTarget] & Enums::ab_run;
}

int Animator::getRun() {
	if (superruntoggle && (!hasWeapon())) {
		return Enums::rabbitrunninganim;
	}else {
		return Enums::runanim;
	}
}

Utils::AnimationFrame& Animator::currentFrame() {
	return AnimationManager::animations.at(animCurrent).frames.at(frameCurrent);
}

Utils::AnimationFrame& Animator::targetFrame() {
	return AnimationManager::animations.at(animTarget).frames.at(frameTarget);
}

int Animator::getIdle() {
	
	if (howactive == Enums::typesitting) {
		return Enums::sitanim;
	}
	if (howactive == Enums::typesittingwall) {
		return Enums::sitwallanim;
	}
	if (howactive == Enums::typesleeping) {
		return Enums::sleepanim;
	}
	if (howactive == Enums::typedead1) {
		return Enums::dead1anim;
	}
	if (howactive == Enums::typedead2) {
		return Enums::dead2anim;
	}
	if (howactive == Enums::typedead3) {
		return Enums::dead3anim;
	}
	if (howactive == Enums::typedead4) {
		return Enums::dead4anim;
	}
	return Enums::bounceidleanim;
}

int Animator::getLanding() {
	return Enums::landanim;
}

int Animator::getLandhard() {
	return  Enums::landhardanim;
}

int Animator::getCrouch() {
	return Enums::crouchanim;
}

bool Animator::isPlayerControlled() {
	return (aitype == playercontrolled);
}

bool Animator::wasStop() {
	return Enums::animation_bits[animCurrent] & Enums::ab_stop;
}

bool Animator::isStop() {
	return Enums::animation_bits[animTarget] & Enums::ab_stop;
}

int Animator::getStop() {
	return Enums::stopanim;
}

bool Animator::wasIdle() {
	return Enums::animation_bits[animCurrent] & Enums::ab_idle;
}

void Animator::FootLand(Utils::bodypart whichfoot, float opacity) {

	if ((whichfoot != Utils::leftfoot) && (whichfoot != Utils::rightfoot)) {
		std::cerr << "FootLand called on wrong bodypart" << std::endl;
		return;
	}
	static Vector3f terrainlight;
	static Vector3f footvel, footpoint;
	
}


void Animator::doAnimations(float dt) {
	if (!skeleton.free) {
		static float oldtarget;

		if (isIdle() && animCurrent != getIdle()) {
			normalsupdatedelay = 0.0f;
		}

		if (animTarget == Enums::tempanim || animCurrent == Enums::tempanim) {
			AnimationManager::animations[Enums::tempanim] = tempanimation;
		}
		if (animTarget == Enums::jumpupanim || animTarget == Enums::jumpdownanim || isFlip()) {
			//float gLoc[3];
			//gLoc[0] = coords.x;
			//gLoc[1] = coords.y;
			//gLoc[2] = coords.z;

			if (id == 0) {
				//OPENAL_3D_SetAttributes(channels[whooshsound], gLoc);
				//OPENAL_SetVolume(channels[whooshsound], 64 * findLength(&velocity) / 5);
			}
			if (((velocity[1] < -15) || (crouchkeydown && velocity[1] < -8)) && abs(velocity[1]) * 4 > sqrtf(velocity[0] * velocity[0] * velocity[2] * velocity[2])) {
				landhard = 1;
			}
			if (!crouchkeydown && velocity[1] >= -15) {
				landhard = 0;
			}
		}

		if ((animCurrent == Enums::jumpupanim || animTarget == Enums::jumpdownanim) && !isFlip() && (!isLanding() && !isLandhard()) && ((crouchkeydown && !crouchtogglekeydown))) {
			Vector3f targfacing;
			targfacing = Vector3f::ZERO;
			targfacing[2] = 1;

			targfacing = Math::RotatePoint(targfacing, 0, targetyaw, 0);

			if (Vector3f::NormalDot(targfacing, velocity) >= -.3) {
				animTarget = Enums::flipanim;
			}
			else {
				animTarget = Enums::backflipanim;
			}
			crouchtogglekeydown = 1;
			frameTarget = 0;
			target = 0;

			if (id == 0) {
				numflipped++;
			}
		}

		if (AnimationManager::animations[animTarget].attack != Utils::reversed) {
			feint = 0;
		}

		if (!crouchkeydown || (isLanding() || isLandhard()) || (wasLanding() || wasLandhard())) {
			crouchtogglekeydown = 0;
			if (isPlayerControlled()) {
				feint = 0;
			}
		}
		else {
			if (!crouchtogglekeydown && AnimationManager::animations[animTarget].attack == Utils::reversed && isPlayerControlled() && (escapednum < 2 || reversaltrain)) {
				feint = 1;
			}
			if (!isFlip()) {
				crouchtogglekeydown = 1;
			}
		}

		if (AnimationManager::animations[animTarget].attack || animCurrent == Enums::getupfrombackanim || animCurrent == Enums::getupfromfrontanim) {
			if (detail) {
				normalsupdatedelay = 0;
			}
		}

		if (target >= 1) {
			if (animTarget == Enums::rollanim && frameTarget == 3 && onfire) {
				onfire = false;
				//emit_sound_at(fireendsound, coords);
				//pause_sound(stream_firesound);
				deathbleeding = 0.0f;
			}

			if (animTarget == Enums::rabbittacklinganim && frameTarget == 1) {
				/*if (victim->aitype == attacktypecutoff && victim->stunned <= 0 && victim->surprised <= 0 && victim->id != 0) {
					Reverse();
				}
				if (animTarget == Enums::rabbittacklinganim && frameTarget == 1 && !victim->isCrouch() && victim->animTarget != Enums::backhandspringanim) {
					if (Vector3f::NormalDot(victim->facing, facing) > 0) {
						victim->animTarget = Enums::rabbittackledbackanim;
					}
					else {
						victim->animTarget = Enums::rabbittackledfrontanim;
					}
					victim->frameTarget = 2;
					victim->target = 0;
					victim->yaw = yaw;
					victim->targetyaw = yaw;
					if (victim->aitype == gethelptype) {
						victim->DoDamage(victim->damagetolerance - victim->damage);
					}
					if (PersonType::types[creature].hasClaws) {
						DoBloodBig(0, 255);
						emit_sound_at(clawslicesound, victim->coords);
						victim->spurt = 1;
						victim->DoBloodBig(1 / victim->armorhead, 210);
					}
					award_bonus(id, TackleBonus,victim->aitype == gethelptype ? 50 : 0);
				}*/
			}

			/*if (!drawtogglekeydown && drawkeydown && (!hasWeapon() || num_weapons == 1) && (targetFrame().label || (animTarget != animCurrent && animCurrent == rollanim)) && num_weapons > 0 && creature != wolftype) {
				if (weapons[weaponids[0]].getType() == knife) {
					if (!hasWeapon()) {
						weaponactive = 0;
						//emit_sound_at(knifedrawsound, coords, 128);
					}
					else if (weaponactive == 0) {
						weaponactive = -1;
						//emit_sound_at(knifesheathesound, coords);
					}
				}
				drawtogglekeydown = 1;
			}*/

			//Footstep sounds
			/*if (!TutorialLu::active || id == 0) {
				if ((targetFrame().label && (targetFrame().label < 5 || targetFrame().label == 8))) {
					int whichsound = -1;
					if (onterrain) {
						if (terrain.getOpacity(coords.x, coords.z) < .2) {
							if (targetFrame().label == 1) {
								whichsound = footstepsound;
							}
							else {
								whichsound = footstepsound2;
							}
							if (targetFrame().label == 1) {
								FootLand(leftfoot, 1);
							}
							if (targetFrame().label == 2) {
								FootLand(rightfoot, 1);
							}
							if (targetFrame().label == 3 && isRun()) {
								FootLand(rightfoot, 1);
								FootLand(leftfoot, 1);
							}
						}
						if (terrain.getOpacity(coords.x, coords.z) >= .2) {
							if (targetFrame().label == 1) {
								whichsound = footstepsound3;
							}
							else {
								whichsound = footstepsound4;
							}
						}
					}
					if (!onterrain) {
						if (targetFrame().label == 1) {
							whichsound = footstepsound3;
						}
						else {
							whichsound = footstepsound4;
						}
					}
					if (targetFrame().label == 4 && (!hasWeapon() || (animTarget != knifeslashstartanim && animTarget != knifethrowanim && animTarget != crouchstabanim && animTarget != swordgroundstabanim && animTarget != knifefollowanim))) {
						if (AnimationLu::animations[animTarget].attack != neutral) {
							unsigned r = abs(Random() % 3);
							if (r == 0) {
								whichsound = lowwhooshsound;
							}
							if (r == 1) {
								whichsound = midwhooshsound;
							}
							if (r == 2) {
								whichsound = highwhooshsound;
							}
						}
						if (AnimationLu::animations[animTarget].attack == neutral) {
							whichsound = movewhooshsound;
						}
					}
					else if (targetFrame().label == 4) {
						whichsound = knifeswishsound;
					}
					if (targetFrame().label == 8 && !TutorialLu::active) {
						whichsound = landsound2;
					}

					if (whichsound != -1) {
						emit_sound_at(whichsound, coords, 256.);

						if (id == 0) {
							if (whichsound == footstepsound || whichsound == footstepsound2 || whichsound == footstepsound3 || whichsound == footstepsound4) {
								if (animTarget == wolfrunninganim || animTarget == rabbitrunninganim) {
									addEnvSound(coords, 15);
								}
								else {
									addEnvSound(coords, 6);
								}
							}
						}

						if (targetFrame().label == 3) {
							whichsound--;
							emit_sound_at(whichsound, coords, 128.);
						}
					}
				}
			}

			//Combat sounds
			if (!TutorialLu::active || id == 0) {
				if (speechdelay <= 0) {
					if (animTarget != crouchstabanim && animTarget != swordgroundstabanim && animTarget != staffgroundsmashanim) {
						if ((targetFrame().label && (targetFrame().label < 5 || targetFrame().label == 8))) {
							int whichsound = -1;
							if (targetFrame().label == 4 && !isPlayerControlled()) {
								if (AnimationLu::animations[animTarget].attack != neutral) {
									unsigned r = abs(Random() % 4);
									whichsound = PersonType::types[creature].soundsAttack[r];
									speechdelay = .3;
								}
							}

							if (whichsound != -1) {
								emit_sound_at(whichsound, coords);
							}
						}
					}
				}
			}*/

			if ((!wasLanding() && !wasLandhard()) && animCurrent != getIdle() && (isLanding() || isLandhard())) {
				FootLand(Utils::bodypart::leftfoot, 1);
				FootLand(Utils::bodypart::rightfoot, 1);
			}

			transspeed = 0.0f;
			currentoffset = targetoffset;
			frameTarget = frameCurrent;
			animCurrent = animTarget;
			frameTarget++;

			/*if (animCurrent == Enums::removeknifeanim && currentFrame().label == 5) {
				for (unsigned i = 0; i < weapons.size(); i++) {
					if (weapons[i].owner == -1) {
						if (distsqflat(&coords, &weapons[i].position) < 4 && !hasWeapon()) {
							if (distsq(&coords, &weapons[i].position) >= 1) {
								if (weapons[i].getType() != staff) {
									emit_sound_at(knifedrawsound, coords, 128.);
								}

								takeWeapon(i);
							}
						}
					}
				}
			}*/

			/*if (animCurrent == crouchremoveknifeanim && currentFrame().label == 5) {
				for (unsigned i = 0; i < weapons.size(); i++) {
					bool willwork = true;
					if (weapons[i].owner != -1) {
						if (Person::players[weapons[i].owner]->weaponstuck != -1) {
							if (Person::players[weapons[i].owner]->weaponids[Person::players[weapons[i].owner]->weaponstuck] == int(i)) {
								if (Person::players[weapons[i].owner]->num_weapons > 1) {
									willwork = 0;
								}
							}
						}
					}
					if ((weapons[i].owner == -1) || (hasvictim && (weapons[i].owner == int(victim->id)) && victim->skeleton.free)) {
						if (willwork && distsqflat(&coords, &weapons[i].position) < 3 && !hasWeapon()) {
							if (distsq(&coords, &weapons[i].position) < 1 || hasvictim) {
								bool fleshstuck = false;
								if (weapons[i].owner != -1) {
									if (victim->weaponstuck != -1) {
										if (victim->weaponids[victim->weaponstuck] == int(i)) {
											fleshstuck = true;
										}
									}
								}
								if (fleshstuck) {
									emit_sound_at(fleshstabremovesound, coords, 128.);
								}
								else {
									if (weapons[i].getType() != staff) {
										emit_sound_at(knifedrawsound, coords, 128.);
									}
								}
								if (weapons[i].owner != -1) {
									victim = Person::players[weapons[i].owner];
									if (victim->num_weapons == 1) {
										victim->num_weapons = 0;
									}
									else {
										victim->num_weapons = 1;
									}

									//victim->weaponactive=-1;
									victim->skeleton.longdead = 0;
									victim->skeleton.free = 1;
									victim->skeleton.broken = 0;

									for (unsigned j = 0; j < victim->skeleton.joints.size(); j++) {
										victim->skeleton.joints[j].velchange = 0;
										victim->skeleton.joints[j].locked = 0;
									}

									XYZ relative;
									relative = 0;
									relative.y = 10;
									Normalise(&relative);
									XYZ footvel, footpoint;
									footvel = 0;
									footpoint = weapons[i].position;
									if (victim->weaponstuck != -1) {
										if (victim->weaponids[victim->weaponstuck] == int(i)) {
											if (bloodtoggle) {
												Sprite::MakeSprite(cloudimpactsprite, footpoint, footvel, 1, 0, 0, .8, .3);
											}
											weapons[i].bloody = 2;
											weapons[i].blooddrip = 5;
											victim->weaponstuck = -1;
										}
									}
									if (victim->num_weapons > 0) {
										if (victim->weaponstuck != 0 && victim->weaponstuck != -1) {
											victim->weaponstuck = 0;
										}
										if (victim->weaponids[0] == int(i)) {
											victim->weaponids[0] = victim->weaponids[victim->num_weapons];
										}
									}

									victim->jointVel(abdomen) += relative * 6;
									victim->jointVel(neck) += relative * 6;
									victim->jointVel(rightshoulder) += relative * 6;
									victim->jointVel(leftshoulder) += relative * 6;
								}
								takeWeapon(i);
							}
						}
					}
				}
			}*/

			/*if (animCurrent == Enums::drawleftanim && currentFrame().label == 5) {
				if (!hasWeapon()) {
					weaponactive = 0;
					emit_sound_at(knifedrawsound, coords, 128.);
				}
				else if (weaponactive == 0) {
					weaponactive = -1;
					if (num_weapons == 2) {
						int buffer;
						buffer = weaponids[0];
						weaponids[0] = weaponids[1];
						weaponids[1] = buffer;
					}
					emit_sound_at(knifesheathesound, coords, 128.);
				}
			}*/

			if ((animCurrent == Enums::walljumprightkickanim && animTarget == Enums::walljumprightkickanim) || (animCurrent == Enums::walljumpleftkickanim && animTarget == Enums::walljumpleftkickanim)) {
				Vector3f rotatetarget = Math::RotatePoint(skeleton.forward, 0, yaw, 0);
				Vector3f::Normalize(rotatetarget);
				targetyaw = -asin(0 - rotatetarget[0]);
				targetyaw *= 360 / 6.28;
				if (rotatetarget[2] < 0) {
					targetyaw = 180 - targetyaw;
				}

				if (animTarget == Enums::walljumprightkickanim) {
					targetyaw += 40;
				}
				if (animTarget == Enums::walljumpleftkickanim) {
					targetyaw -= 40;
				}
			}

			bool dojumpattack;
			dojumpattack = 0;
			if ((animTarget == Enums::rabbitrunninganim || animTarget == Enums::wolfrunninganim) && frameTarget == 3 && (jumpkeydown || attackkeydown || id != 0)) {
				dojumpattack = 1;
			}
			if (hasvictim) {
				/*if (distsq(&victim->coords, &coords) < 5 && victim->aitype == gethelptype && (attackkeydown) && !victim->skeleton.free && victim->isRun() && victim->runninghowlong >= 1) {
					dojumpattack = 1;
				}*/
			}
			if (!hostile) {
				dojumpattack = 0;
			}

			if (dojumpattack) {
				if ((animTarget == Enums::rabbitrunninganim || animTarget == Enums::wolfrunninganim) && id == 0) {
					animTarget = Enums::rabbittackleanim;
					frameTarget = 0;
					//emit_sound_at(jumpsound, coords);
				}

				/*float closestdist;
				closestdist = 0;
				int closestid;
				closestid = -1;
				XYZ targetloc;
				targetloc = velocity;
				Normalise(&targetloc);
				targetloc += coords;
				for (unsigned i = 0; i < Person::players.size(); i++) {
					if (i != id) {
						if (distsq(&targetloc, &Person::players[i]->coords) < closestdist || closestdist == 0) {
							closestdist = distsq(&targetloc, &Person::players[i]->coords);
							closestid = i;
						}
					}
				}
				if (closestid != -1) {
					if (closestdist < 5 && !Person::players[closestid]->dead && AnimationLu::animations[Person::players[closestid]->animTarget].height != lowheight && Person::players[closestid]->animTarget != backhandspringanim) {
						hasvictim = 1;
						victim = Person::players[closestid];
						coords = victim->coords;
						animCurrent = rabbittacklinganim;
						animTarget = rabbittacklinganim;
						frameCurrent = 0;
						frameTarget = 1;
						XYZ rotatetarget;
						if (coords.z != victim->coords.z || coords.x != victim->coords.x) {
							rotatetarget = coords - victim->coords;
							Normalise(&rotatetarget);
							targetyaw = -asin(0 - rotatetarget.x);
							targetyaw *= 360 / 6.28;
							if (rotatetarget.z < 0) {
								targetyaw = 180 - targetyaw;
							}
						}
						if (animTarget != rabbitrunninganim) {
							emit_sound_at(jumpsound, coords, 128.);
						}
					}
				}*/
			}

			//Move impacts
			/*float damagemult = PersonType::types[creature].power * power;
			if (hasvictim) {
				damagemult /= victim->damagetolerance / 200;
			}*/

			//Animation end
			if (frameTarget >= int(AnimationManager::animations[animCurrent].frames.size())) {
				frameTarget = 0;
				if (wasStop()) {
					animTarget = getIdle();
					FootLand(Utils::bodypart::leftfoot, 1);
					FootLand(Utils::bodypart::rightfoot, 1);
				}
				if (animCurrent == Enums::rabbittackleanim || animCurrent == Enums::rabbittacklinganim) {
					animTarget = Enums::rollanim;
					frameTarget = 3;
					//emit_sound_at(movewhooshsound, coords, 128.);
				}
				if (animCurrent == Enums::staggerbackhighanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::staggerbackhardanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::removeknifeanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::crouchremoveknifeanim) {
					animTarget = getCrouch();
				}
				if (animCurrent == Enums::backhandspringanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::dodgebackanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::drawleftanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::drawrightanim || animCurrent == Enums::crouchdrawrightanim) {
					animTarget = getIdle();
					if (animCurrent == Enums::crouchdrawrightanim) {
						animTarget = getCrouch();
					}
					if (!hasWeapon()) {
						weaponactive = 0;
						//emit_sound_at(knifedrawsound, coords, 128.);
					}
					else if (weaponactive == 0) {
						weaponactive = -1;
						if (num_weapons == 2) {
							int buffer;
							buffer = weaponids[0];
							weaponids[0] = weaponids[1];
							weaponids[1] = buffer;
						}
						//emit_sound_at(knifesheathesound, coords, 128.);
					}
				}
				if (animCurrent == Enums::rollanim) {
					animTarget = getCrouch();
					FootLand(Utils::bodypart::leftfoot, 1);
					FootLand(Utils::bodypart::rightfoot, 1);
				}
				if (isFlip()) {
					if (animTarget == Enums::walljumprightkickanim) {
						targetrot = -190;
					}
					if (animTarget == Enums::walljumpleftkickanim) {
						targetrot = 190;
					}
					animTarget = Enums::jumpdownanim;
				}
				if (animCurrent == Enums::climbanim) {
					animTarget = getCrouch();
					frameTarget = 1;
					/*coords += facing * .1;
					if (!isnormal(coords.x)) {
						coords = oldcoords;
					}
					oldcoords = coords;
					targetoffset = 0;
					currentoffset = 0;
					grabdelay = 1;
					velocity = 0;
					collided = 0;
					avoidcollided = 0;*/
				}
				if (animTarget == Enums::rabbitkickreversalanim) {
					animTarget = getCrouch();
					lastfeint = false;
				}
				if (animTarget == Enums::jumpreversalanim) {
					animTarget = getCrouch();
					lastfeint = false;
				}
				if (animTarget == Enums::walljumprightanim || animTarget == Enums::walljumpbackanim || animTarget == Enums::walljumpfrontanim) {
					if (attackkeydown && animTarget != Enums::walljumpfrontanim) {
						int closest = -1;
						float closestdist = -1;
						float distance;
						/*if (Person::players.size() > 1) {
							for (unsigned i = 0; i < Person::players.size(); i++) {
								if (id != i && Person::players[i]->coords.y < coords.y && !Person::players[i]->skeleton.free) {
									distance = distsq(&Person::players[i]->coords, &coords);
									if (closestdist == -1 || distance < closestdist) {
										closestdist = distance;
										closest = i;
									}
								}
							}
						}
						if (closestdist > 0 && closest >= 0 && closestdist < 16) {
							victim = Person::players[closest];
							animTarget = Enums::walljumprightkickanim;
							frameTarget = 0;
							XYZ rotatetarget = victim->coords - coords;
							Normalise(&rotatetarget);
							yaw = -asin(0 - rotatetarget.x);
							yaw *= 360 / 6.28;
							if (rotatetarget.z < 0) {
								yaw = 180 - yaw;
							}
							targettilt2 = -asin(rotatetarget.y) * 360 / 6.28;
							velocity = (victim->coords - coords) * 4;
							velocity.y += 2;
							transspeed = 40;
						}*/
					}
					if (animTarget == Enums::walljumpbackanim) {
						animTarget = Enums::backflipanim;
						frameTarget = 3;
						velocity = facing * -8;
						velocity[1] = 4;
						if (id == 0) {
							//resume_stream(whooshsound);
						}
					}
					if (animTarget == Enums::walljumprightanim) {
						animTarget = Enums::rightflipanim;
						frameTarget = 4;
						targetyaw -= 90;
						yaw -= 90;
						velocity = Math::RotatePoint(facing, 0, 30, 0) * -8;
						velocity[1] = 4;
					}
					if (animTarget == Enums::walljumpfrontanim) {
						animTarget = Enums::frontflipanim;
						frameTarget = 2;
						//targetyaw-=180;
						////yaw-=180;
						velocity = facing * 8;
						velocity[1] = 4;
					}
					if (id == 0) {
						//resume_stream(whooshsound);
					}
				}
				if (animTarget == Enums::walljumpleftanim) {
					if (attackkeydown) {
						int closest = -1;
						float closestdist = -1;
						float distance;
						/*if (Person::players.size() > 1) {
							for (unsigned i = 0; i < Person::players.size(); i++) {
								if (id != i && Person::players[i]->coords.y < coords.y && !Person::players[i]->skeleton.free) {
									distance = distsq(&Person::players[i]->coords, &coords);
									if (closestdist == -1 || distance < closestdist) {
										closestdist = distance;
										closest = i;
									}
								}
							}
						}
						if (closestdist > 0 && closest >= 0 && closestdist < 16) {
							victim = Person::players[closest];
							animTarget = walljumpleftkickanim;
							frameTarget = 0;
							XYZ rotatetarget = victim->coords - coords;
							Normalise(&rotatetarget);
							yaw = -asin(0 - rotatetarget.x);
							yaw *= 360 / 6.28;
							if (rotatetarget.z < 0) {
								yaw = 180 - yaw;
							}
							targettilt2 = -asin(rotatetarget.y) * 360 / 6.28;
							velocity = (victim->coords - coords) * 4;
							velocity.y += 2;
							transspeed = 40;
						}*/
					}
					if (animTarget != Enums::walljumpleftkickanim) {
						animTarget = Enums::leftflipanim;
						frameTarget = 4;
						targetyaw += 90;
						yaw += 90;
						velocity = Math::RotatePoint(facing, 0, -30, 0) * -8;
						velocity[1] = 4;
					}
					if (id == 0) {
						//resume_stream(whooshsound);
					}
				}
				if (animTarget == Enums::sneakattackanim) {
					animCurrent = getCrouch();
					animTarget = getCrouch();
					frameTarget = 1;
					frameCurrent = 0;
					targetyaw += 180;
					yaw += 180;
					/*targettilt2 *= -1;
					tilt2 *= -1;
					transspeed = 1000000;
					targetheadyaw += 180;
					coords -= facing * .7;
					if (onterrain) {
						coords.y = terrain.getHeight(coords.x, coords.z);
					}*/

					lastfeint = 0;
				}
				if (animTarget == Enums::knifesneakattackanim || animTarget == Enums::swordsneakattackanim) {
					animTarget = getIdle();
					frameTarget = 0;
					/*if (onterrain) {
						coords.y = terrain.getHeight(coords.x, coords.z);
					}*/

					lastfeint = 0;
				}
				if (animCurrent == Enums::knifefollowanim) {
					animTarget = getIdle();
					lastfeint = 0;
				}
				/*if (AnimationManager::animations[animTarget].attack == Enums::reversal && animCurrent != Enums::sneakattackanim && animCurrent != Enums::knifesneakattackanim && animCurrent != Enums::swordsneakattackanim && animCurrent != Enums::knifefollowanim) {
					float ycoords = oldcoords.y;
					animTarget = getStop();
					targetyaw += 180;
					yaw += 180;
					targettilt2 *= -1;
					tilt2 *= -1;
					transspeed = 1000000;
					targetheadyaw += 180;
					if (!isnormal(coords.x)) {
						coords = oldcoords;
					}
					if (animCurrent == Enums::spinkickreversalanim || animCurrent == Enums::swordslashreversalanim) {
						oldcoords = coords + facing * .5;
					}
					else if (animCurrent == Enums::sweepreversalanim) {
						oldcoords = coords + facing * 1.1;
					}
					else if (animCurrent == Enums::upunchreversalanim) {
						oldcoords = coords + facing * 1.5;
						targetyaw += 180;
						yaw += 180;
						targetheadyaw += 180;
						targettilt2 *= -1;
						tilt2 *= -1;
					}
					else if (animCurrent == Enums::knifeslashreversalanim) {
						oldcoords = coords + facing * .5;
						targetyaw += 90;
						yaw += 90;
						targetheadyaw += 90;
						targettilt2 = 0;
						tilt2 = 0;
					}
					else if (animCurrent == Enums::staffspinhitreversalanim) {
						targetyaw += 180;
						yaw += 180;
						targetheadyaw += 180;
						targettilt2 = 0;
						tilt2 = 0;
					}
					if (onterrain) {
						oldcoords.y = terrain.getHeight(oldcoords.x, oldcoords.z);
					}
					else {
						oldcoords.y = ycoords;
					}
					currentoffset = coords - oldcoords;
					targetoffset = 0;
					coords = oldcoords;

					lastfeint = 0;
				}*/
				if (animCurrent == Enums::knifesneakattackedanim || animCurrent == Enums::swordsneakattackedanim) {
					velocity = Vector3f::ZERO;
					velocity[1] = -5;
					//RagDoll(0);
				}
				if (AnimationManager::animations[animTarget].attack == Utils::reversed) {
					escapednum++;
					if (animTarget == Enums::sweepreversedanim) {
						targetyaw += 90;
					}
					animTarget = Enums::backhandspringanim;
					frameTarget = 2;
					//emit_sound_at(landsound, coords, 128);

					if (animCurrent == Enums::upunchreversedanim || animCurrent == Enums::swordslashreversedanim) {
						animTarget = Enums::rollanim;
						frameTarget = 5;
						//oldcoords = coords;
						//coords += (DoRotation(jointPos(leftfoot), 0, yaw, 0) + DoRotation(jointPos(rightfoot), 0, yaw, 0)) / 2 * scale;
						//coords.y = oldcoords.y;
					}
					if (animCurrent == Enums::knifeslashreversedanim) {
						animTarget = Enums::rollanim;
						frameTarget = 0;
						targetyaw += 90;
						yaw += 90;
						//oldcoords = coords;
						//coords += (DoRotation(jointPos(leftfoot), 0, yaw, 0) + DoRotation(jointPos(rightfoot), 0, yaw, 0)) / 2 * scale;
						//coords.y = oldcoords.y;
					}
				}
				if (wasFlip()) {
					animTarget = Enums::jumpdownanim;
				}
				if (wasLanding()) {
					animTarget = getIdle();
				}
				if (wasLandhard()) {
					animTarget = getIdle();
				}
				if (animCurrent == Enums::spinkickanim || animCurrent == Enums::getupfrombackanim || animCurrent == Enums::getupfromfrontanim || animCurrent == Enums::lowkickanim) {
					animTarget = getIdle();
					/*oldcoords = coords;
					coords += (DoRotation(jointPos(leftfoot), 0, yaw, 0) + DoRotation(jointPos(rightfoot), 0, yaw, 0)) / 2 * scale;
					coords.y = oldcoords.y;
					//coords+=DoRotation(Animation::animations[animCurrent].offset,0,yaw,0)*scale;
					targetoffset.y = coords.y;
					if (onterrain) {
						targetoffset.y = terrain.getHeight(coords.x, coords.z);
					}
					currentoffset = DoRotation(AnimationLu::animations[animCurrent].offset * -1, 0, yaw, 0) * scale;
					currentoffset.y -= (coords.y - targetoffset.y);
					coords.y = targetoffset.y;
					targetoffset = 0;
					normalsupdatedelay = 0;*/
				}
				if (animCurrent == Enums::upunchanim) {
					animTarget = getStop();
					normalsupdatedelay = 0;
					lastfeint = 0;
				}
				if (animCurrent == Enums::rabbitkickanim && animTarget != Enums::backflipanim) {
					targetyaw = yaw;
					bool hasstaff;
					hasstaff = 0;
					/*if (num_weapons > 0) {
						if (weapons[0].getType() == staff) {
							hasstaff = 1;
						}
					}
					if (!hasstaff) {
						DoDamage(35);
					}
					RagDoll(0);
					lastfeint = 0;
					rabbitkickragdoll = 1;*/
				}
				if (animCurrent == Enums::rabbitkickreversedanim) {
					if (!feint) {
						velocity = Vector3f::ZERO;
						velocity[1] = -10;
						//DoDamage(100);
						//RagDoll(0);
						skeleton.spinny = 0;
						//SolidHitBonus(!id); // FIXME: tricky id
					}
					if (feint) {
						escapednum++;
						animTarget = Enums::rollanim;
						//coords += facing;
						if (id == 0) {
							//pause_sound(whooshsound);
						}
					}
					lastfeint = 0;
				}
				if (animCurrent == Enums::rabbittackledbackanim || animCurrent == Enums::rabbittackledfrontanim) {
					velocity = Vector3f::ZERO;
					velocity[1] = -10;
					//RagDoll(0);
					skeleton.spinny = 0;
				}
				if (animCurrent == Enums::jumpreversedanim) {
					if (!feint) {
						velocity = Vector3f::ZERO;
						velocity[1] = -10;
						//DoDamage(100);
						//RagDoll(0);
						skeleton.spinny = 0;
						//SolidHitBonus(!id); // FIXME: tricky id
					}
					if (feint) {
						escapednum++;
						animTarget = Enums::rollanim;
						//coords += facing * 2;
						if (id == 0) {
							//pause_sound(whooshsound);
						}
					}
					lastfeint = 0;
				}

				/*if (AnimationManager::animations[animCurrent].attack == Enums::normalattack && !victim->skeleton.free && victim->animTarget != Enums::staggerbackhighanim && victim->animTarget != Enums::staggerbackhardanim && animTarget != Enums::winduppunchblockedanim && animTarget != Enums::blockhighleftanim && animTarget != Enums::swordslashparryanim && animTarget != Enums::swordslashparriedanim && animTarget != Enums::crouchstabanim && animTarget != Enums::swordgroundstabanim) {
					animTarget = getupfromfrontanim;
					lastfeint = 0;
				}
				else*/ if (AnimationManager::animations[animCurrent].attack == Utils::normalattack) {
					animTarget = getIdle();
					lastfeint = 0;
				}
				if (animCurrent == Enums::blockhighleftanim && !isPlayerControlled()) {
					animTarget = Enums::blockhighleftstrikeanim;
				}
				if (animCurrent == Enums::knifeslashstartanim || animCurrent == Enums::knifethrowanim || animCurrent == Enums::swordslashanim || animCurrent == Enums::staffhitanim || animCurrent == Enums::staffgroundsmashanim || animCurrent == Enums::staffspinhitanim) {
					animTarget = getIdle();
					lastfeint = 0;
				}
				/*if (animCurrent == Enums::spinkickanim && victim->skeleton.free) {
					if (creature == rabbittype) {
						animTarget = Enums::fightidleanim;
					}
				}*/
			}
			target = 0;

			if (isIdle() && !wasIdle()) {
				normalsupdatedelay = 0;
			}

			if (animCurrent == Enums::jumpupanim && velocity[1] < 0 && !isFlip()) {
				animTarget = Enums::jumpdownanim;
			}
		}

		if (!skeleton.free) {
			oldtarget = target;
			if (!transspeed && AnimationManager::animations[animTarget].attack != 2 && AnimationManager::animations[animTarget].attack != 3) {
				if (!isRun() || !wasRun()) {
					if (targetFrame().speed > currentFrame().speed) {
						target += dt * targetFrame().speed * speed * 2;
					}
					if (targetFrame().speed <= currentFrame().speed) {
						target += dt * currentFrame().speed * speed * 2;
					}
				}
				if (isRun() && wasRun()) {
					float tempspeed;
					tempspeed = velspeed;
					if (tempspeed < 10 * speedmult) {
						tempspeed = 10 * speedmult;
					}
					/* FIXME - mixed of target and current here, is that intended? */
					target += dt * AnimationManager::animations[animTarget].frames[frameCurrent].speed * speed * 1.7 * tempspeed / (speed * 45 * scale);
				}
			}
			else if (transspeed) {
				target += dt * transspeed * speed * 2;
			}
			else {
				if (!isRun() || !wasRun()) {
					if (targetFrame().speed > currentFrame().speed) {
						target += dt * targetFrame().speed * 2;
					}
					if (targetFrame().speed <= currentFrame().speed) {
						target += dt * currentFrame().speed * 2;
					}
				}
			}

			if (animCurrent != animTarget) {
				target = (target + oldtarget) / 2;
			}

			if (target > 1) {
				frameCurrent = frameTarget;
				target = 1;
			}

			if (frameCurrent >= int(AnimationManager::animations[animCurrent].frames.size())) {
				frameCurrent = AnimationManager::animations[animCurrent].frames.size() - 1;
			}

			oldrot = rot;
			rot = targetrot * target;
			yaw += rot - oldrot;
			if (target == 1) {
				rot = 0;
				oldrot = 0;
				targetrot = 0;
			}

			if (animCurrent != oldanimCurrent || animTarget != oldanimTarget || ((frameCurrent != oldframeCurrent || frameTarget != oldframeTarget) && !calcrot)) {
				//Old rotates
				for (unsigned i = 0; i < skeleton.m_joints.size(); i++) {
					skeleton.m_joints[i].position = currentFrame().joints[i].position;					
				}

				skeleton.findForwards();

				for (unsigned i = 0; i < skeleton.m_muscles.size(); i++) {
					if (skeleton.m_muscles[i].visible) {
						skeleton.findRotationMuscle(i, animTarget);
					}
				}
				for (unsigned i = 0; i < skeleton.m_muscles.size(); i++) {
					if (skeleton.m_muscles[i].visible) {
						if (isnormal((float)((int)(skeleton.m_muscles[i].rotate1 * 100) % 36000) / 100)) {
							skeleton.m_muscles[i].oldrotate1 = (float)((int)(skeleton.m_muscles[i].rotate1 * 100) % 36000) / 100;
						}
						if (isnormal((float)((int)(skeleton.m_muscles[i].rotate2 * 100) % 36000) / 100)) {
							skeleton.m_muscles[i].oldrotate2 = (float)((int)(skeleton.m_muscles[i].rotate2 * 100) % 36000) / 100;
						}
						if (isnormal((float)((int)(skeleton.m_muscles[i].rotate3 * 100) % 36000) / 100)) {
							skeleton.m_muscles[i].oldrotate3 = (float)((int)(skeleton.m_muscles[i].rotate3 * 100) % 36000) / 100;
						}
					}
				}

				//New rotates
				for (unsigned i = 0; i < skeleton.m_joints.size(); i++) {
					skeleton.m_joints[i].position = currentFrame().joints[i].position;					
				}

				skeleton.findForwards();

				for (unsigned i = 0; i < skeleton.m_muscles.size(); i++) {
					if (skeleton.m_muscles[i].visible) {
						skeleton.findRotationMuscle(i, animTarget);
					}
				}
				for (unsigned i = 0; i < skeleton.m_muscles.size(); i++) {
					if (skeleton.m_muscles[i].visible) {
						if (isnormal((float)((int)(skeleton.m_muscles[i].rotate1 * 100) % 36000) / 100)) {
							skeleton.m_muscles[i].newrotate1 = (float)((int)(skeleton.m_muscles[i].rotate1 * 100) % 36000) / 100;
						}
						if (isnormal((float)((int)(skeleton.m_muscles[i].rotate2 * 100) % 36000) / 100)) {
							skeleton.m_muscles[i].newrotate2 = (float)((int)(skeleton.m_muscles[i].rotate2 * 100) % 36000) / 100;
						}
						if (isnormal((float)((int)(skeleton.m_muscles[i].rotate3 * 100) % 36000) / 100)) {
							skeleton.m_muscles[i].newrotate3 = (float)((int)(skeleton.m_muscles[i].rotate3 * 100) % 36000) / 100;
						}
						if (skeleton.m_muscles[i].newrotate3 > skeleton.m_muscles[i].oldrotate3 + 180) {
							skeleton.m_muscles[i].newrotate3 -= 360;
						}
						if (skeleton.m_muscles[i].newrotate3 < skeleton.m_muscles[i].oldrotate3 - 180) {
							skeleton.m_muscles[i].newrotate3 += 360;
						}
						if (skeleton.m_muscles[i].newrotate2 > skeleton.m_muscles[i].oldrotate2 + 180) {
							skeleton.m_muscles[i].newrotate2 -= 360;
						}
						if (skeleton.m_muscles[i].newrotate2 < skeleton.m_muscles[i].oldrotate2 - 180) {
							skeleton.m_muscles[i].newrotate2 += 360;
						}
						if (skeleton.m_muscles[i].newrotate1 > skeleton.m_muscles[i].oldrotate1 + 180) {
							skeleton.m_muscles[i].newrotate1 -= 360;
						}
						if (skeleton.m_muscles[i].newrotate1 < skeleton.m_muscles[i].oldrotate1 - 180) {
							skeleton.m_muscles[i].newrotate1 += 360;
						}
					}
				}
			}

			oldanimCurrent = animCurrent;
			oldanimTarget = animTarget;
			oldframeTarget = frameTarget;
			oldframeCurrent = frameCurrent;

			for (unsigned i = 0; i < skeleton.m_joints.size(); i++) {
				skeleton.m_joints[i].velocity = (currentFrame().joints[i].position * (1 - target) + targetFrame().joints[i].position * target - skeleton.m_joints[i].position) / dt;
				skeleton.m_joints[i].position = currentFrame().joints[i].position * (1 - target) + targetFrame().joints[i].position * target;
			}
			offset = currentoffset * (1 - target) + targetoffset * target;
			for (unsigned i = 0; i < skeleton.m_muscles.size(); i++) {
				if (skeleton.m_muscles[i].visible) {
					skeleton.m_muscles[i].rotate1 = skeleton.m_muscles[i].oldrotate1 * (1 - target) + skeleton.m_muscles[i].newrotate1 * target;
					skeleton.m_muscles[i].rotate2 = skeleton.m_muscles[i].oldrotate2 * (1 - target) + skeleton.m_muscles[i].newrotate2 * target;
					skeleton.m_muscles[i].rotate3 = skeleton.m_muscles[i].oldrotate3 * (1 - target) + skeleton.m_muscles[i].newrotate3 * target;
				}
			}
		}

		if (isLanding() && landhard) {
			if (id == 0) {
				//camerashake += .4;
			}
			animTarget = getLandhard();
			frameTarget = 0;
			target = 0;
			landhard = 0;
			transspeed = 15;
		}
	}
}