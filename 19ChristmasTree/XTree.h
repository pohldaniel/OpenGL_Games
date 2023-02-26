#pragma once
#include <vector>

#include <GL/glew.h>
#include "engine/Vector.h"

class XTree {

public:

	//
	// Constructor and destructor
	//
	XTree();
	~XTree();

	//
	//  Functions used to generate the tree
	//
	void grow();
	void genLights();


	//
	//    Functions used to draw the different tree components
	//
	// These functions merely draw the geometry, shaders and textures
	// are handled outside the tree class.
	//
	void drawBranches();
	void drawFronds();
	void drawLights();
	void drawOrnaments();

	//
	//  Functions used to control generation parameters
	//

	void setLightCount(int n) { _numLights = n; }

	void clearLightColors() { _lightColors.clear(); }
	void addLightColor(float r, float g, float b) {
		_lightColors.push_back(r);
		_lightColors.push_back(g);
		_lightColors.push_back(b);
	}

	void clearOrnamentColors() { _ornamentColors.clear(); }
	void addOrnamentMaterial(float r, float g, float b, float shininess) {
		_ornamentColors.push_back(r);
		_ornamentColors.push_back(g);
		_ornamentColors.push_back(b);
		_ornamentColors.push_back(shininess);
	}

	void setSegmentCount(int s) { _numSegments = s; }
	void setHeight(float h) { _height = h; }
	void setRadius(float r) { _radius = r; }
	void setNumBranchSides(int n) { _branchSides = n; }
	void setBranchesPerSegment(int n) { _branchesPerSegment = n; }

	void setBranchGenerations(int gen) { _branchGenerations = gen; }
	void setBranchGenerationDecayRate(float r) { _branchGenerationDecayRate = r; }

private:

	//
	//  Function used for growing the fractal branch
	//
	void growBranch(const Vector3f& dir, const Vector3f& root, float size, int gen);

	//geometry for the trunk and branches
	std::vector<float> _trunkVertices;
	std::vector<GLuint> _trunkIndices;
	GLuint _trunkVBO;

	//geometry for the fronds
	std::vector<float> _frondVertices;
	std::vector<GLuint> _frondIndices;
	GLuint _frondVBO;

	//geometry for the lights
	std::vector<float> _lightVertices;
	GLuint _lightVBO;

	//geometry for ornaments
	std::vector<float> _ornamentVertices;
	GLuint _ornamentVBO;

	//generation parameters
	int _numLights;
	std::vector<float> _lightColors;
	std::vector<float> _ornamentColors;
	int _numSegments; //number of segments high the tree is (each segment has a trunk component and a set of branches
	float _height;
	float _radius;
	int _branchSides; //number of sides on the trunk and branches
	int _branchesPerSegment; //number of branches in a segment

	int _branchGenerations; //how many times a branch will branch, when it grows from the base of the tree
	float _branchGenerationDecayRate; // rate per-segment that the branchiness decays


	float _ornamentFreqAdjust;

	// static constants
	static const int trunkVertexSize;
	static const int frondVertexSize;
	static const int lightVertexSize;
	static const int ornamentVertexSize;

};