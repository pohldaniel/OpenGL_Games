#include <iostream>
#include <GL/glew.h>
#include "Skeleton.h"
#include "Utils/Folders.hpp"

Skeleton::Skeleton()
	: selected(0)
	, id(0)
	, num_models(0)
	, clothes(false)
	, spinny(false)
	, skinsize(0)
	, checkdelay(0)
	, longdead(0)
	, broken(false)
	, free(0)
	, oldfree(0)
	, freetime(0)
	, freefall(false)
{
	memset(forwardjoints, 0, sizeof(forwardjoints));
	memset(lowforwardjoints, 0, sizeof(lowforwardjoints));
	memset(jointlabels, 0, sizeof(jointlabels));
	memset(skinText, 0, sizeof(skinText));
}

/* EFFECT
 * sets forward, lowforward, specialforward[]
 *
 * USES:
 * Skeleton::Load
 * Person/Person::DoAnimations
 * Person/Person::DrawSkeleton
 */
void Skeleton::FindForwards()
{
	//Find forward vectors
	CrossProduct(joints[forwardjoints[1]].position - joints[forwardjoints[0]].position, joints[forwardjoints[2]].position - joints[forwardjoints[0]].position, &forward);
	Normalise(&forward);

	CrossProduct(joints[lowforwardjoints[1]].position - joints[lowforwardjoints[0]].position, joints[lowforwardjoints[2]].position - joints[lowforwardjoints[0]].position, &lowforward);
	Normalise(&lowforward);

	//Special forwards
	specialforward[0] = forward;

	specialforward[1] = jointPos(rightshoulder) + jointPos(rightwrist);
	specialforward[1] = jointPos(rightelbow) - specialforward[1] / 2;
	specialforward[1] += forward * .4;
	Normalise(&specialforward[1]);
	specialforward[2] = jointPos(leftshoulder) + jointPos(leftwrist);
	specialforward[2] = jointPos(leftelbow) - specialforward[2] / 2;
	specialforward[2] += forward * .4;
	Normalise(&specialforward[2]);

	specialforward[3] = jointPos(righthip) + jointPos(rightankle);
	specialforward[3] = specialforward[3] / 2 - jointPos(rightknee);
	specialforward[3] += lowforward * .4;
	Normalise(&specialforward[3]);
	specialforward[4] = jointPos(lefthip) + jointPos(leftankle);
	specialforward[4] = specialforward[4] / 2 - jointPos(leftknee);
	specialforward[4] += lowforward * .4;
	Normalise(&specialforward[4]);
}

/* EFFECT
 * applies gravity to the skeleton
 *
 * USES:
 * Person/Person::DoStuff
 */
void Skeleton::DoGravity(float* scale)
{
	for (unsigned i = 0; i < joints.size(); i++) {
		if (
			(
			((joints[i].label != leftknee) && (joints[i].label != rightknee)) ||
				(lowforward.y > -.1) ||
				(joints[i].mass < 5)) &&
				(((joints[i].label != leftelbow) && (joints[i].label != rightelbow)) ||
			(forward.y < .3))) {
			joints[i].velocity.y += gravity * multiplier / (*scale);
		}
	}
}

/* EFFECT
 * set muscles[which].rotate1
 *     .rotate2
 *     .rotate3
 *
 * special case if animation == hanganim
 */
