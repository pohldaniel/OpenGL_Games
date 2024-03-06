#include "Model.h"
#include "Utils/Folders.hpp"
#include <iostream>

extern float multiplier;
extern float viewdistance;
extern XYZ viewer;
extern float fadestart;
extern float texdetail;
extern bool decalstoggle;

int Model::LineCheck(XYZ* p1, XYZ* p2, XYZ* p, XYZ* move, float* rotate)
{
	static float distance;
	static float olddistance;
	static int intersecting;
	static int firstintersecting;
	static XYZ point;

	*p1 = *p1 - *move;
	*p2 = *p2 - *move;
	if (*rotate) {
		*p1 = DoRotation(*p1, 0, -*rotate, 0);
	}
	if (*rotate) {
		*p2 = DoRotation(*p2, 0, -*rotate, 0);
	}
	if (!sphere_line_intersection(p1, p2, &boundingspherecenter, &boundingsphereradius)) {
		return -1;
	}
	firstintersecting = -1;

	for (unsigned int j = 0; j < Triangles.size(); j++) {
		intersecting = LineFacetd(p1, p2, &vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]], &Triangles[j].facenormal, &point);
		distance = (point.x - p1->x) * (point.x - p1->x) + (point.y - p1->y) * (point.y - p1->y) + (point.z - p1->z) * (point.z - p1->z);
		if ((distance < olddistance || firstintersecting == -1) && intersecting) {
			olddistance = distance;
			firstintersecting = j;
			*p = point;
		}
	}

	if (*rotate) {
		*p = DoRotation(*p, 0, *rotate, 0);
	}
	*p = *p + *move;
	return firstintersecting;
}

int Model::LineCheckPossible(XYZ* p1, XYZ* p2, XYZ* p, XYZ* move, float* rotate)
{
	static float distance;
	static float olddistance;
	static int intersecting;
	static int firstintersecting;
	static XYZ point;

	*p1 = *p1 - *move;
	*p2 = *p2 - *move;
	if (!sphere_line_intersection(p1, p2, &boundingspherecenter, &boundingsphereradius)) {
		return -1;
	}
	firstintersecting = -1;
	if (*rotate) {
		*p1 = DoRotation(*p1, 0, -*rotate, 0);
	}
	if (*rotate) {
		*p2 = DoRotation(*p2, 0, -*rotate, 0);
	}

	for (unsigned int j = 0; j < possible.size(); j++) {
		if (possible[j] < Triangles.size()) {
			intersecting = LineFacetd(p1, p2, &vertex[Triangles[possible[j]].vertex[0]], &vertex[Triangles[possible[j]].vertex[1]], &vertex[Triangles[possible[j]].vertex[2]], &Triangles[possible[j]].facenormal, &point);
			distance = (point.x - p1->x) * (point.x - p1->x) + (point.y - p1->y) * (point.y - p1->y) + (point.z - p1->z) * (point.z - p1->z);
			if ((distance < olddistance || firstintersecting == -1) && intersecting) {
				olddistance = distance;
				firstintersecting = possible[j];
				*p = point;
			}
		}
	}

	if (*rotate) {
		*p = DoRotation(*p, 0, *rotate, 0);
	}
	*p = *p + *move;
	return firstintersecting;
}

