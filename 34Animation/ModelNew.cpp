#include <GL/glew.h>
#include <engine/BuiltInShader.h>
#include "ModelNew.h"
#include "AnimationManager.h"

ModelNew::ModelNew() : animator(m_skeleton) {

	solidConverter.solidToBuffer("res/Body.solid", true, { 180.0f, 0.0f, 0.0f }, { 0.04f, 0.04f, 0.04f }, m_vertexBufferMap, m_indexBuffer);
	solidConverter.loadSkeleton("res/BasicFigure", m_vertexBufferMap, m_vertexBuffer, m_skeleton, m_weights, m_boneIds);

	m_vertexNum = m_vertexBuffer.size() / 5;
	m_muscleNum = m_skeleton.m_muscles.size();

	m_drawCount = m_indexBuffer.size();

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_skinMatrices = new Matrix4f[m_muscleNum];
	resetPose();
	
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_muscleNum, m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	buffer = new Vertex[m_vertexNum];

	glGenBuffers(3, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_vertexNum * sizeof(Vertex), &m_vertexBuffer[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(3 * sizeof(float)));

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

	bufferPtr = buffer;
}

void ModelNew::resetPose() {
	for (size_t i = 0; i < m_muscleNum; ++i) {
		m_skinMatrices[i] = m_skeleton.m_muscles[i].m_modelMatrixInitial;
	}
}

void ModelNew::draw() {

	for (unsigned int j = 0; j < m_muscleNum; j++) {
		Vector3f mid = (m_skeleton.m_joints[m_skeleton.m_muscles[j].parentIndex1].position + m_skeleton.m_joints[m_skeleton.m_muscles[j].parentIndex2].position) * 0.5f;
		m_skinMatrices[j] = Matrix4f::Translate(mid) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f - m_skeleton.m_muscles[j].rotate1) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f - m_skeleton.m_muscles[j].rotate2) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -m_skeleton.m_muscles[j].rotate3);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_muscleNum, m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	bufferPtr = buffer;
}

void ModelNew::update(float dt) {
	animator.doAnimations(dt);
}
////////////////////////////////////////////////////////////////////////////////
Animator::Animator(Utils::Skeleton& skeleton): skeleton(skeleton) {
	//animTarget = Utils::bounceidleanim;
	//animCurrent = Utils::bounceidleanim;
	animTarget = Utils::runanim;
	animCurrent = Utils::runanim;
	howactive = typeactive;
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
	return Utils::animation_bits[animTarget] & Utils::ab_idle;
}

bool Animator::isFlip() {
	return Utils::animation_bits[animTarget] & Utils::ab_flip;
}

bool Animator::wasFlip() {
	return Utils::animation_bits[animCurrent] & Utils::ab_flip;
}

bool Animator::wasLanding() {
	return Utils::animation_bits[animCurrent] & Utils::ab_land;
}

bool Animator::isLanding() {
	return Utils::animation_bits[animTarget] & Utils::ab_land;
}

bool Animator::wasLandhard() {
	return Utils::animation_bits[animCurrent] & Utils::ab_landhard;
}

bool Animator::isLandhard() {
	return Utils::animation_bits[animTarget] & Utils::ab_landhard;
}

bool Animator::wasCrouch() {
	return Utils::animation_bits[animCurrent] & Utils::ab_crouch;
}

bool Animator::isCrouch() {
	return Utils::animation_bits[animTarget] & Utils::ab_crouch;
}

bool Animator::hasWeapon() {
	return (weaponactive != -1);
}

bool Animator::wasRun() {
	return Utils::animation_bits[animCurrent] & Utils::ab_run;
}

bool Animator::isRun() {
	return Utils::animation_bits[animTarget] & Utils::ab_run;
}

int Animator::getRun() {
	if (superruntoggle && (!hasWeapon())) {
		return Utils::rabbitrunninganim;
	}else {
		return Utils::runanim;
	}
}