void Skeleton::FindRotationMuscle(int which, int animation)
{
	XYZ p1, p2, fwd;
	float dist;

	p1 = muscles[which].parent1->position;
	p2 = muscles[which].parent2->position;
	dist = findDistance(&p1, &p2);
	if (p1.y - p2.y <= dist) {
		muscles[which].rotate2 = asin((p1.y - p2.y) / dist);
	}
	if (p1.y - p2.y > dist) {
		muscles[which].rotate2 = asin(1.f);
	}
	muscles[which].rotate2 *= 360.0 / 6.2831853;

	p1.y = 0;
	p2.y = 0;
	dist = findDistance(&p1, &p2);
	if (p1.z - p2.z <= dist) {
		muscles[which].rotate1 = acos((p1.z - p2.z) / dist);
	}
	if (p1.z - p2.z > dist) {
		muscles[which].rotate1 = acos(1.f);
	}
	muscles[which].rotate1 *= 360.0 / 6.2831853;
	if (p1.x > p2.x) {
		muscles[which].rotate1 = 360 - muscles[which].rotate1;
	}
	if (!isnormal(muscles[which].rotate1)) {
		muscles[which].rotate1 = 0;
	}
	if (!isnormal(muscles[which].rotate2)) {
		muscles[which].rotate2 = 0;
	}

	const int label1 = muscles[which].parent1->label;
	const int label2 = muscles[which].parent2->label;
	switch (label1) {
	case head:
		fwd = specialforward[0];
		break;
	case rightshoulder:
	case rightelbow:
	case rightwrist:
	case righthand:
		fwd = specialforward[1];
		break;
	case leftshoulder:
	case leftelbow:
	case leftwrist:
	case lefthand:
		fwd = specialforward[2];
		break;
	case righthip:
	case rightknee:
	case rightankle:
	case rightfoot:
		fwd = specialforward[3];
		break;
	case lefthip:
	case leftknee:
	case leftankle:
	case leftfoot:
		fwd = specialforward[4];
		break;
	default:
		if (muscles[which].parent1->lower) {
			fwd = lowforward;
		}
		else {
			fwd = forward;
		}
		break;
	}

	/*if (animation == hanganim) {
		if (label1 == righthand || label2 == righthand) {
			fwd = 0;
			fwd.x = -1;
		}
		if (label1 == lefthand || label2 == lefthand) {
			fwd = 0;
			fwd.x = 1;
		}
	}*/

	if (free == 0) {
		if (label1 == rightfoot || label2 == rightfoot) {
			fwd.y -= .3;
		}
		if (label1 == leftfoot || label2 == leftfoot) {
			fwd.y -= .3;
		}
	}

	fwd = DoRotation(fwd, 0, muscles[which].rotate1 - 90, 0);
	fwd = DoRotation(fwd, 0, 0, muscles[which].rotate2 - 90);
	fwd.y = 0;
	fwd /= findLength(&fwd);
	if (fwd.z <= 1 && fwd.z >= -1) {
		muscles[which].rotate3 = acos(0 - fwd.z);
	}
	else {
		muscles[which].rotate3 = acos(-1.f);
	}
	muscles[which].rotate3 *= 360.0 / 6.2831853;
	if (0 > fwd.x) {
		muscles[which].rotate3 = 360 - muscles[which].rotate3;
	}
	if (!isnormal(muscles[which].rotate3)) {
		muscles[which].rotate3 = 0;
	}
}

/* EFFECT
 * load skeleton
 * takes filenames for three skeleton files and various models
 */