int Model::LineCheckSlidePossible(XYZ* p1, XYZ* p2, XYZ* move, float* rotate)
{
	static float distance;
	static float olddistance;
	static int intersecting;
	static int firstintersecting;
	static XYZ point;

	*p1 = *p1 - *move;
	*p2 = *p2 - *move;
	if (!sphere_line_intersection(p1, p2, &boundingspherecenter, &boundingsphereradius)) {
		return -1;
	}
	firstintersecting = -1;
	if (*rotate) {
		*p1 = DoRotation(*p1, 0, -*rotate, 0);
	}
	if (*rotate) {
		*p2 = DoRotation(*p2, 0, -*rotate, 0);
	}

	for (unsigned int j = 0; j < possible.size(); j++) {
		if (possible[j] < Triangles.size()) {
			intersecting = LineFacetd(p1, p2, &vertex[Triangles[possible[j]].vertex[0]], &vertex[Triangles[possible[j]].vertex[1]], &vertex[Triangles[possible[j]].vertex[2]], &Triangles[possible[j]].facenormal, &point);
			distance = (point.x - p1->x) * (point.x - p1->x) + (point.y - p1->y) * (point.y - p1->y) + (point.z - p1->z) * (point.z - p1->z);
			if ((distance < olddistance || firstintersecting == -1) && intersecting) {
				olddistance = distance;
				firstintersecting = possible[j];
			}
		}
	}

	if (firstintersecting > 0) {
		distance = abs((Triangles[firstintersecting].facenormal.x * p2->x) + (Triangles[firstintersecting].facenormal.y * p2->y) + (Triangles[firstintersecting].facenormal.z * p2->z) - ((Triangles[firstintersecting].facenormal.x * vertex[Triangles[firstintersecting].vertex[0]].x) + (Triangles[firstintersecting].facenormal.y * vertex[Triangles[firstintersecting].vertex[0]].y) + (Triangles[firstintersecting].facenormal.z * vertex[Triangles[firstintersecting].vertex[0]].z)));
		*p2 -= Triangles[firstintersecting].facenormal * distance;
	}

	if (*rotate) {
		*p2 = DoRotation(*p2, 0, *rotate, 0);
	}
	*p2 = *p2 + *move;
	return firstintersecting;
}

int Model::SphereCheck(XYZ* p1, float radius, XYZ* p, XYZ* move, float* rotate)
{
	static int i;
	static float distance;
	static float olddistance;
	static int intersecting;
	static int firstintersecting;
	static XYZ point;
	static XYZ oldp1;

	firstintersecting = -1;

	oldp1 = *p1;
	*p1 = *p1 - *move;
	if (*rotate) {
		*p1 = DoRotation(*p1, 0, -*rotate, 0);
	}
	if (distsq(p1, &boundingspherecenter) > radius * radius + boundingsphereradius * boundingsphereradius) {
		return -1;
	}

	for (i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < Triangles.size(); j++) {
			intersecting = 0;
			distance = abs((Triangles[j].facenormal.x * p1->x) + (Triangles[j].facenormal.y * p1->y) + (Triangles[j].facenormal.z * p1->z) - ((Triangles[j].facenormal.x * vertex[Triangles[j].vertex[0]].x) + (Triangles[j].facenormal.y * vertex[Triangles[j].vertex[0]].y) + (Triangles[j].facenormal.z * vertex[Triangles[j].vertex[0]].z)));
			if (distance < radius) {
				point = *p1 - Triangles[j].facenormal * distance;
				if (PointInTriangle(&point, Triangles[j].facenormal, &vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]])) {
					intersecting = 1;
				}
				if (!intersecting) {
					intersecting = sphere_line_intersection(&vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], p1, &radius);
				}
				if (!intersecting) {
					intersecting = sphere_line_intersection(&vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]], p1, &radius);
				}
				if (!intersecting) {
					intersecting = sphere_line_intersection(&vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[2]], p1, &radius);
				}
				if (intersecting) {
					*p1 += Triangles[j].facenormal * (distance - radius);
				}
			}
			if ((distance < olddistance || firstintersecting == -1) && intersecting) {
				olddistance = distance;
				firstintersecting = j;
				*p = point;
			}
		}
	}
	if (*rotate) {
		*p = DoRotation(*p, 0, *rotate, 0);
	}
	*p = *p + *move;
	if (*rotate) {
		*p1 = DoRotation(*p1, 0, *rotate, 0);
	}
	*p1 += *move;
	return firstintersecting;
}

