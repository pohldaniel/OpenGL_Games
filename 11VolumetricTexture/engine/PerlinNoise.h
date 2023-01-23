#include <vector>

// THIS CLASS IS A TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#define setup(i,b0,b1,r0,r1)\
        t = i + 4096;\
        b0 = ((int)t) & 255;\
        b1 = (b0+1) & 255;\
        r0 = t - (int)t;\
        r1 = r0 - 1.;
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )
#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )
class PerlinNoise {
	// The permutation vector
	std::vector<int> p;
public:
	// Initialize with the reference values for the permutation vector
	PerlinNoise();
	// Generate a new permutation vector based on the value of seed
	PerlinNoise(unsigned int seed);
	// Get a noise value, for 2D images z can have any value
	double noise(double x, double y, double z);
	double noise2(double x, double y, double z);

	double cloudNoise(double x, double y, double z, double alpha, double beta, int n);
private:

	int p2[256 + 256 + 2];
	double g3[256 + 256 + 2][3];
	double g2[256 + 256 + 2][2];
	double g1[256 + 256 + 2];



	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
	
	void normalize3(double v[2]);
	void normalize2(double v[3]);
};

#endif