void Skeleton::Load(const std::string& filename, const std::string& lowfilename, const std::string& clothesfilename,
	const std::string& modelfilename, const std::string& model2filename,
	const std::string& model3filename, const std::string& model4filename,
	const std::string& model5filename, const std::string& model6filename,
	const std::string& model7filename, const std::string& modellowfilename,
	const std::string& modelclothesfilename, bool clothes)
{
	GLfloat M[16];
	FILE* tfile;
	float lSize;
	int j, num_joints, num_muscles;

	num_models = 7;

	// load various models
	// rotate, scale, do normals, do texcoords for each as needed

	model[0].loadnotex(modelfilename);
	model[1].loadnotex(model2filename);
	model[2].loadnotex(model3filename);
	model[3].loadnotex(model4filename);
	model[4].loadnotex(model5filename);
	model[5].loadnotex(model6filename);
	model[6].loadnotex(model7filename);

	for (int i = 0; i < num_models; i++) {
		model[i].Rotate(180, 0, 0);
		model[i].Scale(.04, .04, .04);
		model[i].CalculateNormals(0);
	}
	
	drawmodel.load(modelfilename);
	drawmodel.Rotate(180, 0, 0);
	drawmodel.Scale(.04, .04, .04);
	drawmodel.FlipTexCoords();
	//if ((TutorialLu::active) && (id != 0)) {
	//	drawmodel.UniformTexCoords();
	//	drawmodel.ScaleTexCoords(0.1);
	//}
	drawmodel.CalculateNormals(0);

	modellow.loadnotex(modellowfilename);
	modellow.Rotate(180, 0, 0);
	modellow.Scale(.04, .04, .04);
	modellow.CalculateNormals(0);

	drawmodellow.load(modellowfilename);
	drawmodellow.Rotate(180, 0, 0);
	drawmodellow.Scale(.04, .04, .04);
	drawmodellow.FlipTexCoords();
	//if (TutorialLu::active && id != 0) {
	//	drawmodellow.UniformTexCoords();
	//}
	//if (TutorialLu::active && id != 0) {
	//	drawmodellow.ScaleTexCoords(0.1);
	//}
	drawmodellow.CalculateNormals(0);

	if (clothes) {
		modelclothes.loadnotex(modelclothesfilename);
		modelclothes.Rotate(180, 0, 0);
		modelclothes.Scale(.041, .04, .041);
		modelclothes.CalculateNormals(0);

		drawmodelclothes.load(modelclothesfilename);
		drawmodelclothes.Rotate(180, 0, 0);
		drawmodelclothes.Scale(.04, .04, .04);
		drawmodelclothes.FlipTexCoords();
		drawmodelclothes.CalculateNormals(0);
	}

	// FIXME: three similar blocks follow, one for each of:
	// filename, lowfilename, clothesfilename

	// load skeleton

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(filename), "rb");

	// read num_joints
	funpackf(tfile, "Bi", &num_joints);

	joints.clear();
	joints.resize(num_joints);

	// read info for each joint
	for (int i = 0; i < num_joints; i++) {
		joints[i].load(tfile, joints);
	}

	// read num_muscles
	funpackf(tfile, "Bi", &num_muscles);

	// allocate memory
	muscles.clear();
	muscles.resize(num_muscles);

	// for each muscle...
	for (int i = 0; i < num_muscles; i++) {
		muscles[i].load(tfile, model[0].vertexNum, joints);

	}

	// read forwardjoints (?)
	for (j = 0; j < 3; j++) {
		funpackf(tfile, "Bi", &forwardjoints[j]);
	}
	// read lowforwardjoints (?)
	for (j = 0; j < 3; j++) {
		funpackf(tfile, "Bi", &lowforwardjoints[j]);
	}
	fclose(tfile);
	// ???
	for (j = 0; j < num_muscles; j++) {
		for (unsigned i = 0; i < muscles[j].vertices.size(); i++) {
			for (int k = 0; k < num_models; k++) {
				if (muscles[j].vertices[i] < model[k].vertexNum) {
					model[k].owner[muscles[j].vertices[i]] = j;
				}
			}
		}
	}

	// calculate some stuff
	FindForwards();

	for (int i = 0; i < num_muscles; i++) {
		std::cout << "Forward: " << forward.x << "  " << forward.y << "  " << forward.z << std::endl;
		std::cout << "Low Forward: " << lowforward.x << "  " << lowforward.y << "  " << lowforward.z << std::endl;
	}

	for (int i = 0; i < num_muscles; i++) {
		FindRotationMuscle(i, -1);

		std::cout << "Rot: " << muscles[i].rotate3 << "  " << muscles[i].rotate2 << "  " << muscles[i].rotate1 << std::endl;
	}
	// this seems to use opengl purely for matrix calculations
	for (int k = 0; k < num_models; k++) {
		for (int i = 0; i < model[k].vertexNum; i++) {
			model[k].vertex[i] = model[k].vertex[i] - (muscles[model[k].owner[i]].parent1->position + muscles[model[k].owner[i]].parent2->position) / 2;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glRotatef(muscles[model[k].owner[i]].rotate3, 0, 1, 0);
			glRotatef(muscles[model[k].owner[i]].rotate2 - 90, 0, 0, 1);
			glRotatef(muscles[model[k].owner[i]].rotate1 - 90, 0, 1, 0);

			

			glTranslatef(model[k].vertex[i].x, model[k].vertex[i].y, model[k].vertex[i].z);
			glGetFloatv(GL_MODELVIEW_MATRIX, M);
			model[k].vertex[i].x = M[12] * 1;
			model[k].vertex[i].y = M[13] * 1;
			model[k].vertex[i].z = M[14] * 1;
			glPopMatrix();
		}
		model[k].CalculateNormals(0);
	}
	// load ???

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(lowfilename), "rb");

	// skip joints section

	fseek(tfile, sizeof(num_joints), SEEK_CUR);
	for (int i = 0; i < num_joints; i++) {
		// skip joint info
		lSize = sizeof(XYZ) + sizeof(float) + sizeof(float) + 1 //sizeof(bool)
			+ 1                                             //sizeof(bool)
			+ sizeof(int) + 1                               //sizeof(bool)
			+ 1                                             //sizeof(bool)
			+ sizeof(int) + sizeof(int) + 1                 //sizeof(bool)
			+ sizeof(int);
		fseek(tfile, lSize, SEEK_CUR);
	}

	// skip num_muscles
	fseek(tfile, sizeof(num_muscles), SEEK_CUR);

	for (int i = 0; i < num_muscles; i++) {
		// skip muscle info
		lSize = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(int);
		fseek(tfile, lSize, SEEK_CUR);

		muscles[i].loadVerticesLow(tfile, modellow.vertexNum);

		// skip more stuff
		lSize = 1; //sizeof(bool);
		fseek(tfile, lSize, SEEK_CUR);
		lSize = sizeof(int);
		fseek(tfile, lSize, SEEK_CUR);
		fseek(tfile, lSize, SEEK_CUR);
	}

	for (j = 0; j < num_muscles; j++) {
		for (unsigned i = 0; i < muscles[j].verticeslow.size(); i++) {
			if (muscles[j].verticeslow[i] < modellow.vertexNum) {
				modellow.owner[muscles[j].verticeslow[i]] = j;
			}
		}
	}

	// use opengl for its matrix math
	for (int i = 0; i < modellow.vertexNum; i++) {
		modellow.vertex[i] = modellow.vertex[i] - (muscles[modellow.owner[i]].parent1->position + muscles[modellow.owner[i]].parent2->position) / 2;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glRotatef(muscles[modellow.owner[i]].rotate3, 0, 1, 0);
		glRotatef(muscles[modellow.owner[i]].rotate2 - 90, 0, 0, 1);
		glRotatef(muscles[modellow.owner[i]].rotate1 - 90, 0, 1, 0);
		glTranslatef(modellow.vertex[i].x, modellow.vertex[i].y, modellow.vertex[i].z);
		glGetFloatv(GL_MODELVIEW_MATRIX, M);
		modellow.vertex[i].x = M[12];
		modellow.vertex[i].y = M[13];
		modellow.vertex[i].z = M[14];
		glPopMatrix();
	}

	modellow.CalculateNormals(0);

	// load clothes

	if (clothes) {
		tfile = Folders::openMandatoryFile(Folders::getResourcePath(clothesfilename), "rb");

		// skip num_joints
		fseek(tfile, sizeof(num_joints), SEEK_CUR);

		for (int i = 0; i < num_joints; i++) {
			// skip joint info
			lSize = sizeof(XYZ) + sizeof(float) + sizeof(float) + 1 //sizeof(bool)
				+ 1                                             //sizeof(bool)
				+ sizeof(int) + 1                               //sizeof(bool)
				+ 1                                             //sizeof(bool)
				+ sizeof(int) + sizeof(int) + 1                 //sizeof(bool)
				+ sizeof(int);
			fseek(tfile, lSize, SEEK_CUR);
		}

		// skip num_muscles
		fseek(tfile, sizeof(num_muscles), SEEK_CUR);

		for (int i = 0; i < num_muscles; i++) {
			// skip muscle info
			lSize = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(int);
			fseek(tfile, lSize, SEEK_CUR);

			muscles[i].loadVerticesClothes(tfile, modelclothes.vertexNum);

			// skip more stuff
			lSize = 1; //sizeof(bool);
			fseek(tfile, lSize, SEEK_CUR);
			lSize = sizeof(int);
			fseek(tfile, lSize, SEEK_CUR);
			fseek(tfile, lSize, SEEK_CUR);
		}

		// ???
		lSize = sizeof(int);
		for (j = 0; j < num_muscles; j++) {
			for (unsigned i = 0; i < muscles[j].verticesclothes.size(); i++) {
				if (muscles[j].verticesclothes.size() && muscles[j].verticesclothes[i] < modelclothes.vertexNum) {
					modelclothes.owner[muscles[j].verticesclothes[i]] = j;
				}
			}
		}

		// use opengl for its matrix math
		for (int i = 0; i < modelclothes.vertexNum; i++) {
			modelclothes.vertex[i] = modelclothes.vertex[i] - (muscles[modelclothes.owner[i]].parent1->position + muscles[modelclothes.owner[i]].parent2->position) / 2;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glRotatef(muscles[modelclothes.owner[i]].rotate3, 0, 1, 0);
			glRotatef(muscles[modelclothes.owner[i]].rotate2 - 90, 0, 0, 1);
			glRotatef(muscles[modelclothes.owner[i]].rotate1 - 90, 0, 1, 0);
			glTranslatef(modelclothes.vertex[i].x, modelclothes.vertex[i].y, modelclothes.vertex[i].z);
			glGetFloatv(GL_MODELVIEW_MATRIX, M);
			modelclothes.vertex[i].x = M[12];
			modelclothes.vertex[i].y = M[13];
			modelclothes.vertex[i].z = M[14];
			glPopMatrix();
		}

		modelclothes.CalculateNormals(0);
	}
	fclose(tfile);

	for (int i = 0; i < num_joints; i++) {
		for (j = 0; j < num_joints; j++) {
			if (joints[i].label == j) {
				jointlabels[j] = i;
			}
		}
	}

	free = 0;
}