int Model::SphereCheckPossible(XYZ* p1, float radius, XYZ* move, float* rotate)
{
	static float distance;
	static float olddistance;
	static int intersecting;
	static int firstintersecting;
	static XYZ point;
	static XYZ oldp1;

	firstintersecting = -1;

	oldp1 = *p1;
	*p1 = *p1 - *move;

	possible.clear();

	if (*rotate) {
		*p1 = DoRotation(*p1, 0, -*rotate, 0);
	}
	if (distsq(p1, &boundingspherecenter) > radius * radius + boundingsphereradius * boundingsphereradius) {
		*p1 = oldp1;
		return -1;
	}

	for (unsigned int j = 0; j < Triangles.size(); j++) {
		intersecting = 0;
		distance = abs((Triangles[j].facenormal.x * p1->x) + (Triangles[j].facenormal.y * p1->y) + (Triangles[j].facenormal.z * p1->z) - ((Triangles[j].facenormal.x * vertex[Triangles[j].vertex[0]].x) + (Triangles[j].facenormal.y * vertex[Triangles[j].vertex[0]].y) + (Triangles[j].facenormal.z * vertex[Triangles[j].vertex[0]].z)));
		if (distance < radius) {
			point = *p1 - Triangles[j].facenormal * distance;
			if (PointInTriangle(&point, Triangles[j].facenormal, &vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]])) {
				intersecting = 1;
			}
			if (!intersecting) {
				intersecting = sphere_line_intersection(&vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[1]], p1, &radius);
			}
			if (!intersecting) {
				intersecting = sphere_line_intersection(&vertex[Triangles[j].vertex[1]], &vertex[Triangles[j].vertex[2]], p1, &radius);
			}
			if (!intersecting) {
				intersecting = sphere_line_intersection(&vertex[Triangles[j].vertex[0]], &vertex[Triangles[j].vertex[2]], p1, &radius);
			}
			if (intersecting) {
				possible.push_back(j);
			}
		}
		if ((distance < olddistance || firstintersecting == -1) && intersecting) {
			olddistance = distance;
			firstintersecting = j;
		}
	}
	if (*rotate) {
		*p1 = DoRotation(*p1, 0, *rotate, 0);
	}
	*p1 += *move;

	return firstintersecting;
}

void Model::UpdateVertexArray()
{
	if (type != normaltype && type != decalstype) {
		return;
	}

	if (flat) {
		for (unsigned int i = 0; i < Triangles.size(); i++) {
			unsigned int j = i * 24;
			vArray[j + 0] = Triangles[i].gx[0];
			vArray[j + 1] = Triangles[i].gy[0];
			vArray[j + 2] = Triangles[i].facenormal.x * -1;
			vArray[j + 3] = Triangles[i].facenormal.y * -1;
			vArray[j + 4] = Triangles[i].facenormal.z * -1;
			vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
			vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
			vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

			vArray[j + 8] = Triangles[i].gx[1];
			vArray[j + 9] = Triangles[i].gy[1];
			vArray[j + 10] = Triangles[i].facenormal.x * -1;
			vArray[j + 11] = Triangles[i].facenormal.y * -1;
			vArray[j + 12] = Triangles[i].facenormal.z * -1;
			vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
			vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
			vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

			vArray[j + 16] = Triangles[i].gx[2];
			vArray[j + 17] = Triangles[i].gy[2];
			vArray[j + 18] = Triangles[i].facenormal.x * -1;
			vArray[j + 19] = Triangles[i].facenormal.y * -1;
			vArray[j + 20] = Triangles[i].facenormal.z * -1;
			vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
			vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
			vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
		}
	}
	else {
		for (unsigned int i = 0; i < Triangles.size(); i++) {
			unsigned int j = i * 24;
			vArray[j + 0] = Triangles[i].gx[0];
			vArray[j + 1] = Triangles[i].gy[0];
			vArray[j + 2] = normals[Triangles[i].vertex[0]].x;
			vArray[j + 3] = normals[Triangles[i].vertex[0]].y;
			vArray[j + 4] = normals[Triangles[i].vertex[0]].z;
			vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
			vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
			vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

			vArray[j + 8] = Triangles[i].gx[1];
			vArray[j + 9] = Triangles[i].gy[1];
			vArray[j + 10] = normals[Triangles[i].vertex[1]].x;
			vArray[j + 11] = normals[Triangles[i].vertex[1]].y;
			vArray[j + 12] = normals[Triangles[i].vertex[1]].z;
			vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
			vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
			vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

			vArray[j + 16] = Triangles[i].gx[2];
			vArray[j + 17] = Triangles[i].gy[2];
			vArray[j + 18] = normals[Triangles[i].vertex[2]].x;
			vArray[j + 19] = normals[Triangles[i].vertex[2]].y;
			vArray[j + 20] = normals[Triangles[i].vertex[2]].z;
			vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
			vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
			vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
		}
	}
}

