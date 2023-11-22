#include "XTree.h"
#include <iostream>
////////////////////////////////////////////////////////////////////////////////
//
//  Helper functions
//
////////////////////////////////////////////////////////////////////////////////

//
// Random floating point number [0-1]
//
//////////////////////////////////////////////////////////////////////
static float randfPos() {
	return float(rand()) / float(RAND_MAX);
}

//
// Random floating point number [-1 - 1]
//
//////////////////////////////////////////////////////////////////////
static float randf() {
	return randfPos() * 2.0f - 1.0f;
}

//
//  Random floating point number [-1 - 1] with the distribution
// clustered toward 0
//
//////////////////////////////////////////////////////////////////////
static float rand2f() {
	float val = randf();

	return (val >0.0f) ? val*val : -(val*val);
}

////////////////////////////////////////////////////////////////////////////////
//
// Tree constants
//
////////////////////////////////////////////////////////////////////////////////
const int XTree::trunkVertexSize = 6;
const int XTree::frondVertexSize = 14;
const int XTree::lightVertexSize = 6;
const int XTree::ornamentVertexSize = 7;

////////////////////////////////////////////////////////////////////////////////
//
// Tree functions
//
////////////////////////////////////////////////////////////////////////////////

//
//
//////////////////////////////////////////////////////////////////////
XTree::XTree() : _numLights(1200), _numSegments(40), _height(16.0f), _radius(6.0f), _branchSides(6), _branchesPerSegment(7), _branchGenerations(5), _branchGenerationDecayRate(1.0f / 7.0f) {
}

//
//
//////////////////////////////////////////////////////////////////////
XTree::~XTree() {
}

