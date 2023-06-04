#include <GL/glew.h>
#include "QuadTree.h"
#include "Constants.h"

Frustum QuadTree::Frustum;

AABB::AABB() {

}

AABB::~AABB() {

}

void AABB::set(const Vector3f& min, const Vector3f& max){

	vertices[0] = Vector3f(min[0], min[1], min[2]);
	vertices[1] = Vector3f(max[0], min[1], min[2]);
	vertices[2] = Vector3f(min[0], max[1], min[2]);
	vertices[3] = Vector3f(max[0], max[1], min[2]);
	vertices[4] = Vector3f(min[0], min[1], max[2]);
	vertices[5] = Vector3f(max[0], min[1], max[2]);
	vertices[6] = Vector3f(min[0], max[1], max[2]);
	vertices[7] = Vector3f(max[0], max[1], max[2]);
}

bool AABB::pointInside(const Vector3f &Point){

	if (Point[0] < vertices[0][0]) return false;
	if (Point[1] < vertices[0][1]) return false;
	if (Point[2] < vertices[0][2]) return false;

	if (Point[0] > vertices[7][0]) return false;
	if (Point[1] > vertices[7][1]) return false;
	if (Point[2] > vertices[7][2]) return false;

	return true;
}

bool AABB::visible(Frustum &frustum) {
	return frustum.aabbVisible(vertices);
}

float AABB::distance(Frustum &frustum) {
	return frustum.aabbDistance(vertices);
}

void AABB::draw(){

	glBegin(GL_LINES);

	glVertex3fv(&vertices[0][0]); glVertex3fv(&vertices[1][0]);
	glVertex3fv(&vertices[2][0]); glVertex3fv(&vertices[3][0]);
	glVertex3fv(&vertices[4][0]); glVertex3fv(&vertices[5][0]);
	glVertex3fv(&vertices[6][0]); glVertex3fv(&vertices[7][0]);

	glVertex3fv(&vertices[0][0]); glVertex3fv(&vertices[2][0]);
	glVertex3fv(&vertices[1][0]); glVertex3fv(&vertices[3][0]);
	glVertex3fv(&vertices[4][0]); glVertex3fv(&vertices[6][0]);
	glVertex3fv(&vertices[5][0]); glVertex3fv(&vertices[7][0]);

	glVertex3fv(&vertices[0][0]); glVertex3fv(&vertices[4][0]);
	glVertex3fv(&vertices[1][0]); glVertex3fv(&vertices[5][0]);
	glVertex3fv(&vertices[2][0]); glVertex3fv(&vertices[6][0]);
	glVertex3fv(&vertices[3][0]); glVertex3fv(&vertices[7][0]);

	glEnd();
}

// ----------------------------------------------------------------------------------------------------------------------------

TreeNode::TreeNode(){
	setDefaults();
}

TreeNode::~TreeNode(){

}

void TreeNode::setDefaults(){
	min = max = Vector3f(0.0f, 0.0f, 0.0f);
	depth = 0;
	indices = NULL;
	indicesCount = 0;
	indexBufferObject = 0;
	children[0] = NULL;
	children[1] = NULL;
}

void TreeNode::initAABB(const Vector3f& min, const Vector3f& max, int depth, float minAABBSize){

	this->min = min;
	this->max = max;

	this->depth = depth;

	Vector3f mid = (min + max) / 2.0f;
	Vector3f size = max - min;

	aabb.set(min, max);

	if (size[0] > minAABBSize || size[2] > minAABBSize) {

		children[0] = new TreeNode();
		children[1] = new TreeNode();

		if (size[0] >= size[2]){
			children[0]->initAABB(Vector3f(min[0], min[1], min[2]), Vector3f(mid[0], max[1], max[2]), depth + 1, minAABBSize);
			children[1]->initAABB(Vector3f(mid[0], min[1], min[2]), Vector3f(max[0], max[1], max[2]), depth + 1, minAABBSize);
		}else{
			children[0]->initAABB(Vector3f(min[0], min[1], min[2]), Vector3f(max[0], max[1], mid[2]), depth + 1, minAABBSize);
			children[1]->initAABB(Vector3f(min[0], min[1], mid[2]), Vector3f(max[0], max[1], max[2]), depth + 1, minAABBSize);
		}
	}
}