void Model::UpdateVertexArrayNoTex()
{
	if (type != normaltype && type != decalstype) {
		return;
	}

	if (flat) {
		for (unsigned int i = 0; i < Triangles.size(); i++) {
			unsigned int j = i * 24;
			vArray[j + 2] = Triangles[i].facenormal.x * -1;
			vArray[j + 3] = Triangles[i].facenormal.y * -1;
			vArray[j + 4] = Triangles[i].facenormal.z * -1;
			vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
			vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
			vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

			vArray[j + 10] = Triangles[i].facenormal.x * -1;
			vArray[j + 11] = Triangles[i].facenormal.y * -1;
			vArray[j + 12] = Triangles[i].facenormal.z * -1;
			vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
			vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
			vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

			vArray[j + 18] = Triangles[i].facenormal.x * -1;
			vArray[j + 19] = Triangles[i].facenormal.y * -1;
			vArray[j + 20] = Triangles[i].facenormal.z * -1;
			vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
			vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
			vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
		}
	}
	else {
		for (unsigned int i = 0; i < Triangles.size(); i++) {
			unsigned int j = i * 24;
			vArray[j + 2] = normals[Triangles[i].vertex[0]].x;
			vArray[j + 3] = normals[Triangles[i].vertex[0]].y;
			vArray[j + 4] = normals[Triangles[i].vertex[0]].z;
			vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
			vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
			vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

			vArray[j + 10] = normals[Triangles[i].vertex[1]].x;
			vArray[j + 11] = normals[Triangles[i].vertex[1]].y;
			vArray[j + 12] = normals[Triangles[i].vertex[1]].z;
			vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
			vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
			vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

			vArray[j + 18] = normals[Triangles[i].vertex[2]].x;
			vArray[j + 19] = normals[Triangles[i].vertex[2]].y;
			vArray[j + 20] = normals[Triangles[i].vertex[2]].z;
			vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
			vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
			vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
		}
	}
}

void Model::UpdateVertexArrayNoTexNoNorm()
{
	if (type != normaltype && type != decalstype) {
		return;
	}

	for (unsigned int i = 0; i < Triangles.size(); i++) {
		unsigned int j = i * 24;
		vArray[j + 5] = vertex[Triangles[i].vertex[0]].x;
		vArray[j + 6] = vertex[Triangles[i].vertex[0]].y;
		vArray[j + 7] = vertex[Triangles[i].vertex[0]].z;

		vArray[j + 13] = vertex[Triangles[i].vertex[1]].x;
		vArray[j + 14] = vertex[Triangles[i].vertex[1]].y;
		vArray[j + 15] = vertex[Triangles[i].vertex[1]].z;

		vArray[j + 21] = vertex[Triangles[i].vertex[2]].x;
		vArray[j + 22] = vertex[Triangles[i].vertex[2]].y;
		vArray[j + 23] = vertex[Triangles[i].vertex[2]].z;
	}
}