//
// Fractal function used to create branches and fronds
//
//  The branches on the tree are all self similar, so simple
// fractal with random perturbations. For each generation of the
// branch, it will grow out by 'size', then fork three ways,
// with the central branch continuing furthest, and closest to
// the original direction. The children will go off to the left
// and right.
//
//////////////////////////////////////////////////////////////////////
void XTree::growBranch(const Vector3f& dir, const Vector3f& root, float size, int gen) {
	Vector3f up(0.0f, 1.0f, 0.0f);
	Vector3f out = Vector3f::Normalize(Vector3f::Cross(dir, up));
	up = Vector3f::Normalize(Vector3f::Cross(out, dir));
	Vector3f pos = root + dir * size;


	if (gen > 0) {
		// if this is not the end of the branch lenghten it

		//save the current position in the vertex list
		GLuint baseIdx = (GLuint)_trunkVertices.size() / trunkVertexSize;

		// Compute random factors used to control how far out the next
		// genration will go. The power function is used to concentrate
		// the distribution.
		float outVal1 = 1.0f - powf(randfPos(), 4.0f);
		float outVal2 = 1.0f - powf(randfPos(), 4.0f);
		float outVal3 = 1.0f - powf(randfPos(), 4.0f);

		// create the vertices for this segment of the branch
		for (int ii = 0; ii < _branchSides; ii++) {
			float x = sinf(float(ii) / float(_branchSides) * 2.0f * 3.1415927f);
			float y = cosf(float(ii) / float(_branchSides) * 2.0f * 3.1415927f);
			Vector3f dir = Vector3f::Normalize(x * out + y * up);
			Vector3f base = root + dir * size * 0.035f;
			Vector3f tip = pos + dir * size * 0.015f;

			_trunkVertices.push_back(base[0]);
			_trunkVertices.push_back(base[1]);
			_trunkVertices.push_back(base[2]);
			_trunkVertices.push_back(dir[0]);
			_trunkVertices.push_back(dir[1]);
			_trunkVertices.push_back(dir[2]);
			_trunkVertices.push_back(tip[0]);
			_trunkVertices.push_back(tip[1]);
			_trunkVertices.push_back(tip[2]);
			_trunkVertices.push_back(dir[0]);
			_trunkVertices.push_back(dir[1]);
			_trunkVertices.push_back(dir[2]);

		}

		// add the indices for this portion of the branch
		for (int ii = 0; ii < _branchSides; ii++) {
			_trunkIndices.push_back(baseIdx + ii * 2);
			_trunkIndices.push_back(baseIdx + ii * 2 + 1);
			_trunkIndices.push_back(baseIdx + (ii * 2 + 2) % (_branchSides * 2));
			_trunkIndices.push_back(baseIdx + (ii * 2 + 2) % (_branchSides * 2));
			_trunkIndices.push_back(baseIdx + ii * 2 + 1);
			_trunkIndices.push_back(baseIdx + (ii * 2 + 3) % (_branchSides * 2));

		}

		float dirFactor = 1.0f;

		//this makes the fronds stick up a bit more
		if (gen == 1)
			dirFactor = 0.7f;

		// continue to grow the branch
		growBranch(Vector3f::Normalize(dir * dirFactor + 0.9f*out + 0.30f *up), pos, size * 0.5f * outVal1, gen - 1);
		growBranch(Vector3f::Normalize(dir * dirFactor + -0.9f*out + 0.22f *up), pos, size * 0.5f * outVal2, gen - 1);
		growBranch(Vector3f::Normalize(dir * dirFactor + 0.2f *up), pos, size * 0.6f * outVal3, gen - 1);
	} else {
		//
		// end of the branch, grow the frond
		//

		// save the current vertex position
		GLuint baseFrondIndex = (GLuint)_frondVertices.size() / frondVertexSize;

		// scale up the size for the frond
		size *= 6.0f;

		// frond is between 1 3/4 foot and 1/2 foot per side
		if (size > 1.75f)
			size = 1.75f;

		if (size < 0.5f)
			size = 0.5f;


		pos = root + dir * size; // scale by length
		float scale = size * 0.5f; // width scale is 1/2 length scale, because it is +/-

		// compute vectors needed to generate vertex and store
		Vector3f offset = Vector3f::Normalize(out * randf() + up * randf() * 0.5f);
		Vector3f n = Vector3f::Normalize(Vector3f::Cross(dir, offset));
		Vector3f sTan = offset;
		Vector3f tTan = dir;

		Vector3f vtx;

		// Compute the 4 vertices of the frond geometry and store them, along with
		// normal, texcoord, and tangents
		vtx = root - offset * scale;
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(vtx[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(n[ii]);
		_frondVertices.push_back(0.0f);
		_frondVertices.push_back(0.0f);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(sTan[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(tTan[ii]);

		vtx = root + offset * scale;
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(vtx[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(n[ii]);
		_frondVertices.push_back(1.0f);
		_frondVertices.push_back(0.0f);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(sTan[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(tTan[ii]);

		vtx = pos + offset * scale;
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(vtx[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(n[ii]);
		_frondVertices.push_back(1.0f);
		_frondVertices.push_back(1.0f);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(sTan[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(tTan[ii]);

		vtx = pos - offset * scale;
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(vtx[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(n[ii]);
		_frondVertices.push_back(0.0f);
		_frondVertices.push_back(1.0f);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(sTan[ii]);
		for (int ii = 0; ii < 3; ii++) _frondVertices.push_back(tTan[ii]);

		// add the indices for this frond to the index list
		_frondIndices.push_back(baseFrondIndex + 0);
		_frondIndices.push_back(baseFrondIndex + 1);
		_frondIndices.push_back(baseFrondIndex + 2);
		_frondIndices.push_back(baseFrondIndex + 0);
		_frondIndices.push_back(baseFrondIndex + 2);
		_frondIndices.push_back(baseFrondIndex + 3);

		//
		// randomly place an ornament at the end of the frond
		//
		const float materialCount = (float)_ornamentColors.size() * 0.25f;

		if (randfPos() < 0.15f * _ornamentFreqAdjust) {
			//add an ornament
			Vector3f oPos = root * 0.2f + pos * 0.8f + Vector3f(0.0f, -1.0f, 0.0f) * (size * 0.05f + 2.0f / 12.0f);
			int which = int(materialCount * randfPos());
			_ornamentVertices.push_back(oPos[0]);
			_ornamentVertices.push_back(oPos[1]);
			_ornamentVertices.push_back(oPos[2]);
			_ornamentVertices.push_back(_ornamentColors[4 * which]);
			_ornamentVertices.push_back(_ornamentColors[4 * which + 1]);
			_ornamentVertices.push_back(_ornamentColors[4 * which + 2]);
			_ornamentVertices.push_back(_ornamentColors[4 * which + 3]);

		}
	}
}

//
//  Function used to grow the trunk and fractal branches off of it
//
//////////////////////////////////////////////////////////////////////
void XTree::grow() {
	Vector3f segmentBase;
	float radius = _radius;

	//make sure we have an ornament color
	if (_ornamentColors.size() == 0) {
		//red 
		_ornamentColors.push_back(0.8f);
		_ornamentColors.push_back(0.0f);
		_ornamentColors.push_back(0.0f);
		_ornamentColors.push_back(32.0f);

		//green
		_ornamentColors.push_back(0.0f);
		_ornamentColors.push_back(0.8f);
		_ornamentColors.push_back(0.0f);
		_ornamentColors.push_back(32.0f);

		//gold
		_ornamentColors.push_back(0.8f);
		_ornamentColors.push_back(0.8f);
		_ornamentColors.push_back(0.0f);
		_ornamentColors.push_back(4.0f);
	}

	//grow the segments of the trunk
	for (int ii = 0; ii < _numSegments; ii++) {
		Vector3f segmentTop = segmentBase + Vector3f(0.0f, _height / float(_numSegments), 0.0f);

		//save the present vertex index
		GLuint baseIndex = (GLuint)_trunkVertices.size() / trunkVertexSize;

		// add the vertices for the trunk segment
		for (int jj = 0; jj < _branchSides; jj++) {
			float x = sinf(float(jj) / float(_branchSides) * 2.0f * 3.1415927f);
			float z = cosf(float(jj) / float(_branchSides) * 2.0f * 3.1415927f);
			Vector3f dir(x, 0.0f, z);

			Vector3f upper = segmentTop + dir * (radius - _radius / float(_numSegments - 2)) * 0.05f;
			Vector3f lower = segmentBase + dir * radius * 0.05f;

			_trunkVertices.push_back(lower[0]);
			_trunkVertices.push_back(lower[1]);
			_trunkVertices.push_back(lower[2]);
			_trunkVertices.push_back(dir[0]);
			_trunkVertices.push_back(dir[1]);
			_trunkVertices.push_back(dir[2]);
			_trunkVertices.push_back(upper[0]);
			_trunkVertices.push_back(upper[1]);
			_trunkVertices.push_back(upper[2]);
			_trunkVertices.push_back(dir[0]);
			_trunkVertices.push_back(dir[1]);
			_trunkVertices.push_back(dir[2]);
		}

		//
		// add the branches
		//

		// bias the rotation to void the limbs all lining up directly over one another
		float rotBias = randf();

		//create the individual branches
		for (int jj = 0; jj < _branchesPerSegment; jj++) {
			float x = sinf(float(jj) / float(_branchesPerSegment) * 2.0f * 3.1415927f + rotBias);
			float z = cosf(float(jj) / float(_branchesPerSegment) * 2.0f * 3.1415927f + rotBias);

			// direction of growth
			Vector3f dir(x, 0.0f, z);

			// values used to add some random behavior to the branches
			Vector3f tweak(rand2f(), randfPos() + 0.25f * (ii / float(_numSegments - 1)), rand2f());
			float val = randfPos();
			Vector3f tweakUp(0.0f, val*val, 0.0f);

			// place a scale factor on ornament count to account for the varying number of branches
			_ornamentFreqAdjust = sqrtf(1.0f / powf(3.0, (float)int(_branchGenerations - ii * _branchGenerationDecayRate + 0.5f)));

			//grow the fractal branch
			growBranch(dir + tweak * 0.1f, segmentBase + tweakUp, radius / 2.25f, int(_branchGenerations - ii * _branchGenerationDecayRate + 0.5f));
		}

		// add the indices for the trunk
		for (int jj = 0; jj < _branchSides; jj++) {
			_trunkIndices.push_back(baseIndex + jj * 2);
			_trunkIndices.push_back(baseIndex + jj * 2 + 1);
			_trunkIndices.push_back(baseIndex + (jj * 2 + 2) % (_branchSides * 2));
			_trunkIndices.push_back(baseIndex + (jj * 2 + 2) % (_branchSides * 2));
			_trunkIndices.push_back(baseIndex + jj * 2 + 1);
			_trunkIndices.push_back(baseIndex + (jj * 2 + 3) % (_branchSides * 2));

		}

		// update values for the next iteration
		segmentBase = segmentTop;

		radius -= _radius / float(_numSegments - 1);
	}

	glGenBuffers(1, &_frondVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _frondVBO);
	glBufferData(GL_ARRAY_BUFFER, _frondVertices.size() * sizeof(float), &_frondVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_trunkVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _trunkVBO);
	glBufferData(GL_ARRAY_BUFFER, _trunkVertices.size() * sizeof(float), &_trunkVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &_ornamentVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _ornamentVBO);
	glBufferData(GL_ARRAY_BUFFER, _ornamentVertices.size() * sizeof(float), &_ornamentVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//
// Function for generating tree lights
//
//  This function places lights randomly in the cone of the tree.
// The lights are generated within a prism, and culled if they are
// not in the are of interest to ensure equal spacing. Finally,
// lights that are too far into the tree are culled. 
//
//////////////////////////////////////////////////////////////////////
void XTree::genLights() {

	// 
	// if no lights are set, default to white lights (not pure white, but warmer)
	//
	if (_lightColors.size() == 0) {
		_lightColors.push_back(1.0f);
		_lightColors.push_back(0.98f);
		_lightColors.push_back(0.90f);
	}

	for (int ii = 0; ii < _numLights; ii++) {
		float x = randfPos() * _radius * 2.0f - _radius;
		float y = randfPos() * _height;
		float z = randfPos() * _radius * 2.0f - _radius;

		float h = y;
		float r = sqrtf(x*x + z*z);

		// make sure is is within a reasonable distance of the surface of the tree
		if (h < 1.1f || r >((_height - h) / _height * _radius * 1.02f) || r < ((_height - h) / _height * _radius - 2.0f)) {
			ii -= 1;
			continue;
		}

		int color = ii % int(_lightColors.size() / 3);
		_lightVertices.push_back(x);
		_lightVertices.push_back(y);
		_lightVertices.push_back(z);
		_lightVertices.push_back(_lightColors[color * 3]);
		_lightVertices.push_back(_lightColors[color * 3 + 1]);
		_lightVertices.push_back(_lightColors[color * 3 + 2]);
	}

	glGenBuffers(1, &_lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, _lightVBO);
	glBufferData(GL_ARRAY_BUFFER, _lightVertices.size() * sizeof(float), &_lightVertices[0], GL_STATIC_DRAW);

}

//
//
//////////////////////////////////////////////////////////////////////
void XTree::drawBranches() {

	float brown[] = { 0.5f, 0.5f, 0.0f, 1.0f };
	glColor3f(0.5f, 0.5f, 0.0f);

	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, brown);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

	glBindBuffer(GL_ARRAY_BUFFER, _trunkVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, trunkVertexSize * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, trunkVertexSize * sizeof(float), (void*)3);

	glDrawElements(GL_TRIANGLES, (GLsizei)_trunkIndices.size(), GL_UNSIGNED_INT, &_trunkIndices[0]);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_LIGHTING);
}

//
//
//////////////////////////////////////////////////////////////////////
void XTree::drawFronds() {

	//draw the fronds
	float green[] = { 0.0f, 0.7f, 0.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glColor3f(0.0f, 0.7f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

	glBindBuffer(GL_ARRAY_BUFFER, _frondVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, frondVertexSize * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, frondVertexSize * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, frondVertexSize * sizeof(float), (void*)(6 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, frondVertexSize * sizeof(float), (void*)(8 * sizeof(float)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, frondVertexSize * sizeof(float), (void*)(11 * sizeof(float)));


	glDrawElements(GL_TRIANGLES, (GLsizei)_frondIndices.size(), GL_UNSIGNED_INT, &_frondIndices[0]);

	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_LIGHTING);
}

//
//
//////////////////////////////////////////////////////////////////////
void XTree::drawLights() {

	glBindBuffer(GL_ARRAY_BUFFER, _lightVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, lightVertexSize * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, lightVertexSize * sizeof(float), (void*)(3 * sizeof(float)));

	glPointSize(3.0f);
	
	glDrawArrays(GL_POINTS, 0, (GLsizei)_lightVertices.size() / lightVertexSize);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

//
//
//////////////////////////////////////////////////////////////////////
void XTree::drawOrnaments() {
	
	glBindBuffer(GL_ARRAY_BUFFER, _ornamentVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, ornamentVertexSize * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, ornamentVertexSize * sizeof(float), (void*)(3 * sizeof(float)));

	glPointSize(5.0f);

	glDrawArrays(GL_POINTS, 0, (GLsizei)_ornamentVertices.size() / ornamentVertexSize);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}