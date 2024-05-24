#include <iostream>
#include <random>
#include "ParticleSystem.h"

ParticleSystem::Particle::Particle(ParticleSystem* _system, const Vector3f& _pos, const Vector3f& _dir){
	
	pos = _pos;
	dir = _dir;

	system = _system;

	lifetime_sec = system->getLifetime();
	lifetime_sec_inv = system->getLifetime_inv();

	canBeRemoved = false;
	elapsed_sec = 0.0f;

	updateParameters(0.0f);
}

void ParticleSystem::Particle::updateParameters(float lrp){

	speed = system->Speed.getValue(lrp);
	size = system->Size.getValue(lrp);
	color = system->Color.getValue(lrp);
	alpha = system->Alpha.getValue(lrp);
}

void ParticleSystem::Particle::update(float dt){

	float elapsed_modified_by_speed = dt * speed;
	elapsed_sec += elapsed_modified_by_speed;
	canBeRemoved = elapsed_sec > lifetime_sec;

	if (!canBeRemoved){

		pos = pos + dir * elapsed_modified_by_speed;
		float lrp = elapsed_sec * lifetime_sec_inv;
		updateParameters(lrp);
	}
}

ParticleSystem::ParticleSystem() {
	loop = false;

	elapsed_sec = 0.0f;
	duration_sec = 1.0f;
	lifetime_sec = 1.0f;
	lifetime_sec_inv = 1.0f;
	rateOverTime = 1.0f;
	rateOverTime_inv = 1.0f;
	rateElapsed_sec = 0.0f;
	boxEmmiter;
	textureColor = Vector4f::ONE;
	soft = true;
}

ParticleSystem::~ParticleSystem() {

}

void ParticleSystem::update(float dt) {
	emissionPos = boxEmmiter.getPosition();
	emissionDir = Quaternion::Rotate(boxEmmiter.getOrientation(), Vector3f(0.0f, 0.0f, -1.0));

	elapsed_sec += dt;

	for (int i = particles.size() - 1; i >= 0; i--)
		particles[i].update(dt);

	for (int i = particles.size() - 1; i >= 0; i--){
		if (particles[i].canBeRemoved)
			particles.erase(particles.begin() + i);
	}

	//std::cout << "Time: " << elapsed_sec << std::endl;

	/*if (!loop && elapsed_sec > duration_sec){

		if (particles.size() == 0){
			
		}

		return;
	}*/
	
	rateElapsed_sec += dt;
	int emmissionToDo = (int)floor(rateElapsed_sec / rateOverTime_inv);
	rateElapsed_sec = rateElapsed_sec - (float)emmissionToDo * rateOverTime_inv;

	for (int i = emmissionToDo - 1; i >= 0; i--){
		Particle *p = emmitParticle();
		p->update(rateElapsed_sec + (float)i * rateOverTime_inv);
	}

	// rateElapsed_sec = 3.25
	// emissionToDo = 3
	//          p        p        p  t(0.25)
	// |--------|--------|--------|--------|--------|

	// 1 2 3 4 a b c
	for (int i = particles.size() - 1; i >= particles.size() - emmissionToDo; i--){
		if (particles[i].canBeRemoved)
			particles.erase(particles.begin() + i);
	}

	computeAABB();
}

void ParticleSystem::setLifetime(float v_sec){
	lifetime_sec = v_sec;
	lifetime_sec_inv = 1.0f / lifetime_sec;
}

float ParticleSystem::getLifetime() const{
	return lifetime_sec;
}

float ParticleSystem::getLifetime_inv() const{
	return lifetime_sec_inv;
}

void ParticleSystem::setRateOverTime(float v){
	rateOverTime = v;
	rateOverTime_inv = 1.0f / rateOverTime;
}

float ParticleSystem::getRand() {
	return (((float)rand()) / RAND_MAX) * 2.0f  -1.0f;
}

ParticleSystem::Particle* ParticleSystem::emmitParticle(){
	Vector3f pos, dir = Vector3f(0.0f, 0.0f, -1.0f);

	pos = { getRand() * 25.5f * 0.5f, getRand() * 10.05f * 0.5f, getRand() };
	pos = boxEmmiter.getPosition() + pos;

	dir = emissionDir;


	Particle p = Particle(this, pos, dir);
	particles.push_back(p);
	return &particles.back();
}

void ParticleSystem::emmitStart(){

	elapsed_sec = 0.0f;
	rateElapsed_sec = 0.0f;


	emissionPos = boxEmmiter.getPosition();
	emissionDir = Quaternion::Rotate(boxEmmiter.getOrientation(), Vector3f(0.0f, 0.0f, -1.0));
	emmitParticle();

	computeAABB();
}

void ParticleSystem::prewarmStart(){

	emissionPos = boxEmmiter.getPosition();
	emissionDir = Quaternion::Rotate(boxEmmiter.getOrientation(), Vector3f(0.0f, 0.0f, -1.0));

	elapsed_sec = 0.0f;
	rateElapsed_sec = 0.0f;

	elapsed_sec = duration_sec;
	rateElapsed_sec = duration_sec;
	int emmissionToDo = (int)floor(rateElapsed_sec / rateOverTime_inv);
	rateElapsed_sec = rateElapsed_sec - (float)emmissionToDo * rateOverTime_inv;

	for (int i = emmissionToDo - 1; i >= 0; i--){
		Particle *p = emmitParticle();
		p->update(rateElapsed_sec + (float)i * rateOverTime_inv);
	}

	for (int i = particles.size() - 1; i >= 0; i--){
		if (particles[i].canBeRemoved)			
			particles.erase(particles.begin() + i);
	}

	computeAABB();
}

bool __compare__particle__reverse__(const ParticleSystem::Particle &a, const ParticleSystem::Particle &b){
	return (a.distance_to_camera > b.distance_to_camera);
}

void ParticleSystem::sortPositions(const Vector3f& cameraPos, const Vector3f& cameraDirection){

	Vector3f aux;
	for (int i = particles.size() - 1; i >= 0; i--){
		aux = particles[i].pos - cameraPos;
		particles[i].distance_to_camera = Vector3f::Dot(aux, cameraDirection);
	}

	std::sort(particles.begin(), particles.end(), __compare__particle__reverse__);
}

void ParticleSystem::computeAABB(){

	if (particles.size() > 0){
		Particle *particle = &particles.back();
		Vector3f size = particle->size * 0.5f;
		aabb.set(particle->pos - size, particle->pos + size);
		for (int i = particles.size() - 2; i >= 0; i--){
			particle = &particles[i];
			size = particle->size * 0.5f;
			aabb.merge(particle->pos - size, particle->pos + size);
		}
		aabb_center = aabb.getCenter();
	}
}