bool Model::loadnotex(const std::string& filename)
{
	FILE* tfile;
	long i;
	short triangleNum;

	type = notextype;
	color = 0;

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(filename), "rb");

	// read model settings

	fseek(tfile, 0, SEEK_SET);
	funpackf(tfile, "Bs Bs", &vertexNum, &triangleNum);

	// read the model data
	deallocate();

	possible.clear();

	owner = (int*)malloc(sizeof(int) * vertexNum);
	vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
	Triangles.resize(triangleNum);
	vArray = (GLfloat*)malloc(sizeof(GLfloat) * triangleNum * 24);

	for (i = 0; i < vertexNum; i++) {
		funpackf(tfile, "Bf Bf Bf", &vertex[i].x, &vertex[i].y, &vertex[i].z);
	}

	for (i = 0; i < triangleNum; i++) {
		short vertex[6];
		funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[0], &vertex[1], &vertex[2], &vertex[3], &vertex[4], &vertex[5]);
		Triangles[i].vertex[0] = vertex[0];
		Triangles[i].vertex[1] = vertex[2];
		Triangles[i].vertex[2] = vertex[4];
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
	}

	fclose(tfile);

	UpdateVertexArray();

	for (i = 0; i < vertexNum; i++) {
		owner[i] = -1;
	}

	boundingsphereradius = 0;
	for (i = 0; i < vertexNum; i++) {
		for (int j = 0; j < vertexNum; j++) {
			if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
				boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
				boundingspherecenter = (vertex[i] + vertex[j]) / 2;
			}
		}
	}
	boundingsphereradius = fast_sqrt(boundingsphereradius);

	return true;
}

bool Model::load(const std::string& filename){
	FILE* tfile;
	long i;
	short triangleNum;

	type = normaltype;
	color = 0;

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(filename), "rb");

	// read model settings

	fseek(tfile, 0, SEEK_SET);
	funpackf(tfile, "Bs Bs", &vertexNum, &triangleNum);
	// read the model data
	deallocate();

	possible.clear();

	owner = (int*)malloc(sizeof(int) * vertexNum);
	vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
	normals = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
	Triangles.resize(triangleNum);
	vArray = (GLfloat*)malloc(sizeof(GLfloat) * triangleNum * 24);

	for (i = 0; i < vertexNum; i++) {
		funpackf(tfile, "Bf Bf Bf", &vertex[i].x, &vertex[i].y, &vertex[i].z);
	}

	for (i = 0; i < triangleNum; i++) {
		short vertex[6];
		funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[0], &vertex[1], &vertex[2], &vertex[3], &vertex[4], &vertex[5]);
		Triangles[i].vertex[0] = vertex[0];
		Triangles[i].vertex[1] = vertex[2];
		Triangles[i].vertex[2] = vertex[4];
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
	}

	modelTexture.xsz = 0;

	fclose(tfile);

	UpdateVertexArray();

	for (i = 0; i < vertexNum; i++) {
		owner[i] = -1;
	}

	static int j;
	boundingsphereradius = 0;
	for (i = 0; i < vertexNum; i++) {
		for (j = 0; j < vertexNum; j++) {
			if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
				boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
				boundingspherecenter = (vertex[i] + vertex[j]) / 2;
			}
		}
	}
	boundingsphereradius = fast_sqrt(boundingsphereradius);

	return true;
}

bool Model::loaddecal(const std::string& filename){
	FILE* tfile;
	long i, j;
	short triangleNum;

	type = decalstype;
	color = 0;

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(filename), "rb");

	// read model settings

	fseek(tfile, 0, SEEK_SET);
	funpackf(tfile, "Bs Bs", &vertexNum, &triangleNum);

	// read the model data

	deallocate();

	possible.clear();

	owner = (int*)malloc(sizeof(int) * vertexNum);
	vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
	normals = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
	Triangles.resize(triangleNum);
	vArray = (GLfloat*)malloc(sizeof(GLfloat) * triangleNum * 24);

	for (i = 0; i < vertexNum; i++) {
		funpackf(tfile, "Bf Bf Bf", &vertex[i].x, &vertex[i].y, &vertex[i].z);
	}

	for (i = 0; i < triangleNum; i++) {
		short vertex[6];
		funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[0], &vertex[1], &vertex[2], &vertex[3], &vertex[4], &vertex[5]);
		Triangles[i].vertex[0] = vertex[0];
		Triangles[i].vertex[1] = vertex[2];
		Triangles[i].vertex[2] = vertex[4];
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
	}

	modelTexture.xsz = 0;

	fclose(tfile);

	UpdateVertexArray();

	for (i = 0; i < vertexNum; i++) {
		owner[i] = -1;
	}

	boundingsphereradius = 0;
	for (i = 0; i < vertexNum; i++) {
		for (j = 0; j < vertexNum; j++) {
			if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
				boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
				boundingspherecenter = (vertex[i] + vertex[j]) / 2;
			}
		}
	}
	boundingsphereradius = fast_sqrt(boundingsphereradius);

	return true;
}