bool TreeNode::checkTriangle(Vector3f* vertices, unsigned int* indices, unsigned int a, unsigned int b, unsigned int c) {

	if (aabb.pointInside(vertices[indices[a]])) {

		if (aabb.pointInside(vertices[indices[b]])){

			if (aabb.pointInside(vertices[indices[c]])){

				bool belongsToAChild = false;

				if (children[0] != NULL){
					belongsToAChild |= children[0]->checkTriangle(vertices, indices, a, b, c);
				}

				if (children[1] != NULL && !belongsToAChild){
					belongsToAChild |= children[1]->checkTriangle(vertices, indices, a, b, c);
				}

				if (!belongsToAChild){
					indicesCount += 3;
				}

				return true;
			}
		}
	}

	return false;
}

void TreeNode::allocateMemory(){

	if (indicesCount > 0){
		indices = new int[indicesCount];
		indicesCount = 0;
	}

	if (children[0] != NULL){
		children[0]->allocateMemory();
	}

	if (children[1] != NULL){
		children[1]->allocateMemory();
	}
}

bool TreeNode::addTriangle(Vector3f* vertices, unsigned int* indices, unsigned int a, unsigned int b, unsigned int c) {
	if (aabb.pointInside(vertices[indices[a]])){

		if (aabb.pointInside(vertices[indices[b]])){

			if (aabb.pointInside(vertices[indices[c]])){

				bool belongsToAChild = false;

				if (children[0] != NULL){
					belongsToAChild |= children[0]->addTriangle(vertices, indices, a, b, c);
				}

				if (children[1] != NULL && !belongsToAChild){
					belongsToAChild |= children[1]->addTriangle(vertices, indices, a, b, c);
				}

				if (!belongsToAChild){

					this->indices[indicesCount++] = indices[a];
					this->indices[indicesCount++] = indices[b];
					this->indices[indicesCount++] = indices[c];
				}

				return true;
			}
		}
	}

	return false;
}

void TreeNode::resetAABB(Vector3f* vertices) {
	float minY = min[1], maxY = max[1];

	min[1] = maxY;
	max[1] = minY;

	if (indicesCount > 0) {

		for (int i = 0; i < indicesCount; i++){
			if (vertices[indices[i]][1] < min[1]) min[1] = vertices[indices[i]][1];
			if (vertices[indices[i]][1] > max[1]) max[1] = vertices[indices[i]][1];
		}
	}

	if (children[0] != NULL) {

		children[0]->resetAABB(vertices);

		if (children[0]->min[1] < min[1]) min[1] = children[0]->min[1];
		if (children[0]->max[1] > max[1]) max[1] = children[0]->max[1];
	}

	if (children[1] != NULL) {

		children[1]->resetAABB(vertices);

		if (children[1]->min[1] < min[1]) min[1] = children[1]->min[1];
		if (children[1]->max[1] > max[1]) max[1] = children[1]->max[1];
	}

	aabb.set(min, max);
}

int TreeNode::initIndexBufferObject() {

	int geometryNodesCount = 0;

	if (indicesCount > 0) {

		glGenBuffers(1, &indexBufferObject);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(int), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] indices;
		indices = NULL;

		geometryNodesCount++;
	}

	if (children[0] != NULL) {
		geometryNodesCount += children[0]->initIndexBufferObject();
	}

	if (children[1] != NULL) {
		geometryNodesCount += children[1]->initIndexBufferObject();
	}

	return geometryNodesCount;
}

int TreeNode::checkVisibility(TreeNode** visibleGeometryNodes, int& visibleGeometryNodesCount) {
	int trianglesRendered = 0;

	visible = aabb.visible(QuadTree::Frustum);
	if (visible) {

		if (indicesCount > 0){

			distance = aabb.distance(QuadTree::Frustum);
			visibleGeometryNodes[visibleGeometryNodesCount++] = this;

			trianglesRendered += indicesCount / 3;
		}

		if (children[0] != NULL) {
			trianglesRendered += children[0]->checkVisibility(visibleGeometryNodes, visibleGeometryNodesCount);
		}

		if (children[1] != NULL) {
			trianglesRendered += children[1]->checkVisibility(visibleGeometryNodes, visibleGeometryNodesCount);
		}
	}

	return trianglesRendered;
}

float TreeNode::getDistance() {
	return distance;
}