Utils::AnimationFrame& Animator::currentFrame() {
	return AnimationManager::animations.at(animCurrent).frames.at(frameCurrent);
}

Utils::AnimationFrame& Animator::targetFrame() {
	return AnimationManager::animations.at(animTarget).frames.at(frameTarget);
}

int Animator::getIdle() {
	
	if (howactive == typesitting) {
		return Utils::sitanim;
	}
	if (howactive == typesittingwall) {
		return Utils::sitwallanim;
	}
	if (howactive == typesleeping) {
		return Utils::sleepanim;
	}
	if (howactive == typedead1) {
		return Utils::dead1anim;
	}
	if (howactive == typedead2) {
		return Utils::dead2anim;
	}
	if (howactive == typedead3) {
		return Utils::dead3anim;
	}
	if (howactive == typedead4) {
		return Utils::dead4anim;
	}
	return Utils::bounceidleanim;
}

int Animator::getLanding() {
	return Utils::landanim;
}

int Animator::getLandhard() {
	return  Utils::landhardanim;
}

int Animator::getCrouch() {
	return Utils::crouchanim;
}

bool Animator::isPlayerControlled() {
	return (aitype == playercontrolled);
}

bool Animator::wasStop() {
	return Utils::animation_bits[animCurrent] & Utils::ab_stop;
}

bool Animator::isStop() {
	return Utils::animation_bits[animTarget] & Utils::ab_stop;
}

int Animator::getStop() {
	return Utils::stopanim;
}

bool Animator::wasIdle() {
	return Utils::animation_bits[animCurrent] & Utils::ab_idle;
}