bool Model::loadraw(const std::string& filename){
	FILE* tfile;
	long i;
	short triangleNum;

	type = rawtype;
	color = 0;

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(filename), "rb");

	// read model settings

	fseek(tfile, 0, SEEK_SET);
	funpackf(tfile, "Bs Bs", &vertexNum, &triangleNum);

	// read the model data
	deallocate();

	possible.clear();

	owner = (int*)malloc(sizeof(int) * vertexNum);
	vertex = (XYZ*)malloc(sizeof(XYZ) * vertexNum);
	Triangles.resize(triangleNum);
	vArray = (GLfloat*)malloc(sizeof(GLfloat) * triangleNum * 24);

	for (i = 0; i < vertexNum; i++) {
		funpackf(tfile, "Bf Bf Bf", &vertex[i].x, &vertex[i].y, &vertex[i].z);
	}

	for (i = 0; i < triangleNum; i++) {
		short vertex[6];
		funpackf(tfile, "Bs Bs Bs Bs Bs Bs", &vertex[0], &vertex[1], &vertex[2], &vertex[3], &vertex[4], &vertex[5]);
		Triangles[i].vertex[0] = vertex[0];
		Triangles[i].vertex[1] = vertex[2];
		Triangles[i].vertex[2] = vertex[4];
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gx[0], &Triangles[i].gx[1], &Triangles[i].gx[2]);
		funpackf(tfile, "Bf Bf Bf", &Triangles[i].gy[0], &Triangles[i].gy[1], &Triangles[i].gy[2]);
	}

	fclose(tfile);

	for (i = 0; i < vertexNum; i++) {
		owner[i] = -1;
	}

	return true;
}

void Model::UniformTexCoords()
{
	for (unsigned int i = 0; i < Triangles.size(); i++) {
		Triangles[i].gy[0] = vertex[Triangles[i].vertex[0]].y;
		Triangles[i].gy[1] = vertex[Triangles[i].vertex[1]].y;
		Triangles[i].gy[2] = vertex[Triangles[i].vertex[2]].y;
		Triangles[i].gx[0] = vertex[Triangles[i].vertex[0]].x;
		Triangles[i].gx[1] = vertex[Triangles[i].vertex[1]].x;
		Triangles[i].gx[2] = vertex[Triangles[i].vertex[2]].x;
	}
	UpdateVertexArray();
}

void Model::FlipTexCoords()
{
	for (unsigned int i = 0; i < Triangles.size(); i++) {
		Triangles[i].gy[0] = -Triangles[i].gy[0];
		Triangles[i].gy[1] = -Triangles[i].gy[1];
		Triangles[i].gy[2] = -Triangles[i].gy[2];
	}
	UpdateVertexArray();
}

void Model::ScaleTexCoords(float howmuch)
{
	for (unsigned int i = 0; i < Triangles.size(); i++) {
		Triangles[i].gx[0] *= howmuch;
		Triangles[i].gx[1] *= howmuch;
		Triangles[i].gx[2] *= howmuch;
		Triangles[i].gy[0] *= howmuch;
		Triangles[i].gy[1] *= howmuch;
		Triangles[i].gy[2] *= howmuch;
	}
	UpdateVertexArray();
}

void Model::Scale(float xscale, float yscale, float zscale)
{
	static int i;
	for (i = 0; i < vertexNum; i++) {
		vertex[i].x *= xscale;
		vertex[i].y *= yscale;
		vertex[i].z *= zscale;
	}
	UpdateVertexArray();

	static int j;

	boundingsphereradius = 0;
	for (i = 0; i < vertexNum; i++) {
		for (j = 0; j < vertexNum; j++) {
			if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
				boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
				boundingspherecenter = (vertex[i] + vertex[j]) / 2;
			}
		}
	}
	boundingsphereradius = fast_sqrt(boundingsphereradius);
}