void TreeNode::draw() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TreeNode::drawAABB(int depth) {

	if (visible) {

		if (depth == -1 || depth == this->depth) {
			aabb.draw();
		}

		if (children[0] != NULL) {
			children[0]->drawAABB(depth);
		}

		if (children[1] != NULL) {
			children[1]->drawAABB(depth);
		}
	}
}

void TreeNode::destroy() {

	if (indices != NULL) {
		delete[] indices;
	}

	if (indexBufferObject != 0) {
		glDeleteBuffers(1, &indexBufferObject);
	}

	if (children[0] != NULL) {

		children[0]->destroy();
		delete children[0];
	}

	if (children[1] != NULL) {
		children[1]->destroy();
		delete children[1];
	}

	setDefaults();
}

// ----------------------------------------------------------------------------------------------------------------------------

QuadTree::QuadTree() {
	setDefaults();
	Frustum.init();
}

QuadTree::~QuadTree() {
	destroy();
}

void QuadTree::setDefaults() {
	root = NULL;
	visibleGeometryNodes = NULL;
}

void QuadTree::init(Vector3f* vertices, unsigned int* indices, unsigned int indicesCount, const Vector3f& min, const Vector3f& max, float minAABBSize) {
	destroy();

	if (vertices != NULL && indices != NULL && indicesCount > 0) {
		root = new TreeNode();

		root->initAABB(min, max, 0, minAABBSize);

		for (int i = 0; i < indicesCount; i += 3) {
			root->checkTriangle(vertices, indices, i, i + 1, i + 2);
		}

		root->allocateMemory();

		for (int i = 0; i < indicesCount; i += 3) {
			root->addTriangle(vertices, indices, i, i + 1, i + 2);
		}

		root->resetAABB(vertices);

		int geometryNodesCount = root->initIndexBufferObject();

		visibleGeometryNodes = new TreeNode*[geometryNodesCount];
	}
}

void QuadTree::quickSortVisibleGeometryNodes(int left, int right) {

	float pivot = visibleGeometryNodes[(left + right) / 2]->getDistance();
	int i = left, j = right;

	while (i <= j) {

		while (visibleGeometryNodes[i]->getDistance() < pivot) i++;
		while (visibleGeometryNodes[j]->getDistance() > pivot) j--;

		if (i <= j) {

			if (i != j) {

				TreeNode *temp = visibleGeometryNodes[i];
				visibleGeometryNodes[i] = visibleGeometryNodes[j];
				visibleGeometryNodes[j] = temp;
			}

			i++;
			j--;
		}
	}

	if (left < j) {
		quickSortVisibleGeometryNodes(left, j);
	}

	if (i < right) {
		quickSortVisibleGeometryNodes(i, right);
	}
}

int QuadTree::checkVisibility(bool sortVisibleGeometryNodes) {

	int trianglesRendered = 0;

	visibleGeometryNodesCount = 0;

	if (root != NULL) {

		trianglesRendered = root->checkVisibility(visibleGeometryNodes, visibleGeometryNodesCount);

		if (sortVisibleGeometryNodes) {

			if (visibleGeometryNodesCount > 1) {
				quickSortVisibleGeometryNodes(0, visibleGeometryNodesCount - 1);
			}
		}
	}

	return trianglesRendered;
}

void QuadTree::draw(bool visualizeRenderingOrder) const {

	if (visibleGeometryNodesCount > 0) {
		if (!visualizeRenderingOrder) {

			for (int i = 0; i < visibleGeometryNodesCount; i++) {			
				visibleGeometryNodes[i]->draw();
			}
		}else {

			for (int i = 0; i < visibleGeometryNodesCount; i++) {

				float Color = ((float)i / (float)visibleGeometryNodesCount) + 0.05f;
				auto shader = Globals::shaderManager.getAssetPointer("culling");
				shader->loadVector("color", Vector4f(Color, Color, Color, 1.0f));
				visibleGeometryNodes[i]->draw();
			}
		}
	}
}

void QuadTree::drawAABB(int Depth) {
	if (root != NULL) {
		root->drawAABB(Depth);
	}
}

void QuadTree::destroy() {
	if (root != NULL) {

		root->destroy();
		delete root;
	}

	if (visibleGeometryNodes != NULL) {

		delete[] visibleGeometryNodes;
	}

	setDefaults();
}