#ifndef _FLAME_H_
#define _FLAME_H_
#include <vector>

#include <engine/Camera.h>
#include "Particle.h"

//definition of the maximum number of particles
#define NUMBEROFPARTICLES 1500

//KDTree class used to spatially partition the particles for faster interactions
//each instance is a node of the tree, containing references to its left and right nodes and a particle
//the class contains functions used in the generation, deletion and searching of the tree
class KDTree{

public:
	KDTree(int _depth, std::vector<Particle*> _particles);
	void destroyTree();
	void PrintTree(); //for debug only
	int findMedian(float _start, float _middle, float _end);
	void swap(std::vector<Particle*> _particles, int _index1, int _index2);
	void findParticlesInRange(float _range, const Vector3f& _position, std::vector<Particle*> &_particlesInRange, float* _largestDistanceInRange);

private:
	int m_depth;
	Particle* m_point;
	KDTree* m_left;
	KDTree* m_right;
};


//the flame class acts as a container and intermediary of the particles and KDTree
//it also contains its own VAO which it buffers particle positions to each frame and some other neccesary data such as a material
//it contains functions for updating all the particles and drawing the flame

class Flame{

public:
	Flame();
	~Flame();
	void update(float dt);
	void draw();

private:
	KDTree* m_root;
	std::vector<Particle*> m_particles;
	std::vector<Particle*> m_particlesInRange;
	unsigned int m_VAO;
	unsigned int m_posBuffer;
	float m_positions[NUMBEROFPARTICLES * 3];
	unsigned int m_numParticles;

};

#endif