void Model::ScaleNormals(float xscale, float yscale, float zscale)
{
	if (type != normaltype && type != decalstype) {
		return;
	}

	for (int i = 0; i < vertexNum; i++) {
		normals[i].x *= xscale;
		normals[i].y *= yscale;
		normals[i].z *= zscale;
	}
	for (unsigned int i = 0; i < Triangles.size(); i++) {
		Triangles[i].facenormal.x *= xscale;
		Triangles[i].facenormal.y *= yscale;
		Triangles[i].facenormal.z *= zscale;
	}
	UpdateVertexArray();
}

void Model::Translate(float xtrans, float ytrans, float ztrans)
{
	static int i;
	for (i = 0; i < vertexNum; i++) {
		vertex[i].x += xtrans;
		vertex[i].y += ytrans;
		vertex[i].z += ztrans;
	}
	UpdateVertexArray();

	static int j;
	boundingsphereradius = 0;
	for (i = 0; i < vertexNum; i++) {
		for (j = 0; j < vertexNum; j++) {
			if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
				boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
				boundingspherecenter = (vertex[i] + vertex[j]) / 2;
			}
		}
	}
	boundingsphereradius = fast_sqrt(boundingsphereradius);
}

void Model::Rotate(float xang, float yang, float zang)
{
	static int i;
	for (i = 0; i < vertexNum; i++) {
		vertex[i] = DoRotation(vertex[i], xang, yang, zang);
	}
	UpdateVertexArray();

	static int j;
	boundingsphereradius = 0;
	for (i = 0; i < vertexNum; i++) {
		for (j = 0; j < vertexNum; j++) {
			if (j != i && distsq(&vertex[j], &vertex[i]) / 2 > boundingsphereradius) {
				boundingsphereradius = distsq(&vertex[j], &vertex[i]) / 2;
				boundingspherecenter = (vertex[i] + vertex[j]) / 2;
			}
		}
	}
	boundingsphereradius = fast_sqrt(boundingsphereradius);
}

void Model::CalculateNormals(bool facenormalise){

	if (type != normaltype && type != decalstype) {
		return;
	}

	for (int i = 0; i < vertexNum; i++) {
		normals[i].x = 0;
		normals[i].y = 0;
		normals[i].z = 0;
	}

	for (unsigned int i = 0; i < Triangles.size(); i++) {
		CrossProduct(vertex[Triangles[i].vertex[1]] - vertex[Triangles[i].vertex[0]], vertex[Triangles[i].vertex[2]] - vertex[Triangles[i].vertex[0]], &Triangles[i].facenormal);

		normals[Triangles[i].vertex[0]].x += Triangles[i].facenormal.x;
		normals[Triangles[i].vertex[0]].y += Triangles[i].facenormal.y;
		normals[Triangles[i].vertex[0]].z += Triangles[i].facenormal.z;

		normals[Triangles[i].vertex[1]].x += Triangles[i].facenormal.x;
		normals[Triangles[i].vertex[1]].y += Triangles[i].facenormal.y;
		normals[Triangles[i].vertex[1]].z += Triangles[i].facenormal.z;

		normals[Triangles[i].vertex[2]].x += Triangles[i].facenormal.x;
		normals[Triangles[i].vertex[2]].y += Triangles[i].facenormal.y;
		normals[Triangles[i].vertex[2]].z += Triangles[i].facenormal.z;
		if (facenormalise) {
			Normalise(&Triangles[i].facenormal);
		}
	}
	for (int i = 0; i < vertexNum; i++) {
		Normalise(&normals[i]);
		normals[i] *= -1;
	}
	UpdateVertexArrayNoTex();
}