void Animator::FootLand(bodypart whichfoot, float opacity) {

	if ((whichfoot != leftfoot) && (whichfoot != rightfoot)) {
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

		if (animTarget == Utils::tempanim || animCurrent == Utils::tempanim) {
			AnimationManager::animations[Utils::tempanim] = tempanimation;
		}
		if (animTarget == Utils::jumpupanim || animTarget == Utils::jumpdownanim || isFlip()) {
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

		if ((animCurrent == Utils::jumpupanim || animTarget == Utils::jumpdownanim) && !isFlip() && (!isLanding() && !isLandhard()) && ((crouchkeydown && !crouchtogglekeydown))) {
			Vector3f targfacing;
			targfacing = Vector3f::ZERO;
			targfacing[2] = 1;

			targfacing = Math::RotatePoint(targfacing, 0, targetyaw, 0);

			if (Vector3f::NormalDot(targfacing, velocity) >= -.3) {
				animTarget = Utils::flipanim;
			}
			else {
				animTarget = Utils::backflipanim;
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

		if (AnimationManager::animations[animTarget].attack || animCurrent == Utils::getupfrombackanim || animCurrent == Utils::getupfromfrontanim) {
			if (detail) {
				normalsupdatedelay = 0;
			}
		}

		if (target >= 1) {
			if (animTarget == Utils::rollanim && frameTarget == 3 && onfire) {
				onfire = false;
				//emit_sound_at(fireendsound, coords);
				//pause_sound(stream_firesound);
				deathbleeding = 0.0f;
			}

			if (animTarget == Utils::rabbittacklinganim && frameTarget == 1) {
				/*if (victim->aitype == attacktypecutoff && victim->stunned <= 0 && victim->surprised <= 0 && victim->id != 0) {
					Reverse();
				}
				if (animTarget == Utils::rabbittacklinganim && frameTarget == 1 && !victim->isCrouch() && victim->animTarget != Utils::backhandspringanim) {
					if (Vector3f::NormalDot(victim->facing, facing) > 0) {
						victim->animTarget = Utils::rabbittackledbackanim;
					}
					else {
						victim->animTarget = Utils::rabbittackledfrontanim;
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
				FootLand(leftfoot, 1);
				FootLand(rightfoot, 1);
			}

			transspeed = 0.0f;
			currentoffset = targetoffset;
			frameTarget = frameCurrent;
			animCurrent = animTarget;
			frameTarget++;

			/*if (animCurrent == Utils::removeknifeanim && currentFrame().label == 5) {
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

			/*if (animCurrent == Utils::drawleftanim && currentFrame().label == 5) {
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

			if ((animCurrent == Utils::walljumprightkickanim && animTarget == Utils::walljumprightkickanim) || (animCurrent == Utils::walljumpleftkickanim && animTarget == Utils::walljumpleftkickanim)) {
				Vector3f rotatetarget = Math::RotatePoint(skeleton.forward, 0, yaw, 0);
				Vector3f::Normalize(rotatetarget);
				targetyaw = -asin(0 - rotatetarget[0]);
				targetyaw *= 360 / 6.28;
				if (rotatetarget[2] < 0) {
					targetyaw = 180 - targetyaw;
				}

				if (animTarget == Utils::walljumprightkickanim) {
					targetyaw += 40;
				}
				if (animTarget == Utils::walljumpleftkickanim) {
					targetyaw -= 40;
				}
			}

			bool dojumpattack;
			dojumpattack = 0;
			if ((animTarget == Utils::rabbitrunninganim || animTarget == Utils::wolfrunninganim) && frameTarget == 3 && (jumpkeydown || attackkeydown || id != 0)) {
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
				if ((animTarget == Utils::rabbitrunninganim || animTarget == Utils::wolfrunninganim) && id == 0) {
					animTarget = Utils::rabbittackleanim;
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
					FootLand(leftfoot, 1);
					FootLand(rightfoot, 1);
				}
				if (animCurrent == Utils::rabbittackleanim || animCurrent == Utils::rabbittacklinganim) {
					animTarget = Utils::rollanim;
					frameTarget = 3;
					//emit_sound_at(movewhooshsound, coords, 128.);
				}
				if (animCurrent == Utils::staggerbackhighanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::staggerbackhardanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::removeknifeanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::crouchremoveknifeanim) {
					animTarget = getCrouch();
				}
				if (animCurrent == Utils::backhandspringanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::dodgebackanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::drawleftanim) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::drawrightanim || animCurrent == Utils::crouchdrawrightanim) {
					animTarget = getIdle();
					if (animCurrent == Utils::crouchdrawrightanim) {
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
				if (animCurrent == Utils::rollanim) {
					animTarget = getCrouch();
					FootLand(leftfoot, 1);
					FootLand(rightfoot, 1);
				}
				if (isFlip()) {
					if (animTarget == Utils::walljumprightkickanim) {
						targetrot = -190;
					}
					if (animTarget == Utils::walljumpleftkickanim) {
						targetrot = 190;
					}
					animTarget = Utils::jumpdownanim;
				}
				if (animCurrent == Utils::climbanim) {
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
				if (animTarget == Utils::rabbitkickreversalanim) {
					animTarget = getCrouch();
					lastfeint = false;
				}
				if (animTarget == Utils::jumpreversalanim) {
					animTarget = getCrouch();
					lastfeint = false;
				}
				if (animTarget == Utils::walljumprightanim || animTarget == Utils::walljumpbackanim || animTarget == Utils::walljumpfrontanim) {
					if (attackkeydown && animTarget != Utils::walljumpfrontanim) {
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
							animTarget = Utils::walljumprightkickanim;
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
					if (animTarget == Utils::walljumpbackanim) {
						animTarget = Utils::backflipanim;
						frameTarget = 3;
						velocity = facing * -8;
						velocity[1] = 4;
						if (id == 0) {
							//resume_stream(whooshsound);
						}
					}
					if (animTarget == Utils::walljumprightanim) {
						animTarget = Utils::rightflipanim;
						frameTarget = 4;
						targetyaw -= 90;
						yaw -= 90;
						velocity = Math::RotatePoint(facing, 0, 30, 0) * -8;
						velocity[1] = 4;
					}
					if (animTarget == Utils::walljumpfrontanim) {
						animTarget = Utils::frontflipanim;
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
				if (animTarget == Utils::walljumpleftanim) {
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
					if (animTarget != Utils::walljumpleftkickanim) {
						animTarget = Utils::leftflipanim;
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
				if (animTarget == Utils::sneakattackanim) {
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
				if (animTarget == Utils::knifesneakattackanim || animTarget == Utils::swordsneakattackanim) {
					animTarget = getIdle();
					frameTarget = 0;
					/*if (onterrain) {
						coords.y = terrain.getHeight(coords.x, coords.z);
					}*/

					lastfeint = 0;
				}
				if (animCurrent == Utils::knifefollowanim) {
					animTarget = getIdle();
					lastfeint = 0;
				}
				/*if (AnimationManager::animations[animTarget].attack == Utils::reversal && animCurrent != Utils::sneakattackanim && animCurrent != Utils::knifesneakattackanim && animCurrent != Utils::swordsneakattackanim && animCurrent != Utils::knifefollowanim) {
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
					if (animCurrent == Utils::spinkickreversalanim || animCurrent == Utils::swordslashreversalanim) {
						oldcoords = coords + facing * .5;
					}
					else if (animCurrent == Utils::sweepreversalanim) {
						oldcoords = coords + facing * 1.1;
					}
					else if (animCurrent == Utils::upunchreversalanim) {
						oldcoords = coords + facing * 1.5;
						targetyaw += 180;
						yaw += 180;
						targetheadyaw += 180;
						targettilt2 *= -1;
						tilt2 *= -1;
					}
					else if (animCurrent == Utils::knifeslashreversalanim) {
						oldcoords = coords + facing * .5;
						targetyaw += 90;
						yaw += 90;
						targetheadyaw += 90;
						targettilt2 = 0;
						tilt2 = 0;
					}
					else if (animCurrent == Utils::staffspinhitreversalanim) {
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
				if (animCurrent == Utils::knifesneakattackedanim || animCurrent == Utils::swordsneakattackedanim) {
					velocity = Vector3f::ZERO;
					velocity[1] = -5;
					//RagDoll(0);
				}
				if (AnimationManager::animations[animTarget].attack == Utils::reversed) {
					escapednum++;
					if (animTarget == Utils::sweepreversedanim) {
						targetyaw += 90;
					}
					animTarget = Utils::backhandspringanim;
					frameTarget = 2;
					//emit_sound_at(landsound, coords, 128);

					if (animCurrent == Utils::upunchreversedanim || animCurrent == Utils::swordslashreversedanim) {
						animTarget = Utils::rollanim;
						frameTarget = 5;
						//oldcoords = coords;
						//coords += (DoRotation(jointPos(leftfoot), 0, yaw, 0) + DoRotation(jointPos(rightfoot), 0, yaw, 0)) / 2 * scale;
						//coords.y = oldcoords.y;
					}
					if (animCurrent == Utils::knifeslashreversedanim) {
						animTarget = Utils::rollanim;
						frameTarget = 0;
						targetyaw += 90;
						yaw += 90;
						//oldcoords = coords;
						//coords += (DoRotation(jointPos(leftfoot), 0, yaw, 0) + DoRotation(jointPos(rightfoot), 0, yaw, 0)) / 2 * scale;
						//coords.y = oldcoords.y;
					}
				}
				if (wasFlip()) {
					animTarget = Utils::jumpdownanim;
				}
				if (wasLanding()) {
					animTarget = getIdle();
				}
				if (wasLandhard()) {
					animTarget = getIdle();
				}
				if (animCurrent == Utils::spinkickanim || animCurrent == Utils::getupfrombackanim || animCurrent == Utils::getupfromfrontanim || animCurrent == Utils::lowkickanim) {
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
				if (animCurrent == Utils::upunchanim) {
					animTarget = getStop();
					normalsupdatedelay = 0;
					lastfeint = 0;
				}
				if (animCurrent == Utils::rabbitkickanim && animTarget != Utils::backflipanim) {
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
				if (animCurrent == Utils::rabbitkickreversedanim) {
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
						animTarget = Utils::rollanim;
						//coords += facing;
						if (id == 0) {
							//pause_sound(whooshsound);
						}
					}
					lastfeint = 0;
				}
				if (animCurrent == Utils::rabbittackledbackanim || animCurrent == Utils::rabbittackledfrontanim) {
					velocity = Vector3f::ZERO;
					velocity[1] = -10;
					//RagDoll(0);
					skeleton.spinny = 0;
				}
				if (animCurrent == Utils::jumpreversedanim) {
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
						animTarget = Utils::rollanim;
						//coords += facing * 2;
						if (id == 0) {
							//pause_sound(whooshsound);
						}
					}
					lastfeint = 0;
				}

				/*if (AnimationManager::animations[animCurrent].attack == Utils::normalattack && !victim->skeleton.free && victim->animTarget != Utils::staggerbackhighanim && victim->animTarget != Utils::staggerbackhardanim && animTarget != Utils::winduppunchblockedanim && animTarget != Utils::blockhighleftanim && animTarget != Utils::swordslashparryanim && animTarget != Utils::swordslashparriedanim && animTarget != Utils::crouchstabanim && animTarget != Utils::swordgroundstabanim) {
					animTarget = getupfromfrontanim;
					lastfeint = 0;
				}
				else*/ if (AnimationManager::animations[animCurrent].attack == Utils::normalattack) {
					animTarget = getIdle();
					lastfeint = 0;
				}
				if (animCurrent == Utils::blockhighleftanim && !isPlayerControlled()) {
					animTarget = Utils::blockhighleftstrikeanim;
				}
				if (animCurrent == Utils::knifeslashstartanim || animCurrent == Utils::knifethrowanim || animCurrent == Utils::swordslashanim || animCurrent == Utils::staffhitanim || animCurrent == Utils::staffgroundsmashanim || animCurrent == Utils::staffspinhitanim) {
					animTarget = getIdle();
					lastfeint = 0;
				}
				/*if (animCurrent == Utils::spinkickanim && victim->skeleton.free) {
					if (creature == rabbittype) {
						animTarget = Utils::fightidleanim;
					}
				}*/
			}
			target = 0;

			if (isIdle() && !wasIdle()) {
				normalsupdatedelay = 0;
			}

			if (animCurrent == Utils::jumpupanim && velocity[1] < 0 && !isFlip()) {
				animTarget = Utils::jumpdownanim;
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
					//m_skeleton.joints[i].position.x = currentFrame().joints[i].position[0];
					//m_skeleton.joints[i].position.y = currentFrame().joints[i].position[1];
					//m_skeleton.joints[i].position.z = currentFrame().joints[i].position[2];
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
					//m_skeleton.joints[i].position.x = currentFrame().joints[i].position[0];
					//m_skeleton.joints[i].position.y = currentFrame().joints[i].position[1];
					//m_skeleton.joints[i].position.z = currentFrame().joints[i].position[2];
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
				Vector3f vel = currentFrame().joints[i].position * (1 - target) + targetFrame().joints[i].position * target;
				skeleton.m_joints[i].velocity[0] = vel[0];
				skeleton.m_joints[i].velocity[1] = vel[1];
				skeleton.m_joints[i].velocity[2] = vel[2];

				skeleton.m_joints[i].velocity -= skeleton.m_joints[i].position;
				skeleton.m_joints[i].velocity /= dt;

				Vector3f pos = currentFrame().joints[i].position * (1 - target) + targetFrame().joints[i].position * target;
				skeleton.m_joints[i].position[0] = pos[0];
				skeleton.m_joints[i].position[1] = pos[1];
				skeleton.m_joints[i].position[2] = pos[2];
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