void Model::drawimmediate(){
	//textureptr.bind();
	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < Triangles.size(); i++) {
		glTexCoord2f(Triangles[i].gx[0], Triangles[i].gy[0]);
		if (color) {
			glColor3f(normals[Triangles[i].vertex[0]].x, normals[Triangles[i].vertex[0]].y, normals[Triangles[i].vertex[0]].z);
		}
		else if (flat) {
			glNormal3f(Triangles[i].facenormal.x, Triangles[i].facenormal.y, Triangles[i].facenormal.y);
		}
		else {
			glNormal3f(normals[Triangles[i].vertex[0]].x, normals[Triangles[i].vertex[0]].y, normals[Triangles[i].vertex[0]].z);
		}
		glVertex3f(vertex[Triangles[i].vertex[0]].x, vertex[Triangles[i].vertex[0]].y, vertex[Triangles[i].vertex[0]].z);

		glTexCoord2f(Triangles[i].gx[1], Triangles[i].gy[1]);
		if (color) {
			glColor3f(normals[Triangles[i].vertex[1]].x, normals[Triangles[i].vertex[1]].y, normals[Triangles[i].vertex[1]].z);
		}
		else if (flat) {
			glNormal3f(Triangles[i].facenormal.x, Triangles[i].facenormal.y, Triangles[i].facenormal.y);
		}
		else {
			glNormal3f(normals[Triangles[i].vertex[1]].x, normals[Triangles[i].vertex[1]].y, normals[Triangles[i].vertex[1]].z);
		}
		glVertex3f(vertex[Triangles[i].vertex[1]].x, vertex[Triangles[i].vertex[1]].y, vertex[Triangles[i].vertex[1]].z);

		glTexCoord2f(Triangles[i].gx[2], Triangles[i].gy[2]);
		if (color) {
			glColor3f(normals[Triangles[i].vertex[2]].x, normals[Triangles[i].vertex[2]].y, normals[Triangles[i].vertex[2]].z);
		}
		else if (flat) {
			glNormal3f(Triangles[i].facenormal.x, Triangles[i].facenormal.y, Triangles[i].facenormal.y);
		}
		else {
			glNormal3f(normals[Triangles[i].vertex[2]].x, normals[Triangles[i].vertex[2]].y, normals[Triangles[i].vertex[2]].z);
		}
		glVertex3f(vertex[Triangles[i].vertex[2]].x, vertex[Triangles[i].vertex[2]].y, vertex[Triangles[i].vertex[2]].z);
	}
	glEnd();
}

void Model::draw(){

	if (type != normaltype && type != decalstype) {
		return;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (color) {
		glInterleavedArrays(GL_T2F_C3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);
	}
	else {
		glInterleavedArrays(GL_T2F_N3F_V3F, 8 * sizeof(GLfloat), &vArray[0]);
	}
	//textureptr.bind();

	//std::cout << vArray[0] << "  " << vArray[1] << "  " << vArray[2] << "  " << vArray[3] << "  " << vArray[4] << "  " << vArray[5] << "  " << vArray[6] << "  " << vArray[7] << std::endl;
	//std::cout << vArray[8] << "  " << vArray[9] << "  " << vArray[10] << "  " << vArray[11] << "  " << vArray[12] << "  " << vArray[13] << "  " << vArray[14] << "  " << vArray[15] << std::endl;
	//std::cout << vArray[16] << "  " << vArray[17] << "  " << vArray[18] << "  " << vArray[19] << "  " << vArray[20] << "  " << vArray[21] << "  " << vArray[22] << "  " << vArray[23] << std::endl;
	
	glDrawArrays(GL_TRIANGLES, 0, Triangles.size() * 3);

	if (color) {
		glDisableClientState(GL_COLOR_ARRAY);
	}
	else {
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

const XYZ& Model::getTriangleVertex(unsigned triangleId, unsigned vertexId) const
{
	return vertex[Triangles[triangleId].vertex[vertexId]];
}

Model::~Model()
{
	deallocate();
}

void Model::deallocate()
{
	if (owner) {
		free(owner);
	}
	owner = 0;

	if (vertex) {
		free(vertex);
	}
	vertex = 0;

	if (normals) {
		free(normals);
	}
	normals = 0;

	if (vArray) {
		free(vArray);
	}
	vArray = 0;

}

Model::Model()
	: vertexNum(0)
	, type(nothing)
	, owner(0)
	, vertex(0)
	, normals(0)
	, vArray(0)
	, color(0)
	, boundingspherecenter()
	, boundingsphereradius(0)
	, flat(false)
{
	memset(&modelTexture, 0, sizeof(modelTexture));
}
