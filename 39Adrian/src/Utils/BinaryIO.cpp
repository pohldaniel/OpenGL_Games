#include "BinaryIO.h"

short Utils::bytesToShortLE(unsigned char b0, unsigned char b1) {
	short f;
	unsigned char b[] = { b0, b1};
	memcpy(&f, &b, sizeof(short));
	return f;
}

short Utils::bytesToShortBE(unsigned char b0, unsigned char b1) {
	short f;
	unsigned char b[] = { b1, b0 };
	memcpy(&f, &b, sizeof(short));
	return f;
}

bool Utils::bytesToBool(unsigned char b0) {
	bool f;
	unsigned char b[] = { b0 };
	memcpy(&f, &b, sizeof(bool));
	return f;
}

int Utils::bytesToIntLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
	int f;
	unsigned char b[] = { b0, b1, b2, b3 };
	memcpy(&f, &b, sizeof(int));
	return f;
}

int Utils::bytesToIntBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
	int f;
	unsigned char b[] = { b3, b2, b1, b0 };
	memcpy(&f, &b, sizeof(int));
	return f;
}

unsigned int Utils::bytesToUIntLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
	unsigned int f;
	unsigned char b[] = { b0, b1, b2, b3 };
	memcpy(&f, &b, sizeof(unsigned int));
	return f;
}

unsigned int Utils::bytesToUIntBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
	unsigned int f;
	unsigned char b[] = { b3, b2, b1, b0 };
	memcpy(&f, &b, sizeof(unsigned int));
	return f;
}

float Utils::bytesToFloatLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
	float f;
	unsigned char b[] = { b0, b1, b2, b3 };
	memcpy(&f, &b, sizeof(float));
	return f;
}

float Utils::bytesToFloatBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3) {
	float f;
	unsigned char b[] = { b3, b2, b1, b0 };
	memcpy(&f, &b, sizeof(float));
	return f;
}

void Utils::Joint::print() {
	std::cout << "-------Joint-------" << std::endl;
	std::cout << "Position: " << position[0] << "  " << position[1] << "  " << position[2] << std::endl;
	std::cout << "Length: " << length << " Mass: " << mass << std::endl;

	std::cout << "Has Parent: " << hasparent << " Locked: " << locked << std::endl;
	std::cout << "Modelnum: " << modelnum << std::endl;
	std::cout << "Visible: " << visible << " Sametwist: " << sametwist << std::endl;
	std::cout << "Label: " << label << " Has Gun: " << hasgun << std::endl;;
	std::cout << "Lower: " << lower << std::endl;
	std::cout << "ParentIndex: " << parentIndex << std::endl;
	std::cout << "-------------------" << std::endl;
}

void Utils::Muscle::print() {
	std::cout << "-------Muscle-------" << std::endl;
	std::cout << "Length: " << length << " Target Length: "  << targetlength << " Min Length: " << minlength <<  " Max Length: "<< maxlength << " Strength: " << strength << std::endl;
	std::cout << "Type: " << type << " Num Vertices: " << numvertices << std::endl;
	for (int index : vertexIndices) {
		std::cout << "Index: " << index << std::endl;
	}
	std::cout << "Visible: " << visible << std::endl;
	std::cout << "ParentIndex1: " << parentIndex1 << " ParentIndex2: " << parentIndex2 << std::endl;
	std::cout << "--------------------" << std::endl;
}

void Utils::Skeleton::print() {
	std::cout << "###########SKELETON###########" << std::endl;
	for (auto&& joint : m_joints) {
		joint.print();
	}

	for (auto&& muscle : m_muscles) {
		muscle.print();
	}
	std::cout << "Forward: " << forwardJoint1 << "  " << forwardJoint2 << "  " << forwardJoint3 << std::endl;
	std::cout << "Low Forward: " << lowForwardJoint1 << "  " << lowForwardJoint2 << "  " << lowForwardJoint3 << std::endl;
}

void Utils::Animation::print() {
	std::cout << "###########ANIMATION###########" << std::endl;
	for (unsigned int i = 0; i < frames.size(); i++) {
		for (unsigned int j = 0; j < numJoints; j++) {
			std::cout << "Pos: " << frames[i].joints[j].position[0] << "  " << frames[i].joints[j].position[1] << "  " << frames[i].joints[j].position[2] << std::endl;
			std::cout << "Twist: " << frames[i].joints[j].twist << std::endl;
			std::cout << "Twist2: " << frames[i].joints[j].twist2 << std::endl;
			std::cout << "On Ground: " << frames[i].joints[j].onground << std::endl;
		}
		std::cout << "Speed: " << frames[i].speed << std::endl;
		std::cout << "Label: " << frames[i].label << std::endl;
		std::cout << "Weapontarget: " << frames[i].weapontarget[0] << "  " << frames[i].weapontarget[1] << "  " << frames[i].weapontarget[2] << std::endl;
	}
	std::cout << "Offset: " << offset[0] << "  " << offset[1] << "  " << offset[2] << std::endl;
}

Utils::Joint& Utils::Skeleton::joint(int bodypart) { 
	return m_joints[jointlabels[bodypart]]; 
}

void Utils::Skeleton::findForwards() {

	forward = Vector3f::Cross(m_joints[forwardJoint2].position - m_joints[forwardJoint1].position, m_joints[forwardJoint3].position - m_joints[forwardJoint1].position);
	specialforward[0] = Vector3f::Normalize(forward);
	
	lowForward = Vector3f::Cross(m_joints[lowForwardJoint2].position - m_joints[lowForwardJoint1].position, m_joints[lowForwardJoint3].position - m_joints[lowForwardJoint1].position);
	Vector3f::Normalize(lowForward);

	specialforward[1] = joint(Utils::rightshoulder).position + joint(Utils::rightwrist).position;
	specialforward[1] = joint(Utils::rightelbow).position - (specialforward[1] * 0.5f);
	specialforward[1] += forward * 0.4f;
	Vector3f::Normalize(specialforward[1]);
	specialforward[2] = joint(Utils::leftshoulder).position + joint(Utils::leftwrist).position;
	specialforward[2] = (joint(Utils::leftelbow).position - (specialforward[2]) * 0.5f);
	specialforward[2] += forward * 0.4f;
	Vector3f::Normalize(specialforward[2]);
	specialforward[3] = joint(Utils::righthip).position + joint(Utils::rightankle).position;
	specialforward[3] = (specialforward[3] * 0.5f) - joint(Utils::rightknee).position;
	specialforward[3] += lowForward * 0.4f;
	Vector3f::Normalize(specialforward[3]);
	specialforward[4] = joint(Utils::lefthip).position + joint(Utils::leftankle).position;
	specialforward[4] = (specialforward[4] * 0.5f) - joint(Utils::leftknee).position;
	specialforward[4] += lowForward * 0.4f;
	Vector3f::Normalize(specialforward[4]);
}

void Utils::Skeleton::findRotationMuscle(int which, int animation) {
	Vector3f p1, p2, fwd;
	float dist;

	p1 = m_joints[m_muscles[which].parentIndex1].position;
	p2 = m_joints[m_muscles[which].parentIndex2].position;
	dist = (p2 - p1).length();

	
	if (p1[1] - p2[1] <= dist) {
		m_muscles[which].rotate2 = asin((p1[1] - p2[1]) / dist);
	}
	if (p1[1] - p2[1] > dist) {
		m_muscles[which].rotate2 = asin(1.f);
	}
	m_muscles[which].rotate2 *= 360.0 / 6.2831853;

	p1[1] = 0;
	p2[1] = 0;
	dist = (p2 - p1).length();

	if (p1[2] - p2[2] <= dist) {
		m_muscles[which].rotate1 = acos((p1[2] - p2[2]) / dist);
	}
	if (p1[2] - p2[2] > dist) {
		m_muscles[which].rotate1 = acos(1.f);
	}
	m_muscles[which].rotate1 *= 360.0 / 6.2831853;
	if (p1[0] > p2[0]) {
		m_muscles[which].rotate1 = 360 - m_muscles[which].rotate1;
	}
	if (!isnormal(m_muscles[which].rotate1)) {
		m_muscles[which].rotate1 = 0;
	}
	if (!isnormal(m_muscles[which].rotate2)) {
		m_muscles[which].rotate2 = 0;
	}

	const int label1 = m_joints[m_muscles[which].parentIndex1].label;
	const int label2 = m_joints[m_muscles[which].parentIndex2].label;

	switch (label1) {
	case Utils::head:
		fwd = specialforward[0];
		break;
	case Utils::rightshoulder:
	case Utils::rightelbow:
	case Utils::rightwrist:
	case Utils::righthand:
		fwd = specialforward[1];
		break;
	case Utils::leftshoulder:
	case Utils::leftelbow:
	case Utils::leftwrist:
	case Utils::lefthand:
		fwd = specialforward[2];
		break;
	case Utils::righthip:
	case Utils::rightknee:
	case Utils::rightankle:
	case Utils::rightfoot:
		fwd = specialforward[3];
		break;
	case Utils::lefthip:
	case Utils::leftknee:
	case Utils::leftankle:
	case Utils::leftfoot:
		fwd = specialforward[4];
		break;
	default:
		if (m_joints[m_muscles[which].parentIndex1].lower) {
			fwd = lowForward;
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
		if (label1 == Utils::rightfoot || label2 == Utils::rightfoot) {
			fwd[1] -= .3;
		}
		if (label1 == Utils::leftfoot || label2 == Utils::leftfoot) {
			fwd[1] -= .3;
		}
	}
	fwd = Math::RotatePoint(fwd, 0.0f, m_muscles[which].rotate1 - 90.0f, 0.0f);
	fwd = Math::RotatePoint(fwd, 0.0f, 0.0f, m_muscles[which].rotate2 - 90.0f);
	fwd[1] = 0.0f;
	Vector3f::Normalize(fwd);

	if (fwd[2] <= 1 && fwd[2] >= -1) {
		m_muscles[which].rotate3 = acos(0 - fwd[2]);
	}else {
		m_muscles[which].rotate3 = acos(-1.f);
	}

	m_muscles[which].rotate3 *= 360.0 / 6.2831853;
	if (0 > fwd[0]) {
		m_muscles[which].rotate3 = 360 - m_muscles[which].rotate3;
	}
	if (!isnormal(m_muscles[which].rotate3)) {
		m_muscles[which].rotate3 = 0;
	}
}

bool Utils::SolidIO::getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, ComparerUv>& uvToOutIndex, short & result) {
	std::map<std::array<float, 2>, short>::iterator it = uvToOutIndex.find(packed);
	if (it == uvToOutIndex.end()) {
		return false;
	}else {
		result = it->second;
		return true;
	}
}

bool Utils::SolidIO::getSimilarVertexIndex(Vertex& packed, std::map<Vertex, short, Comparer>& vertexToOutIndex, short & result) {
	std::map<Vertex, short>::iterator it = vertexToOutIndex.find(packed);
	if (it == vertexToOutIndex.end()) {
		return false;
	}else {
		result = it->second;
		return true;
	}
}

void Utils::SolidIO::solidToObj(const char* filename, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical) {

	std::filesystem::path mltPath(outFileMtl);

	std::vector<std::array<float, 3>> vertices;
	std::vector<std::array<short, 3>> faces;

	std::vector <std::array<float, 2>> uvCoords;
	std::vector<short> uvFaves;
	std::map<std::array<float, 2>, short, ComparerUv> vertexCache;

	std::ifstream file(filename, std::ios::binary);
	char metaData[4];
	file.read(&metaData[0], 1);
	file.read(&metaData[1], 1);
	file.read(&metaData[2], 1);
	file.read(&metaData[3], 1);

	short vertexCount = bytesToShortBE(metaData[0], metaData[1]);
	short triangleCount = bytesToShortBE(metaData[2], metaData[3]);

	char* buffer = new char[vertexCount * sizeof(float) * 3];
	file.read(buffer, vertexCount * sizeof(float) * 3);

	for (int i = 0; i < vertexCount * 3 * sizeof(float); i = i + 12) {
		UFloat value[3];
		value[0].c[0] = (unsigned int)(buffer[i + 3]); value[0].c[1] = (unsigned int)(buffer[i + 2]); value[0].c[2] = (unsigned int)(buffer[i + 1]); value[0].c[3] = (unsigned int)(buffer[i + 0]);
		value[1].c[0] = (unsigned int)(buffer[i + 7]); value[1].c[1] = (unsigned int)(buffer[i + 6]); value[1].c[2] = (unsigned int)(buffer[i + 5]); value[1].c[3] = (unsigned int)(buffer[i + 4]);
		value[2].c[0] = (unsigned int)(buffer[i + 11]); value[2].c[1] = (unsigned int)(buffer[i + 10]); value[2].c[2] = (unsigned int)(buffer[i + 9]); value[2].c[3] = (unsigned int)(buffer[i + 8]);
		vertices.push_back({ value[0].flt , value[1].flt , value[2].flt });
	}

	char* bufferTris = new char[triangleCount * (6 * (sizeof(short) + sizeof(float)))];

	file.read(bufferTris, triangleCount * (6 * (sizeof(short) + sizeof(float))));
	file.close();

	for (int i = 0; i < triangleCount * (6 * (sizeof(short) + sizeof(float))); i = i + 36) {
		faces.push_back({ bytesToShortBE(bufferTris[i] ,bufferTris[i + 1]) , bytesToShortBE(bufferTris[i + 4], bufferTris[i + 5]) ,  bytesToShortBE(bufferTris[i + 8], bufferTris[i + 9]) });

		short index;
		bool found;
		std::array<float, 2> uv;
		for (int k = 0; k < 3; k++) {
			if(flipVertical)
				uv = { bytesToFloatBE(bufferTris[i + (k + 3) * 4], bufferTris[i + (k + 3) * 4 + 1], bufferTris[i + (k + 3) * 4 + 2], bufferTris[i + (k + 3) * 4 + 3]), 1.0f - bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3]) };
			else
				uv = { bytesToFloatBE(bufferTris[i + (k + 3) * 4], bufferTris[i + (k + 3) * 4 + 1], bufferTris[i + (k + 3) * 4 + 2], bufferTris[i + (k + 3) * 4 + 3]), bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3]) };
			found = getSimilarVertexIndex(uv, vertexCache, index);
			if (found) {
				uvFaves.push_back(index);
			}else {
				uvCoords.push_back(uv);
				short newindex = (short)uvCoords.size() - 1;
				uvFaves.push_back(newindex);
				vertexCache[uv] = newindex;
			}
		}
	}

	delete buffer;
	delete bufferTris;

	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";
	fileOut << "mtllib " << mltPath.filename().string() << std::endl;

	for (int i = 0; i < vertices.size(); i++) {
		fileOut << "v " << vertices[i][0] << " " << vertices[i][1] << " " << vertices[i][2] << std::endl;
	}
	for (int i = 0; i < uvCoords.size(); i++) {
		fileOut << "vt " << uvCoords[i][0] << " " << uvCoords[i][1] << std::endl;
	}
	fileOut << "usemtl Material\n";
	for (int i = 0; i < faces.size(); i++) {
		fileOut << "f " << faces[i][0] + 1 << "/" << uvFaves[i * 3] + 1 << " " << faces[i][1] + 1 << "/" << uvFaves[i * 3 + 1] + 1 << " " << faces[i][2] + 1 << "/" << uvFaves[i * 3 + 2] + 1 << std::endl;
	}

	fileOut.close();

	fileOut.open(outFileMtl);
	fileOut << std::setprecision(6) << std::fixed;
	fileOut << "newmtl Material\n";
	fileOut << "Ns 10.000000\n";

	fileOut << "Ka 0.000000 0.000000 0.000000\n";
	fileOut << "Kd 1.000000 1.000000 1.000000\n";
	fileOut << "Ks 0.000000 0.000000 0.000000\n";
	fileOut << "Ni 1.000000\n";
	fileOut << "d 1.000000\n";
	fileOut << "illum 1\n";
	std::string absPath = std::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '\\', '/');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();
}

std::array<float, 3> Utils::RotatePoint(std::array<float, 3> point, float xang, float yang, float zang){
	std::array<float, 3> newpoint;

	xang = xang * PI_ON_180;
	yang = yang * PI_ON_180;
	zang = zang * PI_ON_180;
	
	if (yang) {
		newpoint[2] = point[2] * cosf(yang) - point[0] * sinf(yang);
		newpoint[0] = point[2] * sinf(yang) + point[0] * cosf(yang);
		point[2] = newpoint[2];
		point[0] = newpoint[0];
	}

	if (zang) {
		newpoint[0] = point[0] * cosf(zang) - point[1] * sinf(zang);
		newpoint[1] = point[1] * cosf(zang) + point[0] * sinf(zang);
		point[0] = newpoint[0];
		point[1] = newpoint[1];
	}

	if (xang) {
		newpoint[1] = point[1] * cosf(xang) - point[2] * sinf(xang);
		newpoint[2] = point[1] * sinf(xang) + point[2] * cosf(xang);
		point[2] = newpoint[2];
		point[1] = newpoint[1];
	}

	return point;
}

std::array<float, 3> Utils::ScalePoint(std::array<float, 3> point, float scaleX, float scaleY, float scaleZ) {
	return { point[0] * scaleX, point[1] * scaleY , point[2] * scaleZ };
}

void Utils::SolidIO::solidToBuffer(const char* filename, bool flipTextureVertical, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<Vertex>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	std::vector<Vertex> vertices;
	std::map<Vertex, short, Comparer> vertexCache;

	std::ifstream file(filename, std::ios::binary);
	char metaData[4];
	file.read(&metaData[0], 1);
	file.read(&metaData[1], 1);
	file.read(&metaData[2], 1);
	file.read(&metaData[3], 1);

	short vertexCount = bytesToShortBE(metaData[0], metaData[1]);
	short triangleCount = bytesToShortBE(metaData[2], metaData[3]);

	char* buffer = new char[vertexCount * sizeof(float) * 3];
	file.read(buffer, vertexCount * sizeof(float) * 3);

	for (int i = 0; i < vertexCount * 3 * sizeof(float); i = i + 12) {
		UFloat value[3];
		value[0].c[0] = (unsigned int)(buffer[i + 3]); value[0].c[1] = (unsigned int)(buffer[i + 2]); value[0].c[2] = (unsigned int)(buffer[i + 1]); value[0].c[3] = (unsigned int)(buffer[i + 0]);
		value[1].c[0] = (unsigned int)(buffer[i + 7]); value[1].c[1] = (unsigned int)(buffer[i + 6]); value[1].c[2] = (unsigned int)(buffer[i + 5]); value[1].c[3] = (unsigned int)(buffer[i + 4]);
		value[2].c[0] = (unsigned int)(buffer[i + 11]); value[2].c[1] = (unsigned int)(buffer[i + 10]); value[2].c[2] = (unsigned int)(buffer[i + 9]); value[2].c[3] = (unsigned int)(buffer[i + 8]);
		std::array<float, 3> vert = ScalePoint(RotatePoint({ value[0].flt, value[1].flt, value[2].flt }, eulerAngle[0], eulerAngle[1], eulerAngle[2]), scale[0], scale[1], scale[2]);		
		vertices.push_back({ vert[0], vert[1], vert[2], 0.0f, 0.0f , static_cast<unsigned int>(vertices.size()) });
	}

	char* bufferTris = new char[triangleCount * (6 * (sizeof(short) + sizeof(float)))];

	file.read(bufferTris, triangleCount * (6 * (sizeof(short) + sizeof(float))));
	file.close();

	std::array<short, 3> ccw = { 0, 2, 1 };
	std::array<short, 3> cw = { 0, 1, 2 };

	for (int i = 0; i < triangleCount * (6 * (sizeof(short) + sizeof(float))); i = i + 36) {

		short index;
		bool found;
		Vertex vert;

		for (const short &k : ccw) {
			vert = { vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])].data[0],
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])].data[1],
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])].data[2],
					 bytesToFloatBE(bufferTris[i + (k + 3) * 4], bufferTris[i + (k + 3) * 4 + 1], bufferTris[i + (k + 3) * 4 + 2], bufferTris[i + (k + 3) * 4 + 3]),
					 flipTextureVertical ? 1.0f - bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3]) : bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3]),
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])].index
			};

			found = getSimilarVertexIndex(vert, vertexCache, index);
			if (found) {
				indexBufferOut.push_back(index);
			}else {
				unsigned int newindex = (unsigned int)(vertexBufferOut.size());
				vertexBufferOut.insert(vertexBufferOut.end(), vert);
				indexBufferOut.push_back(newindex);
				vertexCache[vert] = newindex;
			}
		}
	}

	delete buffer;
	delete bufferTris;
}

void Utils::SolidIO::loadSkeleton(const char* filename, const std::vector<Vertex>& vertexBufferMap, std::vector<float>& vertexBufferOut, Skeleton& skeleton, std::vector<std::array<float, 4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut) {
		
	std::ifstream file(filename, std::ios::binary);
	char metaData[4];
	file.read(&metaData[0], 1);
	file.read(&metaData[1], 1);
	file.read(&metaData[2], 1);
	file.read(&metaData[3], 1);

	int jointCount = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);


	short offset = 5 * sizeof(float) + 2 * sizeof(bool) + sizeof(int) + 2 * sizeof(bool) + 2 * sizeof(int) + sizeof(bool) + sizeof(int);
	char* buffer = new char[jointCount * offset];
	file.read(buffer, jointCount * offset);

	for (int i = 0; i < jointCount; i++) {
		UFloat flt[5];
		flt[0].c[0] = buffer[i * offset + 3]; flt[0].c[1] = buffer[i * offset + 2]; flt[0].c[2] = buffer[i * offset + 1]; flt[0].c[3] = buffer[i * offset + 0];
		flt[1].c[0] = buffer[i * offset + 7]; flt[1].c[1] = buffer[i * offset + 6]; flt[1].c[2] = buffer[i * offset + 5]; flt[1].c[3] = buffer[i * offset + 4];
		flt[2].c[0] = buffer[i * offset + 11]; flt[2].c[1] = buffer[i * offset + 10]; flt[2].c[2] = buffer[i * offset + 9]; flt[2].c[3] = buffer[i * offset + 8];
		flt[3].c[0] = buffer[i * offset + 15]; flt[3].c[1] = buffer[i * offset + 14]; flt[3].c[2] = buffer[i * offset + 13]; flt[3].c[3] = buffer[i * offset + 12];
		flt[4].c[0] = buffer[i * offset + 19]; flt[4].c[1] = buffer[i * offset + 18]; flt[4].c[2] = buffer[i * offset + 17]; flt[4].c[3] = buffer[i * offset + 16];

		UBool bl[5];
		bl[0].c[0] = buffer[i * offset + 20];
		bl[1].c[0] = buffer[i * offset + 21];

		UInt nt[4];
		nt[0].c[0] = buffer[i * offset + 25]; nt[0].c[1] = buffer[i * offset + 24]; nt[0].c[2] = buffer[i * offset + 23]; nt[0].c[3] = buffer[i * offset + 22];

		bl[2].c[0] = buffer[i * offset + 26];
		bl[3].c[0] = buffer[i * offset + 27];

		nt[1].c[0] = buffer[i * offset + 31]; nt[1].c[1] = buffer[i * offset + 30]; nt[1].c[2] = buffer[i * offset + 29]; nt[1].c[3] = buffer[i * offset + 28];
		nt[2].c[0] = buffer[i * offset + 35]; nt[2].c[1] = buffer[i * offset + 34]; nt[2].c[2] = buffer[i * offset + 33]; nt[2].c[3] = buffer[i * offset + 32];

		bl[4].c[0] = buffer[i * offset + 36];
		nt[3].c[0] = buffer[i * offset + 40]; nt[3].c[1] = buffer[i * offset + 39]; nt[3].c[2] = buffer[i * offset + 38]; nt[3].c[3] = buffer[i * offset + 37];

		skeleton.m_joints.push_back({ { flt[0].flt, flt[1].flt, flt[2].flt },
                                        flt[3].flt, flt[4].flt,
                                        bl[0].bl, bl[1].bl,
                                        nt[0].nt,
                                        bl[2].bl,bl[3].bl,
                                        nt[1].nt, nt[2].nt,
                                        bl[4].bl,
                                        nt[3].nt });
	}
	delete buffer;

	file.read(&metaData[0], 1);
	file.read(&metaData[1], 1);
	file.read(&metaData[2], 1);
	file.read(&metaData[3], 1);

	int numMuscles = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

	offset = 5 * sizeof(float) + 2 * sizeof(int);
	buffer = new char[offset];

	for (int i = 0; i < numMuscles; i++) {
		skeleton.m_muscles.resize(skeleton.m_muscles.size() + 1);
		Muscle& muscle = skeleton.m_muscles.back();

		file.read(buffer, offset);
		UFloat flt[5];
		flt[0].c[0] = buffer[3]; flt[0].c[1] = buffer[2]; flt[0].c[2] = buffer[1]; flt[0].c[3] = buffer[0];
		flt[1].c[0] = buffer[7]; flt[1].c[1] = buffer[6]; flt[1].c[2] = buffer[5]; flt[1].c[3] = buffer[4];
		flt[2].c[0] = buffer[11]; flt[2].c[1] = buffer[10]; flt[2].c[2] = buffer[9]; flt[2].c[3] = buffer[8];
		flt[3].c[0] = buffer[15]; flt[3].c[1] = buffer[14]; flt[3].c[2] = buffer[13]; flt[3].c[3] = buffer[12];
		flt[4].c[0] = buffer[19]; flt[4].c[1] = buffer[18]; flt[4].c[2] = buffer[17]; flt[4].c[3] = buffer[16];

		muscle.length = flt[0].flt;
		muscle.targetlength = flt[1].flt;
		muscle.minlength = flt[2].flt;
		muscle.maxlength = flt[3].flt;
		muscle.strength = flt[4].flt;

		UInt nt[4];
		nt[0].c[0] = buffer[23]; nt[0].c[1] = buffer[22]; nt[0].c[2] = buffer[21]; nt[0].c[3] = buffer[20];
		nt[1].c[0] = buffer[27]; nt[1].c[1] = buffer[26]; nt[1].c[2] = buffer[25]; nt[1].c[3] = buffer[24];

		muscle.type = nt[0].nt;
		muscle.numvertices = nt[1].nt;

		for (int j = 0; j < nt[1].nt; j++) {
			file.read(&metaData[0], 1);file.read(&metaData[1], 1);file.read(&metaData[2], 1);file.read(&metaData[3], 1);
			muscle.vertexIndices.push_back(bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]));
		}

		file.read(buffer, sizeof(bool) + 2 * sizeof(int));

		UBool bl[1];
		bl[0].c[0] = buffer[0];

		muscle.visible = bl[0].bl;

		nt[2].c[0] = buffer[4]; nt[2].c[1] = buffer[3]; nt[2].c[2] = buffer[2]; nt[2].c[3] = buffer[1];
		nt[3].c[0] = buffer[8]; nt[3].c[1] = buffer[7]; nt[3].c[2] = buffer[6]; nt[3].c[3] = buffer[5];

		muscle.parentIndex1 = nt[2].nt;
		muscle.parentIndex2 = nt[3].nt;
	}
	delete buffer;

	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	skeleton.forwardJoint1 = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	skeleton.forwardJoint2 = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	skeleton.forwardJoint3 = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	skeleton.lowForwardJoint1 = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	skeleton.lowForwardJoint2 = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	skeleton.lowForwardJoint3 = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);
	file.close();

	int numVert = vertexBufferMap.size();

	std::vector<int> owner;
	owner.resize(numVert);

	for (unsigned int j = 0; j < numMuscles; j++) {
		std::vector<int> vertexIndicesNew;
		for (unsigned int i = 0; i < skeleton.m_muscles[j].vertexIndices.size(); i++) {
			for (int k = 0; k < vertexBufferMap.size(); k++) {
				if (skeleton.m_muscles[j].vertexIndices[i] == vertexBufferMap[k].index) {
					vertexIndicesNew.push_back(k);
				}
			}
		}
		skeleton.m_muscles[j].vertexIndices = vertexIndicesNew;
	}
	
	for (unsigned int j = 0; j < numMuscles; j++) {
		for (unsigned int i = 0; i < skeleton.m_muscles[j].vertexIndices.size(); i++) {
				if (skeleton.m_muscles[j].vertexIndices[i] < numVert) {
					owner[skeleton.m_muscles[j].vertexIndices[i]] = j;
				}			
		}
	}
	memset(skeleton.jointlabels, 0, sizeof(skeleton.jointlabels));
	skeleton.findForwards();

	for (int i = 0; i < numMuscles; i++) {
		skeleton.findRotationMuscle(i, -1);
	}

	for (unsigned int i = 0; i < numVert; i++) {
		for (unsigned int j = 0; j < numMuscles; j++) {
			for (unsigned int k = 0; k < skeleton.m_muscles[j].vertexIndices.size(); k++) {

				if (i == skeleton.m_muscles[j].vertexIndices[k]) {
					boneIdsOut.push_back({ j, 0, 0, 0 });
					weightsOut.push_back({1.0f, 0.0f, 0.0f, 0.0f});
				}
			}	
		}
	}

	for (unsigned int k = 0; k < 1; k++) {
		for (unsigned int i = 0; i < numVert; i++) {
			Vector3f pos = { vertexBufferMap[i].data[0] , vertexBufferMap[i].data[1], vertexBufferMap[i].data[2] };
			Matrix4f trans = Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), skeleton.m_muscles[owner[i]].rotate3) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), skeleton.m_muscles[owner[i]].rotate2 - 90.0f) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), skeleton.m_muscles[owner[i]].rotate1 - 90.0f) *   Matrix4f::InvTranslate((skeleton.m_joints[skeleton.m_muscles[owner[i]].parentIndex1].position + skeleton.m_joints[skeleton.m_muscles[owner[i]].parentIndex2].position) * 0.5f);
			pos = trans ^ pos;
			vertexBufferOut.push_back(pos[0]); vertexBufferOut.push_back(pos[1]); vertexBufferOut.push_back(pos[2]);
			vertexBufferOut.push_back(vertexBufferMap[i].data[3]); vertexBufferOut.push_back(vertexBufferMap[i].data[4]);
		}
	}

	for (unsigned int j = 0; j < numMuscles; j++) {
		Vector3f mid = (skeleton.m_joints[skeleton.m_muscles[j].parentIndex1].position + skeleton.m_joints[skeleton.m_muscles[j].parentIndex2].position) * 0.5f;
		skeleton.m_muscles[j].m_modelMatrixInitial = Matrix4f::Translate(mid) * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), 90.0f - skeleton.m_muscles[j].rotate1) * Matrix4f::Rotate(Vector3f(0.0f, 0.0f, 1.0f), 90.0f - skeleton.m_muscles[j].rotate2)  * Matrix4f::Rotate(Vector3f(0.0f, 1.0f, 0.0f), -skeleton.m_muscles[j].rotate3);
	}

	for (int i = 0; i < jointCount; i++) {
		for (int j = 0; j < jointCount; j++) {
			if (skeleton.m_joints[i].label == j) {
				skeleton.jointlabels[j] = i;
			}
		}
	}
}

std::ifstream::pos_type filesize(const char* filename){
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void Utils::SolidIO::loadAnimation(const char* filename, Utils::anim_height_type aheight, Utils::anim_attack_type aattack, Animation& animation) {
	animation.height = aheight;
	animation.attack = aattack;

	std::ifstream file(filename, std::ios::binary);
	char metaData[4];
	
	file.read(&metaData[0], 1);file.read(&metaData[1], 1);file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	animation.numFrames = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);
	file.read(&metaData[0], 1); file.read(&metaData[1], 1); file.read(&metaData[2], 1); file.read(&metaData[3], 1);
	animation.numJoints = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);
	animation.frames.resize(animation.numFrames);
	
	short offset = 4 * sizeof(float) + 1 * sizeof(bool);
	char* buffer = new char[offset * animation.numJoints];
	
	for (unsigned int i = 0; i < animation.frames.size(); i++) {
		file.read(buffer, offset * animation.numJoints);
		AnimationFrameJointInfo jointInfo;
		for (unsigned int j = 0, k = 0, l = 0; j < 12 * animation.numJoints; j = j + 12, k = k + 4, l++ ) {
			animation.frames[i].joints.push_back({
				{bytesToFloatBE(buffer[j], buffer[j + 1], buffer[j + 2], buffer[j + 3]),
				 bytesToFloatBE(buffer[j + 4], buffer[j + 5], buffer[j + 6], buffer[j + 7]),
				 bytesToFloatBE(buffer[j + 8], buffer[j + 9], buffer[j + 10], buffer[j + 11])},
				 bytesToFloatBE(buffer[12 * animation.numJoints + k], buffer[12 * animation.numJoints + k + 1], buffer[12 * animation.numJoints + k + 2], buffer[12 * animation.numJoints + k + 3]),
				 0.0f,
                 buffer[16 * animation.numJoints + l] != 0 });
		}
		
		file.read(metaData, 4);
		animation.frames[i].speed = bytesToFloatBE(metaData[0], metaData[1], metaData[2], metaData[3]);
	}

	offset = sizeof(float);
	for (unsigned int i = 0; i < animation.frames.size(); i++) {
		file.read(buffer, offset * animation.numJoints);

		for (unsigned int j = 0; j < animation.numJoints; j++) {
			animation.frames[i].joints[j].twist2 = bytesToFloatBE(buffer[j * offset], buffer[j * offset + 1], buffer[j * offset + 2], buffer[j * offset + 3]);
		}
	}

	file.read(buffer, offset * animation.frames.size());
	for (unsigned int i = 0; i < animation.frames.size(); i++) {
		animation.frames[i].label = bytesToIntBE(buffer[i * offset], buffer[i * offset + 1], buffer[i * offset + 2], buffer[i * offset + 3]);
	}

	if (file.peek() != EOF) {
		file.read(metaData, 4);
		int weapontargetnum = bytesToIntBE(metaData[0], metaData[1], metaData[2], metaData[3]);

		offset = 3 * sizeof(float);
		file.read(buffer, offset * animation.frames.size());
		for (unsigned int i = 0; i < animation.frames.size(); i++) {
			animation.frames[i].weapontarget[0] = bytesToFloatBE(buffer[i * offset], buffer[i * offset + 1], buffer[i * offset + 2], buffer[i * offset + 3]);
			animation.frames[i].weapontarget[1] = bytesToFloatBE(buffer[i * offset + 4], buffer[i * offset + 5], buffer[i * offset + 6], buffer[i * offset + 7]);
			animation.frames[i].weapontarget[2] = bytesToFloatBE(buffer[i * offset + 8], buffer[i * offset + 9], buffer[i * offset + 10], buffer[i * offset + 11]);
		}
	}

	delete buffer;
	file.close();

	Vector3f endoffset = Vector3f(0.0f);
	for (unsigned int i = 0; i < animation.frames.back().joints.size(); i++) {
		if (animation.frames.back().joints[i].position[1] < 1.0f) {
			endoffset += animation.frames.back().joints[i].position;
		}
	}
	endoffset /= animation.numJoints;
	animation.offset = endoffset;
	animation.offset[1] = 0.0f;
}

void Utils::MdlIO::mdlToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath) {
	std::filesystem::path mltPath(outFileMtl);

	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<float, 3>> normals;
	std::vector <std::array<float, 2>> uvCoords;

	std::vector<std::array<short, 3>> faces;
	std::vector<std::array<short, 3>> facesLod;
	std::map<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> geomteries;

	std::ifstream file(path, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));

	char metaData[4];

	file.read(metaData, sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numElements = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	unsigned int vertexSize = 0;

	if (ret == "UMD2") {
		for (unsigned j = 0; j < numElements; ++j) {
			file.read(metaData, sizeof(unsigned int));
			unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			VertexElementSemantic semantic = (VertexElementSemantic)((elementDesc >> 8) & 0xff);
			vertexSize += Utils::ELEMENT_TYPESIZES[elementDesc & 0xff];
		}
	}else {
		if (numElements & 1)
			vertexSize += sizeof(float) * 3;
		if (numElements & 2)
			vertexSize += sizeof(float) * 3;
		if (numElements & 4)
			vertexSize += 4;
		if (numElements & 8)
			vertexSize += sizeof(float) * 2;
		if (numElements & 16)
			vertexSize += sizeof(float) * 2;
		if (numElements & 32)
			vertexSize += sizeof(float) * 3;
		if (numElements & 64)
			vertexSize += sizeof(float) * 3;
		if (numElements & 128)
			vertexSize += sizeof(float) * 4;
		if (numElements & 256)
			vertexSize += sizeof(float) * 4;
		if (numElements & 512)
			vertexSize += 4;
	}

	file.read(metaData, sizeof(unsigned int));
	file.read(metaData, sizeof(unsigned int));

	char* buffer = new char[vertexCount * vertexSize];
	file.read(buffer, vertexCount * vertexSize);

	for (unsigned int i = 0; i < vertexCount * vertexSize; i = i + vertexSize) {

		UFloat value[3];
		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1]; value[0].c[2] = buffer[i + 2]; value[0].c[3] = buffer[i + 3];
		value[1].c[0] = buffer[i + 4]; value[1].c[1] = buffer[i + 5]; value[1].c[2] = buffer[i + 6]; value[1].c[3] = buffer[i + 7];
		value[2].c[0] = buffer[i + 8]; value[2].c[1] = buffer[i + 9]; value[2].c[2] = buffer[i + 10]; value[2].c[3] = buffer[i + 11];
		positions.push_back({ value[0].flt , value[1].flt , value[2].flt });

		if (numElements & 8 && ret == "UMDL") {
			value[0].c[0] = buffer[i + 28]; value[0].c[1] = buffer[i + 29]; value[0].c[2] = buffer[i + 30]; value[0].c[3] = buffer[i + 31];
			value[1].c[0] = buffer[i + 32]; value[1].c[1] = buffer[i + 33]; value[1].c[2] = buffer[i + 34]; value[1].c[3] = buffer[i + 35];
		}else {
			value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
			value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
		}
		uvCoords.push_back({ value[0].flt , value[1].flt });

		value[0].c[0] = buffer[i + 12]; value[0].c[1] = buffer[i + 13]; value[0].c[2] = buffer[i + 14]; value[0].c[3] = buffer[i + 15];
		value[1].c[0] = buffer[i + 16]; value[1].c[1] = buffer[i + 17]; value[1].c[2] = buffer[i + 18]; value[1].c[3] = buffer[i + 19];
		value[2].c[0] = buffer[i + 20]; value[2].c[1] = buffer[i + 21]; value[2].c[2] = buffer[i + 22]; value[2].c[3] = buffer[i + 23];
		normals.push_back({ value[0].flt , value[1].flt , value[2].flt });	
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	//std::cout << "Num IndexBuffer: " << Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]) << std::endl;

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	//std::cout << "Index Count: " << indexCount << std::endl;

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexSize = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	

	buffer = new char[indexCount * indexSize];
	file.read(buffer, indexCount * indexSize);

	for (unsigned int i = 0; i < indexCount * indexSize; i = i + indexSize * 3) {
		Utils::UShort value[3];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt, value[1].shrt, value[2].shrt });
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	unsigned int numGeometries = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	for (unsigned int i = 0; i < numGeometries; ++i){
		file.read(metaData, sizeof(unsigned int));
		unsigned int boneMappingCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		if (boneMappingCount)
			file.ignore(boneMappingCount * sizeof(unsigned int));
		
		file.read(metaData, sizeof(unsigned int));
		unsigned int numLodLevels = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		for (unsigned int j = 0; j < numLodLevels; ++j){
			file.read(metaData, sizeof(float));
			float lodDistance = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);			
			file.read(metaData, sizeof(unsigned int));
			file.read(metaData, sizeof(unsigned int));
			unsigned int vbRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			unsigned int ibRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			unsigned int drawStart = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			unsigned int drawCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			geomteries[std::make_pair(i, j)] = std::make_pair(drawStart / 3, drawCount / 3);
		}
	}
	
	short maxIndex = 0, minIndex = positions.size();
	for (const std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> &geometrie : geomteries) {
		if (geometrie.first.second == 0u ) {
			for (unsigned int i = geometrie.second.first; i < geometrie.second.first + geometrie.second.second; i++) {
				facesLod.push_back(faces[i]);
				maxIndex = std::max(maxIndex, faces[i][0]);
				maxIndex = std::max(maxIndex, faces[i][1]);
				maxIndex = std::max(maxIndex, faces[i][2]);

				minIndex = std::min(minIndex, faces[i][0]);
				minIndex = std::min(minIndex, faces[i][1]);
				minIndex = std::min(minIndex, faces[i][2]);
			}
		}
	}
	file.close();

	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";
	fileOut << "mtllib " << mltPath.filename().string() << std::endl;

	for (int i = 0; i < positions.size(); i++) {
		fileOut << "v " << positions[i][0] << " " << positions[i][1] << " " << positions[i][2] << std::endl;
	}

	for (int i = 0; i < normals.size(); i++) {
		fileOut << "vn " << normals[i][0] << " " << normals[i][1] << " " << normals[i][2] << std::endl;
	}

	for (int i = 0; i < uvCoords.size(); i++) {
		fileOut << "vt " << uvCoords[i][0] << " " << uvCoords[i][1] << std::endl;
	}

	fileOut << "usemtl Material\n";
	for (int i = 0; i < facesLod.size(); i++) {
		fileOut << "f " << facesLod[i][0] + 1 << "/" << facesLod[i][0] + 1 << "/" << facesLod[i][0] + 1 << " " << facesLod[i][1] + 1 << "/" << facesLod[i][1] + 1 << "/" << facesLod[i][1] + 1 << " " << facesLod[i][2] + 1 << "/" << facesLod[i][2] + 1 << "/" << facesLod[i][2] + 1 << std::endl;
	}

	fileOut.close();

	fileOut.open(outFileMtl);
	fileOut << std::setprecision(6) << std::fixed;
	fileOut << "newmtl Material\n";
	fileOut << "Ns 10.000000\n";

	fileOut << "Ka 0.000000 0.000000 0.000000\n";
	fileOut << "Kd 1.000000 1.000000 1.000000\n";
	fileOut << "Ks 0.000000 0.000000 0.000000\n";
	fileOut << "Ni 1.000000\n";
	fileOut << "d 1.000000\n";
	fileOut << "illum 1\n";
	std::string absPath = std::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '\\', '/');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();
}

void Utils::MdlIO::mdlToBuffer(const char* filename, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	mdlToBuffer(filename, { scale, scale, scale }, vertexBufferOut, indexBufferOut);
}

void Utils::MdlIO::mdlToBuffer(const char* filename, std::array<float, 3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	std::vector<std::array<short, 3>> faces;
	std::map<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> geomteries;
	
	std::ifstream file(filename, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));

	char metaData[4];
	file.read(metaData, sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numElements = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	unsigned int vertexSize = 0;

	if (ret == "UMD2") {
		for (unsigned j = 0; j < numElements; ++j) {
			file.read(metaData, sizeof(unsigned int));
			unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			VertexElementSemantic semantic = (VertexElementSemantic)((elementDesc >> 8) & 0xff);
			vertexSize += Utils::ELEMENT_TYPESIZES[elementDesc & 0xff];
		}
	}else {
		if (numElements & 1)
			vertexSize += sizeof(float) * 3;
		if (numElements & 2)
			vertexSize += sizeof(float) * 3;		
		if (numElements & 4)
			vertexSize += 4;
		if (numElements & 8)	
			vertexSize += sizeof(float) * 2;
		if (numElements & 16)
			vertexSize += sizeof(float) * 2;
		if (numElements & 32)
			vertexSize += sizeof(float) * 3;
		if (numElements & 64)
			vertexSize += sizeof(float) * 3;		
		if (numElements & 128)
			vertexSize += sizeof(float) * 4;
		if (numElements & 256)
			vertexSize += sizeof(float) * 4;
		if (numElements & 512)
			vertexSize += 4;
	}

	//morph
	file.read(metaData, sizeof(unsigned int));
	file.read(metaData, sizeof(unsigned int));

	char* buffer = new char[vertexCount * vertexSize];
	file.read(buffer, vertexCount * vertexSize);

	for (unsigned int i = 0; i < vertexCount * vertexSize; i = i + vertexSize) {

		UFloat value[3];
		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1]; value[0].c[2] = buffer[i + 2]; value[0].c[3] = buffer[i + 3];
		value[1].c[0] = buffer[i + 4]; value[1].c[1] = buffer[i + 5]; value[1].c[2] = buffer[i + 6]; value[1].c[3] = buffer[i + 7];
		value[2].c[0] = buffer[i + 8]; value[2].c[1] = buffer[i + 9]; value[2].c[2] = buffer[i + 10]; value[2].c[3] = buffer[i + 11];
		vertexBufferOut.push_back(value[0].flt * scale[0]); vertexBufferOut.push_back(value[1].flt * scale[1]); vertexBufferOut.push_back(value[2].flt * scale[2]);

		if (numElements & 8 && ret == "UMDL" && vertexSize == 52) {
			value[0].c[0] = buffer[i + 28]; value[0].c[1] = buffer[i + 29]; value[0].c[2] = buffer[i + 30]; value[0].c[3] = buffer[i + 31];
			value[1].c[0] = buffer[i + 32]; value[1].c[1] = buffer[i + 33]; value[1].c[2] = buffer[i + 34]; value[1].c[3] = buffer[i + 35];
		}else {
			value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
			value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
		}
		
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

		value[0].c[0] = buffer[i + 12]; value[0].c[1] = buffer[i + 13]; value[0].c[2] = buffer[i + 14]; value[0].c[3] = buffer[i + 15];
		value[1].c[0] = buffer[i + 16]; value[1].c[1] = buffer[i + 17]; value[1].c[2] = buffer[i + 18]; value[1].c[3] = buffer[i + 19];
		value[2].c[0] = buffer[i + 20]; value[2].c[1] = buffer[i + 21]; value[2].c[2] = buffer[i + 22]; value[2].c[3] = buffer[i + 23];
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt); vertexBufferOut.push_back(value[2].flt);

	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexSize = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	buffer = new char[indexCount * indexSize];
	file.read(buffer, indexCount * indexSize);

	for (unsigned int i = 0; i < indexCount * indexSize; i = i + indexSize * 3) {
		Utils::UShort value[3];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt, value[1].shrt, value[2].shrt });
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	unsigned int numGeometries = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	for (unsigned int i = 0; i < numGeometries; ++i) {
		file.read(metaData, sizeof(unsigned int));
		unsigned int boneMappingCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		if (boneMappingCount)
			file.ignore(boneMappingCount * sizeof(unsigned int));

		file.read(metaData, sizeof(unsigned int));
		unsigned int numLodLevels = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		for (unsigned int j = 0; j < numLodLevels; ++j) {
			file.read(metaData, sizeof(float));
			float lodDistance = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			file.read(metaData, sizeof(unsigned int));
			unsigned int vbRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			unsigned int ibRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			unsigned int drawStart = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			unsigned int drawCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			geomteries[std::make_pair(i, j)] = std::make_pair(drawStart / 3, drawCount / 3);
		}
	}

	for (const std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>> &geometrie : geomteries) {
		if (geometrie.first.second == 0u) {
			for (unsigned int i = geometrie.second.first; i < geometrie.second.first + geometrie.second.second; i++) {			
				indexBufferOut.push_back(faces[i][0]); indexBufferOut.push_back(faces[i][1]); indexBufferOut.push_back(faces[i][2]);
			}
		}
	}

	file.close();
}

void Utils::MdlIO::mdlToBuffer(const char* path, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float, 4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<MeshBone>& bones, BoundingBox& boundingBox) {
	mdlToBuffer(path, { scale, scale, scale }, vertexBufferOut, indexBufferOut, weightsOut, boneIdsOut, bones, boundingBox);
}

void Utils::MdlIO::mdlToBuffer(const char* path, std::array<float,3> _scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float, 4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<MeshBone>& bones, BoundingBox& boundingBox) {	
	std::vector<std::array<short, 3>> faces;
	std::map<std::pair<unsigned int, unsigned int>, std::tuple<unsigned int, unsigned int, std::vector<unsigned int>>> geomteries;
	
	std::ifstream file(path, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));
	char metaData[4];

	//Num VertexBuffer
	file.read(metaData, sizeof(unsigned int));
	Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	
	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numElements = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	
	unsigned int vertexSize = 0;

	if (ret == "UMD2") {
		for (unsigned j = 0; j < numElements; ++j) {
			file.read(metaData, sizeof(unsigned int));
			unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			VertexElementSemantic semantic = (VertexElementSemantic)((elementDesc >> 8) & 0xff);
			vertexSize += Utils::ELEMENT_TYPESIZES[elementDesc & 0xff];
		}
	}else {
		if (numElements & 1)
			vertexSize += sizeof(float) * 3;
		if (numElements & 2)
			vertexSize += sizeof(float) * 3;
		if (numElements & 4)
			vertexSize += 4;
		if (numElements & 8)
			vertexSize += sizeof(float) * 2;
		if (numElements & 16)
			vertexSize += sizeof(float) * 2;
		if (numElements & 32)
			vertexSize += sizeof(float) * 3;
		if (numElements & 64)
			vertexSize += sizeof(float) * 3;
		if (numElements & 128)
			vertexSize += sizeof(float) * 4;
		if (numElements & 256)
			vertexSize += sizeof(float) * 4;
		if (numElements & 512)
			vertexSize += 4;
	}

	file.read(metaData, sizeof(unsigned int));
	file.read(metaData, sizeof(unsigned int));

	char* buffer = new char[vertexCount * vertexSize];
	file.read(buffer, vertexCount * vertexSize);

	for (unsigned int i = 0; i < vertexCount * vertexSize; i = i + vertexSize) {

		Utils::UFloat value[4];
		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1]; value[0].c[2] = buffer[i + 2]; value[0].c[3] = buffer[i + 3];
		value[1].c[0] = buffer[i + 4]; value[1].c[1] = buffer[i + 5]; value[1].c[2] = buffer[i + 6]; value[1].c[3] = buffer[i + 7];
		value[2].c[0] = buffer[i + 8]; value[2].c[1] = buffer[i + 9]; value[2].c[2] = buffer[i + 10]; value[2].c[3] = buffer[i + 11];
		vertexBufferOut.push_back(value[0].flt * _scale[0]); vertexBufferOut.push_back(value[1].flt * _scale[1]); vertexBufferOut.push_back(value[2].flt * _scale[2]);

		if (numElements & 8 && ret == "UMDL") {
			value[0].c[0] = buffer[i + 28]; value[0].c[1] = buffer[i + 29]; value[0].c[2] = buffer[i + 30]; value[0].c[3] = buffer[i + 31];
			value[1].c[0] = buffer[i + 32]; value[1].c[1] = buffer[i + 33]; value[1].c[2] = buffer[i + 34]; value[1].c[3] = buffer[i + 35];
		}else {
			value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
			value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
		}
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

		value[0].c[0] = buffer[i + 12]; value[0].c[1] = buffer[i + 13]; value[0].c[2] = buffer[i + 14]; value[0].c[3] = buffer[i + 15];
		value[1].c[0] = buffer[i + 16]; value[1].c[1] = buffer[i + 17]; value[1].c[2] = buffer[i + 18]; value[1].c[3] = buffer[i + 19];
		value[2].c[0] = buffer[i + 20]; value[2].c[1] = buffer[i + 21]; value[2].c[2] = buffer[i + 22]; value[2].c[3] = buffer[i + 23];
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt); vertexBufferOut.push_back(value[2].flt);

		value[0].c[0] = buffer[i + vertexSize - 20]; value[0].c[1] = buffer[i + vertexSize - 19]; value[0].c[2] = buffer[i + vertexSize - 18]; value[0].c[3] = buffer[i + vertexSize - 17];
		value[1].c[0] = buffer[i + vertexSize - 16]; value[1].c[1] = buffer[i + vertexSize - 15]; value[1].c[2] = buffer[i + vertexSize - 14]; value[1].c[3] = buffer[i + vertexSize - 13];
		value[2].c[0] = buffer[i + vertexSize - 12]; value[2].c[1] = buffer[i + vertexSize - 11]; value[2].c[2] = buffer[i + vertexSize - 10]; value[2].c[3] = buffer[i + vertexSize - 9];
		value[3].c[0] = buffer[i + vertexSize - 8]; value[3].c[1] = buffer[i + vertexSize - 7]; value[3].c[2] = buffer[i + vertexSize - 6]; value[3].c[3] = buffer[i + vertexSize - 5];
		weightsOut.push_back({ value[0].flt , value[1].flt , value[2].flt ,  value[3].flt });
		boneIdsOut.push_back({ static_cast<unsigned int>(buffer[i + vertexSize - 4]),  static_cast<unsigned int>(buffer[i + vertexSize - 3]), static_cast<unsigned int>(buffer[i + vertexSize - 2]), static_cast<unsigned int>(buffer[i + vertexSize - 1]) });
	}
	delete buffer;

	// Num IndexBuffer
	file.read(metaData, sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexSize = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	buffer = new char[indexCount * indexSize];
	file.read(buffer, indexCount * indexSize);
	
	for (unsigned int i = 0; i < indexCount * indexSize; i = i + indexSize * 3) {
		Utils::UShort value[3];
		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];
		faces.push_back({ value[0].shrt, value[1].shrt, value[2].shrt });
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	unsigned int numGeometries = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	for (unsigned int i = 0; i < numGeometries; ++i) {
		std::vector<unsigned int> boneMap;

		file.read(metaData, sizeof(unsigned int));
		unsigned int boneMappingCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		buffer = new char[boneMappingCount * sizeof(unsigned int)];
		if (boneMappingCount) {
			file.read(buffer, boneMappingCount * sizeof(unsigned int));
			for (unsigned int i = 0; i < boneMappingCount * sizeof(unsigned int); i = i + sizeof(unsigned int)) {
				Utils::UUint value;
				value.c[0] = buffer[i + 0]; value.c[1] = buffer[i + 1]; value.c[2] = buffer[i + 2]; value.c[3] = buffer[i + 3];			
				boneMap.push_back(value.unt);				
			}
		}
		delete buffer;

		file.read(metaData, sizeof(unsigned int));
		unsigned int numLodLevels = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		for (unsigned int j = 0; j < numLodLevels; ++j) {
			file.read(metaData, sizeof(float));
			float lodDistance = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			file.read(metaData, sizeof(unsigned int));
			unsigned int vbRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			unsigned int ibRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			unsigned int drawStart = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			file.read(metaData, sizeof(unsigned int));
			unsigned int drawCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			geomteries[std::make_pair(i, j)] = std::make_tuple(drawStart / 3, drawCount / 3, boneMap);
		}
	}
	std::set<short> processedVertices;

	for (const std::pair<std::pair<unsigned int, unsigned int>, std::tuple<unsigned int, unsigned int, std::vector<unsigned int>>> &geometrie : geomteries) {
		if (geometrie.first.second == 0u) {
			for (unsigned int i = std::get<0>(geometrie.second); i < std::get<0>(geometrie.second) + std::get<1>(geometrie.second); i++) {
				indexBufferOut.push_back(faces[i][0]); indexBufferOut.push_back(faces[i][1]); indexBufferOut.push_back(faces[i][2]);
				if (!std::get<2>(geometrie.second).empty()) {
					if (processedVertices.find(faces[i][0]) == processedVertices.end()) {
						processedVertices.insert(faces[i][0]);
						boneIdsOut[faces[i][0]][0] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][0]][0]];
						boneIdsOut[faces[i][0]][1] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][0]][1]];
						boneIdsOut[faces[i][0]][2] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][0]][2]];
						boneIdsOut[faces[i][0]][3] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][0]][3]];
					}

					if (processedVertices.find(faces[i][1]) == processedVertices.end()) {
						processedVertices.insert(faces[i][1]);
						boneIdsOut[faces[i][1]][0] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][1]][0]];
						boneIdsOut[faces[i][1]][1] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][1]][1]];
						boneIdsOut[faces[i][1]][2] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][1]][2]];
						boneIdsOut[faces[i][1]][3] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][1]][3]];
					}

					if (processedVertices.find(faces[i][2]) == processedVertices.end()) {
						processedVertices.insert(faces[i][2]);
						boneIdsOut[faces[i][2]][0] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][2]][0]];
						boneIdsOut[faces[i][2]][1] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][2]][1]];
						boneIdsOut[faces[i][2]][2] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][2]][2]];
						boneIdsOut[faces[i][2]][3] = std::get<2>(geometrie.second)[boneIdsOut[faces[i][2]][3]];
					}
				}
			}
		}
	}

	//Num Morphs
	file.read(metaData, sizeof(unsigned int));
	unsigned int numMorphs = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numBones = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	bones.resize(numBones);
	
	for (size_t i = 0; i < numBones; ++i) {
		MeshBone& bone = bones[i];

		std::string boneName;
		while (true) {
			file.read(metaData, sizeof(char));
			if (!metaData[0])
				break;
			else
				boneName += metaData[0];
		}
		//std::cout << "Name: " << boneName << std::endl;
		bone.name = boneName;
		bone.nameHash = StringHash(bone.name);

		file.read(metaData, sizeof(unsigned int));
		bone.parentIndex = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		char* bufferBoneTrans = new char[48];

		file.read(bufferBoneTrans, 12);
		Utils::UFloat position[3];
		position[0].c[0] = bufferBoneTrans[0]; position[0].c[1] = bufferBoneTrans[1]; position[0].c[2] = bufferBoneTrans[2]; position[0].c[3] = bufferBoneTrans[3];
		position[1].c[0] = bufferBoneTrans[4]; position[1].c[1] = bufferBoneTrans[5]; position[1].c[2] = bufferBoneTrans[6]; position[1].c[3] = bufferBoneTrans[7];
		position[2].c[0] = bufferBoneTrans[8]; position[2].c[1] = bufferBoneTrans[9]; position[2].c[2] = bufferBoneTrans[10]; position[2].c[3] = bufferBoneTrans[11];
		bone.initialPosition.set(position[0].flt, position[1].flt, position[2].flt);

		file.read(bufferBoneTrans, 16);
		Utils::UFloat orientation[4];
		orientation[0].c[0] = bufferBoneTrans[0]; orientation[0].c[1] = bufferBoneTrans[1]; orientation[0].c[2] = bufferBoneTrans[2]; orientation[0].c[3] = bufferBoneTrans[3];
		orientation[1].c[0] = bufferBoneTrans[4]; orientation[1].c[1] = bufferBoneTrans[5]; orientation[1].c[2] = bufferBoneTrans[6]; orientation[1].c[3] = bufferBoneTrans[7];
		orientation[2].c[0] = bufferBoneTrans[8]; orientation[2].c[1] = bufferBoneTrans[9]; orientation[2].c[2] = bufferBoneTrans[10]; orientation[2].c[3] = bufferBoneTrans[11];
		orientation[3].c[0] = bufferBoneTrans[12]; orientation[3].c[1] = bufferBoneTrans[13]; orientation[3].c[2] = bufferBoneTrans[14]; orientation[3].c[3] = bufferBoneTrans[15];
		bone.initialRotation.set(orientation[1].flt, orientation[2].flt, orientation[3].flt, orientation[0].flt);

		file.read(bufferBoneTrans, 12);
		Utils::UFloat scale[3];
		scale[0].c[0] = bufferBoneTrans[0]; scale[0].c[1] = bufferBoneTrans[1]; scale[0].c[2] = bufferBoneTrans[2]; scale[0].c[3] = bufferBoneTrans[3];
		scale[1].c[0] = bufferBoneTrans[4]; scale[1].c[1] = bufferBoneTrans[5]; scale[1].c[2] = bufferBoneTrans[6]; scale[1].c[3] = bufferBoneTrans[7];
		scale[2].c[0] = bufferBoneTrans[8]; scale[2].c[1] = bufferBoneTrans[9]; scale[2].c[2] = bufferBoneTrans[10]; scale[2].c[3] = bufferBoneTrans[11];
		bone.initialScale.set(scale[0].flt, scale[1].flt, scale[2].flt);

		file.read(bufferBoneTrans, 48);
		Utils::UFloat offset[12];
		offset[0].c[0] = bufferBoneTrans[0]; offset[0].c[1] = bufferBoneTrans[1]; offset[0].c[2] = bufferBoneTrans[2]; offset[0].c[3] = bufferBoneTrans[3];
		offset[1].c[0] = bufferBoneTrans[4]; offset[1].c[1] = bufferBoneTrans[5]; offset[1].c[2] = bufferBoneTrans[6]; offset[1].c[3] = bufferBoneTrans[7];
		offset[2].c[0] = bufferBoneTrans[8]; offset[2].c[1] = bufferBoneTrans[9]; offset[2].c[2] = bufferBoneTrans[10]; offset[2].c[3] = bufferBoneTrans[11];
		offset[3].c[0] = bufferBoneTrans[12]; offset[3].c[1] = bufferBoneTrans[13]; offset[3].c[2] = bufferBoneTrans[14]; offset[3].c[3] = bufferBoneTrans[15];

		offset[4].c[0] = bufferBoneTrans[16]; offset[4].c[1] = bufferBoneTrans[17]; offset[4].c[2] = bufferBoneTrans[18]; offset[4].c[3] = bufferBoneTrans[19];
		offset[5].c[0] = bufferBoneTrans[20]; offset[5].c[1] = bufferBoneTrans[21]; offset[5].c[2] = bufferBoneTrans[22]; offset[5].c[3] = bufferBoneTrans[23];
		offset[6].c[0] = bufferBoneTrans[24]; offset[6].c[1] = bufferBoneTrans[25]; offset[6].c[2] = bufferBoneTrans[26]; offset[6].c[3] = bufferBoneTrans[27];
		offset[7].c[0] = bufferBoneTrans[28]; offset[7].c[1] = bufferBoneTrans[29]; offset[7].c[2] = bufferBoneTrans[30]; offset[7].c[3] = bufferBoneTrans[31];

		offset[8].c[0] = bufferBoneTrans[32]; offset[8].c[1] = bufferBoneTrans[33]; offset[8].c[2] = bufferBoneTrans[34]; offset[8].c[3] = bufferBoneTrans[35];
		offset[9].c[0] = bufferBoneTrans[36]; offset[9].c[1] = bufferBoneTrans[37]; offset[9].c[2] = bufferBoneTrans[38]; offset[9].c[3] = bufferBoneTrans[39];
		offset[10].c[0] = bufferBoneTrans[40]; offset[10].c[1] = bufferBoneTrans[41]; offset[10].c[2] = bufferBoneTrans[42]; offset[10].c[3] = bufferBoneTrans[43];
		offset[11].c[0] = bufferBoneTrans[44]; offset[11].c[1] = bufferBoneTrans[45]; offset[11].c[2] = bufferBoneTrans[46]; offset[11].c[3] = bufferBoneTrans[47];

		bone.offsetMatrix.set(offset[0].flt, offset[4].flt, offset[8].flt, 0.0f,
			offset[1].flt, offset[5].flt, offset[9].flt, 0.0f,
			offset[2].flt, offset[6].flt, offset[10].flt, 0.0f,
			offset[3].flt, offset[7].flt, offset[11].flt, 1.0f);

		delete bufferBoneTrans;

		file.read(metaData, sizeof(unsigned char));
		unsigned char boneCollisionType = metaData[0];

		if (boneCollisionType & 1) {
			file.read(metaData, sizeof(float));
			bone.radius = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			if (bone.radius < BONE_SIZE_THRESHOLD * 0.5f)
				bone.active = false;
		}

		if (boneCollisionType & 2) {
			char* bufferBox = new char[24];
			file.read(bufferBox, 24);
			Utils::UFloat box[6];
			box[0].c[0] = bufferBox[0]; box[0].c[1] = bufferBox[1]; box[0].c[2] = bufferBox[2]; box[0].c[3] = bufferBox[3];
			box[1].c[0] = bufferBox[4]; box[1].c[1] = bufferBox[5]; box[1].c[2] = bufferBox[6]; box[1].c[3] = bufferBox[7];
			box[2].c[0] = bufferBox[8]; box[2].c[1] = bufferBox[9]; box[2].c[2] = bufferBox[10]; box[2].c[3] = bufferBox[11];

			box[3].c[0] = bufferBox[12]; box[3].c[1] = bufferBox[13]; box[3].c[2] = bufferBox[14]; box[3].c[3] = bufferBox[15];
			box[4].c[0] = bufferBox[16]; box[4].c[1] = bufferBox[17]; box[4].c[2] = bufferBox[18]; box[4].c[3] = bufferBox[19];
			box[5].c[0] = bufferBox[20]; box[5].c[1] = bufferBox[21]; box[5].c[2] = bufferBox[22]; box[5].c[3] = bufferBox[23];

			bone.boundingBox.min.set(box[0].flt, box[1].flt, box[2].flt);
			bone.boundingBox.max.set(box[3].flt, box[4].flt, box[5].flt);
			delete bufferBox;
			if (bone.boundingBox.getSize().length() < BONE_SIZE_THRESHOLD)
				bone.active = false;
		}
	}

	char* bufferBox = new char[24];
	file.read(bufferBox, 24);
	Utils::UFloat box[6];
	box[0].c[0] = bufferBox[0]; box[0].c[1] = bufferBox[1]; box[0].c[2] = bufferBox[2]; box[0].c[3] = bufferBox[3];
	box[1].c[0] = bufferBox[4]; box[1].c[1] = bufferBox[5]; box[1].c[2] = bufferBox[6]; box[1].c[3] = bufferBox[7];
	box[2].c[0] = bufferBox[8]; box[2].c[1] = bufferBox[9]; box[2].c[2] = bufferBox[10]; box[2].c[3] = bufferBox[11];

	box[3].c[0] = bufferBox[12]; box[3].c[1] = bufferBox[13]; box[3].c[2] = bufferBox[14]; box[3].c[3] = bufferBox[15];
	box[4].c[0] = bufferBox[16]; box[4].c[1] = bufferBox[17]; box[4].c[2] = bufferBox[18]; box[4].c[3] = bufferBox[19];
	box[5].c[0] = bufferBox[20]; box[5].c[1] = bufferBox[21]; box[5].c[2] = bufferBox[22]; box[5].c[3] = bufferBox[23];

	boundingBox.min.set(box[0].flt, box[1].flt, box[2].flt);
	boundingBox.max.set(box[3].flt, box[4].flt, box[5].flt);

	delete bufferBox;

	file.close();
}

void Utils::VbmIO::vbmToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath) {
	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<float, 3>> normals;
	std::vector <std::array<float, 2>> uvCoords;

	std::ifstream file(path, std::ios::binary);

	VBM_HEADER header;
	file.read(reinterpret_cast<char*>(&header), sizeof(VBM_HEADER));
	std::vector<VBM_ATTRIB_HEADER> attribHeaders;
	std::vector<VBM_FRAME_HEADER> frameHeaders;
	
	for (int i = 0; i < header.num_attribs; i++) {
		VBM_ATTRIB_HEADER attribHeader;
		file.read(reinterpret_cast<char*>(&attribHeader), sizeof(VBM_ATTRIB_HEADER));
		attribHeaders.push_back(attribHeader);
	}

	for (int i = 0; i < header.num_frames; i++) {
		VBM_FRAME_HEADER frameHeader;
		file.read(reinterpret_cast<char*>(&frameHeader), sizeof(VBM_FRAME_HEADER));
		frameHeaders.push_back(frameHeader);
	}
	unsigned int vertexSize = 0;
	for (int i = 0; i < header.num_attribs; i++) {
		vertexSize += attribHeaders[i].components;
	}
	vertexSize *= sizeof(float);

	char* buffer = new char[header.num_vertices * vertexSize];
	file.read(buffer, header.num_vertices * vertexSize);

	for (unsigned int i = 0; i < header.num_vertices; i++) {

		Utils::UFloat value[4];
		value[0].c[0] = buffer[i * 16 + 0]; value[0].c[1] = buffer[i * 16 + 1]; value[0].c[2] = buffer[i * 16 + 2]; value[0].c[3] = buffer[i * 16 + 3];
		value[1].c[0] = buffer[i * 16 + 4]; value[1].c[1] = buffer[i * 16 + 5]; value[1].c[2] = buffer[i * 16 + 6]; value[1].c[3] = buffer[i * 16 + 7];
		value[2].c[0] = buffer[i * 16 + 8]; value[2].c[1] = buffer[i * 16 + 9]; value[2].c[2] = buffer[i * 16 + 10]; value[2].c[3] = buffer[i * 16 + 11];
		value[3].c[0] = buffer[i * 16 + 12]; value[3].c[1] = buffer[i * 16 + 13]; value[3].c[2] = buffer[i * 16 + 14]; value[3].c[3] = buffer[i * 16 + 15];
		positions.push_back({ value[0].flt, value[1].flt, value[2].flt }); 

		value[0].c[0] = buffer[header.num_vertices * 16 + i * 12 + 0]; value[0].c[1] = buffer[header.num_vertices * 16 + i * 12 + 1]; value[0].c[2] = buffer[header.num_vertices * 16 + i * 12 + 2]; value[0].c[3] = buffer[header.num_vertices * 16 + i * 12 + 3];
		value[1].c[0] = buffer[header.num_vertices * 16 + i * 12 + 4]; value[1].c[1] = buffer[header.num_vertices * 16 + i * 12 + 5]; value[1].c[2] = buffer[header.num_vertices * 16 + i * 12 + 6]; value[1].c[3] = buffer[header.num_vertices * 16 + i * 12 + 7];
		value[2].c[0] = buffer[header.num_vertices * 16 + i * 12 + 8]; value[2].c[1] = buffer[header.num_vertices * 16 + i * 12 + 9]; value[2].c[2] = buffer[header.num_vertices * 16 + i * 12 + 10]; value[2].c[3] = buffer[header.num_vertices * 16 + i * 12 + 11];
		normals.push_back({ value[0].flt, value[1].flt, value[2].flt });

		value[0].c[0] = buffer[header.num_vertices * 28 + i * 8 + 0]; value[0].c[1] = buffer[header.num_vertices * 28 + i * 8 + 1]; value[0].c[2] = buffer[header.num_vertices * 28 + i * 8 + 2]; value[0].c[3] = buffer[header.num_vertices * 28 + i * 8 + 3];
		value[1].c[0] = buffer[header.num_vertices * 28 + i * 8 + 4]; value[1].c[1] = buffer[header.num_vertices * 28 + i * 8 + 5]; value[1].c[2] = buffer[header.num_vertices * 28 + i * 8 + 6]; value[1].c[3] = buffer[header.num_vertices * 28 + i * 8 + 7];
		uvCoords.push_back({ value[0].flt, value[1].flt });
	}
	delete buffer;
	
	file.close();

	std::filesystem::path mltPath(outFileMtl);
	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";
	fileOut << "mtllib " << mltPath.filename().string() << std::endl;

	for (unsigned int i = 0; i < positions.size(); i++) {
		fileOut << "v " << positions[i][0] << " " << positions[i][1] << " " << positions[i][2] << std::endl;
	}

	for (unsigned int i = 0; i < normals.size(); i++) {
		fileOut << "vn " << normals[i][0] << " " << normals[i][1] << " " << normals[i][2] << std::endl;
	}

	for (unsigned int i = 0; i < uvCoords.size(); i++) {
		fileOut << "vt " << uvCoords[i][0] << " " << uvCoords[i][1] << std::endl;
	}
	fileOut << "usemtl Material\n";
	for (unsigned int i = 0; i < header.num_vertices / 3; i++) {
		fileOut << "f " << i * 3 + 1 << "/" << i * 3 + 1 << "/" << i * 3 + 1 << " " << i * 3 + 2 << "/" << i * 3 + 2 << "/" << i * 3 + 2 << " " << i * 3 + 3 << "/" << i * 3 + 3 << "/" << i * 3 + 3 << std::endl;
	}

	fileOut.close();

	fileOut.open(outFileMtl);
	fileOut << std::setprecision(6) << std::fixed;
	fileOut << "newmtl Material\n";
	fileOut << "Ns 10.000000\n";

	fileOut << "Ka 0.000000 0.000000 0.000000\n";
	fileOut << "Kd 1.000000 1.000000 1.000000\n";
	fileOut << "Ks 0.000000 0.000000 0.000000\n";
	fileOut << "Ni 1.000000\n";
	fileOut << "d 1.000000\n";
	fileOut << "illum 1\n";
	std::string absPath = std::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '\\', '/');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();
}

void Utils::VbmIO::vbmToBuffer(const char* path, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	std::ifstream file(path, std::ios::binary);

	VBM_HEADER header;
	file.read(reinterpret_cast<char*>(&header), sizeof(VBM_HEADER));
	std::vector<VBM_ATTRIB_HEADER> attribHeaders;
	std::vector<VBM_FRAME_HEADER> frameHeaders;

	for (int i = 0; i < header.num_attribs; i++) {
		VBM_ATTRIB_HEADER attribHeader;
		file.read(reinterpret_cast<char*>(&attribHeader), sizeof(VBM_ATTRIB_HEADER));
		attribHeaders.push_back(attribHeader);
	}

	for (int i = 0; i < header.num_frames; i++) {
		VBM_FRAME_HEADER frameHeader;
		file.read(reinterpret_cast<char*>(&frameHeader), sizeof(VBM_FRAME_HEADER));
		frameHeaders.push_back(frameHeader);
	}
	unsigned int vertexSize = 0;
	for (int i = 0; i < header.num_attribs; i++) {
		vertexSize += attribHeaders[i].components;
	}
	vertexSize *= sizeof(float);

	char* buffer = new char[header.num_vertices * vertexSize];
	file.read(buffer, header.num_vertices * vertexSize);

	for (unsigned int i = 0; i < header.num_vertices; i++) {

		Utils::UFloat value[4];
		value[0].c[0] = buffer[i * 16 + 0]; value[0].c[1] = buffer[i * 16 + 1]; value[0].c[2] = buffer[i * 16 + 2]; value[0].c[3] = buffer[i * 16 + 3];
		value[1].c[0] = buffer[i * 16 + 4]; value[1].c[1] = buffer[i * 16 + 5]; value[1].c[2] = buffer[i * 16 + 6]; value[1].c[3] = buffer[i * 16 + 7];
		value[2].c[0] = buffer[i * 16 + 8]; value[2].c[1] = buffer[i * 16 + 9]; value[2].c[2] = buffer[i * 16 + 10]; value[2].c[3] = buffer[i * 16 + 11];
		value[3].c[0] = buffer[i * 16 + 12]; value[3].c[1] = buffer[i * 16 + 13]; value[3].c[2] = buffer[i * 16 + 14]; value[3].c[3] = buffer[i * 16 + 15];
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt); vertexBufferOut.push_back(value[2].flt);

		value[0].c[0] = buffer[header.num_vertices * 28 + i * 8 + 0]; value[0].c[1] = buffer[header.num_vertices * 28 + i * 8 + 1]; value[0].c[2] = buffer[header.num_vertices * 28 + i * 8 + 2]; value[0].c[3] = buffer[header.num_vertices * 28 + i * 8 + 3];
		value[1].c[0] = buffer[header.num_vertices * 28 + i * 8 + 4]; value[1].c[1] = buffer[header.num_vertices * 28 + i * 8 + 5]; value[1].c[2] = buffer[header.num_vertices * 28 + i * 8 + 6]; value[1].c[3] = buffer[header.num_vertices * 28 + i * 8 + 7];
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);


		value[0].c[0] = buffer[header.num_vertices * 16 + i * 12 + 0]; value[0].c[1] = buffer[header.num_vertices * 16 + i * 12 + 1]; value[0].c[2] = buffer[header.num_vertices * 16 + i * 12 + 2]; value[0].c[3] = buffer[header.num_vertices * 16 + i * 12 + 3];
		value[1].c[0] = buffer[header.num_vertices * 16 + i * 12 + 4]; value[1].c[1] = buffer[header.num_vertices * 16 + i * 12 + 5]; value[1].c[2] = buffer[header.num_vertices * 16 + i * 12 + 6]; value[1].c[3] = buffer[header.num_vertices * 16 + i * 12 + 7];
		value[2].c[0] = buffer[header.num_vertices * 16 + i * 12 + 8]; value[2].c[1] = buffer[header.num_vertices * 16 + i * 12 + 9]; value[2].c[2] = buffer[header.num_vertices * 16 + i * 12 + 10]; value[2].c[3] = buffer[header.num_vertices * 16 + i * 12 + 11];
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt); vertexBufferOut.push_back(value[2].flt);		
	}
	delete buffer;

	file.close();

	for (unsigned int i = 0; i < header.num_vertices / 3; i++) {
		indexBufferOut.push_back(i * 3); indexBufferOut.push_back(i * 3 + 1); indexBufferOut.push_back(i * 3 + 2);
	}
}

void Utils::MD2IO::md2ToObj(const char *path, const char* outFileObj, const char* outFileMtl, const char* texturePath, bool flipVertical, int frame) {
	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<short, 3>> faces;

	std::vector <std::array<float, 2>> uvCoords;
	std::vector<std::array<short, 3>> uvFaves;

	std::ifstream file(path, std::ios::binary);

	MD2Header header;
	file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));

	if (frame >= header.num_frames)
		return;

	char* buffer = new char[header.num_st * 2 * sizeof(short)];
	file.seekg(header.ofs_st, std::ios::beg);
	file.read(buffer, header.num_st * 2 * sizeof(short));

	for (int i = 0; i < header.num_st * 2 * sizeof(short); i = i + 2 * sizeof(short)) {
		Utils::UShort s, t;

		s.c[0] = buffer[i + 0]; s.c[1] = buffer[i + 1];
		t.c[0] = buffer[i + 2]; t.c[1] = buffer[i + 3];

		if (flipVertical)
			uvCoords.push_back({ (float)s.shrt / header.skinwidth , 1.0f - (float)t.shrt / header.skinheight });
		else
			uvCoords.push_back({ (float)s.shrt / header.skinwidth , (float)t.shrt / header.skinheight });
	}
	delete buffer;

	buffer = new char[header.num_tris * 6 * sizeof(short)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read(buffer, header.num_tris * 6 * sizeof(short));

	for (int i = 0; i < header.num_tris * 6 * sizeof(short); i = i + 6 * sizeof(short)) {
		Utils::UShort value[6];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt , value[1].shrt , value[2].shrt });

		value[3].c[0] = buffer[i + 6]; value[3].c[1] = buffer[i + 7];
		value[4].c[0] = buffer[i + 8]; value[4].c[1] = buffer[i + 9];
		value[5].c[0] = buffer[i + 10]; value[5].c[1] = buffer[i + 11];

		uvFaves.push_back({ value[3].shrt , value[4].shrt , value[5].shrt });
	}
	delete buffer;

	int framesize = 40 + header.num_xyz * 4 * sizeof(unsigned char);
	buffer = new char[framesize];

	int i = frame;

	file.seekg(header.ofs_frames + i * framesize, std::ios::beg);
	file.read(buffer, framesize);

	Utils::UFloat scale[3];

	scale[0].c[0] = buffer[0]; scale[0].c[1] = buffer[1]; scale[0].c[2] = buffer[2]; scale[0].c[3] = buffer[3];
	scale[1].c[0] = buffer[4]; scale[1].c[1] = buffer[5]; scale[1].c[2] = buffer[6]; scale[1].c[3] = buffer[7];
	scale[2].c[0] = buffer[8]; scale[2].c[1] = buffer[9]; scale[2].c[2] = buffer[10]; scale[2].c[3] = buffer[11];

	Utils::UFloat translate[3];

	translate[0].c[0] = buffer[12]; translate[0].c[1] = buffer[13]; translate[0].c[2] = buffer[14]; translate[0].c[3] = buffer[15];
	translate[1].c[0] = buffer[16]; translate[1].c[1] = buffer[17]; translate[1].c[2] = buffer[18]; translate[1].c[3] = buffer[19];
	translate[2].c[0] = buffer[20]; translate[2].c[1] = buffer[21]; translate[2].c[2] = buffer[22]; translate[2].c[3] = buffer[23];

	std::string name;
	name.resize(16);
	std::copy(buffer + 24, buffer + 40, name.begin());

	for (int k = 0; k < header.num_xyz; k++) {
		Utils::UShort vertexS[3];
		vertexS[0].c[0] = buffer[40 + k * 4 + 0]; vertexS[0].c[1] = 0;
		vertexS[1].c[0] = buffer[40 + k * 4 + 1]; vertexS[1].c[1] = 0;
		vertexS[2].c[0] = buffer[40 + k * 4 + 2]; vertexS[2].c[1] = 0;

		Utils::UFloat vertex[3];
		vertex[0].flt = vertexS[0].shrt;
		vertex[1].flt = vertexS[1].shrt;
		vertex[2].flt = vertexS[2].shrt;

		positions.push_back({ vertex[0].flt * scale[0].flt + translate[0].flt, vertex[2].flt * scale[2].flt + translate[2].flt, vertex[1].flt * scale[1].flt + translate[1].flt });
	}
	delete buffer;

	file.close();

	std::filesystem::path mltPath(outFileMtl);
	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";
	fileOut << "mtllib " << mltPath.filename().string() << std::endl;

	for (int i = 0; i < positions.size(); i++) {
		fileOut << "v " << positions[i][0] << " " << positions[i][1] << " " << positions[i][2] << std::endl;
	}
	for (int i = 0; i < uvCoords.size(); i++) {
		fileOut << "vt " << uvCoords[i][0] << " " << uvCoords[i][1] << std::endl;
	}
	fileOut << "usemtl Material\n";
	for (int i = 0; i < faces.size(); i++) {
		fileOut << "f " << faces[i][0] + 1 << "/" << uvFaves[i][0] + 1 << " " << faces[i][1] + 1 << "/" << uvFaves[i][1] + 1 << " " << faces[i][2] + 1 << "/" << uvFaves[i][2] + 1 << std::endl;
	}

	fileOut.close();

	fileOut.open(outFileMtl);
	fileOut << std::setprecision(6) << std::fixed;
	fileOut << "newmtl Material\n";
	fileOut << "Ns 10.000000\n";

	fileOut << "Ka 0.000000 0.000000 0.000000\n";
	fileOut << "Kd 1.000000 1.000000 1.000000\n";
	fileOut << "Ks 0.000000 0.000000 0.000000\n";
	fileOut << "Ni 1.000000\n";
	fileOut << "d 1.000000\n";
	fileOut << "illum 1\n";
	std::string absPath = std::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '\\', '/');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();
}

int Utils::MD2IO::addVertex(int hash, const float *pVertex, int stride, std::vector<float>& vertexBufferOut) {

	int index = -1;
	std::map<int, std::vector<int> >::const_iterator iter = m_vertexCache.find(hash);

	if (iter == m_vertexCache.end()) {
		// Vertex hash doesn't exist in the cache.
		index = static_cast<int>(vertexBufferOut.size() / stride);
		vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
		m_vertexCache.insert(std::make_pair(hash, std::vector<int>(1, index)));
	}
	else {
		// One or more vertices have been hashed to this entry in the cache.
		const std::vector<int> &vertices = iter->second;
		const float *pCachedVertex = 0;
		bool found = false;

		for (std::vector<int>::const_iterator i = vertices.begin(); i != vertices.end(); ++i) {
			index = *i;
			pCachedVertex = &vertexBufferOut[index * stride];

			if (memcmp(pCachedVertex, pVertex, sizeof(float)* stride) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			index = static_cast<int>(vertexBufferOut.size() / stride);
			vertexBufferOut.insert(vertexBufferOut.end(), pVertex, pVertex + stride);
			m_vertexCache[hash].push_back(index);
		}
	}
	return index;
}

void Utils::MD2IO::md2ToBuffer(const char* path, bool flipVertical, int frame, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	std::ifstream file(path, std::ios::binary);
	std::vector<std::array<short, 3>> faces;
	std::vector<std::array<short, 3>> uvFaves;
	std::vector<std::array<float, 3>> positions;
	std::vector <std::array<float, 2>> textureCoords;

	MD2Header header;
	file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));

	if (frame >= header.num_frames)
		return;

	char* buffer = new char[header.num_tris * 6 * sizeof(short)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read(buffer, header.num_tris * 6 * sizeof(short));

	for (int i = 0; i < header.num_tris * 6 * sizeof(short); i = i + 6 * sizeof(short)) {
		Utils::UShort value[6];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt , value[1].shrt , value[2].shrt });

		value[3].c[0] = buffer[i + 6]; value[3].c[1] = buffer[i + 7];
		value[4].c[0] = buffer[i + 8]; value[4].c[1] = buffer[i + 9];
		value[5].c[0] = buffer[i + 10]; value[5].c[1] = buffer[i + 11];

		uvFaves.push_back({ value[3].shrt , value[4].shrt , value[5].shrt });
	}
	delete buffer;

	buffer = new char[header.num_st * 2 * sizeof(short)];
	file.seekg(header.ofs_st, std::ios::beg);
	file.read(buffer, header.num_st * 2 * sizeof(short));

	for (int i = 0; i < header.num_st * 2 * sizeof(short); i = i + 2 * sizeof(short)) {
		Utils::UShort s, t;

		s.c[0] = buffer[i + 0]; s.c[1] = buffer[i + 1];
		t.c[0] = buffer[i + 2]; t.c[1] = buffer[i + 3];

		if (flipVertical)
			textureCoords.push_back({ (float)s.shrt / header.skinwidth , 1.0f - (float)t.shrt / header.skinheight });
		else
			textureCoords.push_back({ (float)s.shrt / header.skinwidth , (float)t.shrt / header.skinheight });
	}
	delete buffer;

	int framesize = 40 + header.num_xyz * 4 * sizeof(unsigned char);
	buffer = new char[framesize];

	int i = frame;

	file.seekg(header.ofs_frames + i * framesize, std::ios::beg);
	file.read(buffer, framesize);

	Utils::UFloat _scale[3];

	_scale[0].c[0] = buffer[0]; _scale[0].c[1] = buffer[1]; _scale[0].c[2] = buffer[2]; _scale[0].c[3] = buffer[3];
	_scale[1].c[0] = buffer[4]; _scale[1].c[1] = buffer[5]; _scale[1].c[2] = buffer[6]; _scale[1].c[3] = buffer[7];
	_scale[2].c[0] = buffer[8]; _scale[2].c[1] = buffer[9]; _scale[2].c[2] = buffer[10]; _scale[2].c[3] = buffer[11];

	Utils::UFloat translate[3];

	translate[0].c[0] = buffer[12]; translate[0].c[1] = buffer[13]; translate[0].c[2] = buffer[14]; translate[0].c[3] = buffer[15];
	translate[1].c[0] = buffer[16]; translate[1].c[1] = buffer[17]; translate[1].c[2] = buffer[18]; translate[1].c[3] = buffer[19];
	translate[2].c[0] = buffer[20]; translate[2].c[1] = buffer[21]; translate[2].c[2] = buffer[22]; translate[2].c[3] = buffer[23];

	std::string name;
	name.resize(16);
	std::copy(buffer + 24, buffer + 40, name.begin());

	for (int k = 0; k < header.num_xyz; k++) {
		Utils::UShort vertexS[3];
		vertexS[0].c[0] = buffer[40 + k * 4 + 0]; vertexS[0].c[1] = 0;
		vertexS[1].c[0] = buffer[40 + k * 4 + 1]; vertexS[1].c[1] = 0;
		vertexS[2].c[0] = buffer[40 + k * 4 + 2]; vertexS[2].c[1] = 0;

		Utils::UFloat vertex[3];
		vertex[0].flt = vertexS[0].shrt;
		vertex[1].flt = vertexS[1].shrt;
		vertex[2].flt = vertexS[2].shrt;

		std::array<float, 3> vert = Utils::ScalePoint(Utils::RotatePoint({ vertex[0].flt * _scale[0].flt + translate[0].flt, vertex[2].flt * _scale[2].flt + translate[2].flt, vertex[1].flt * _scale[1].flt + translate[1].flt }, eulerAngle[0], eulerAngle[1], eulerAngle[2]), scale[0], scale[1], scale[2]);

		positions.push_back({ vert[0], vert[1], vert[2] });
	}
	delete buffer;

	file.close();

	indexBufferOut.resize(faces.size() * 3);
	for (int i = 0; i < faces.size(); i++) {
		float vertex1[] = { positions[faces[i][0]][0], positions[faces[i][0]][1], positions[faces[i][0]][2],
                            textureCoords[uvFaves[i][0]][0], textureCoords[uvFaves[i][0]][1] };
		indexBufferOut[i * 3] = addVertex(faces[i][0], &vertex1[0], 5, vertexBufferOut);

		float vertex2[] = { positions[faces[i][1]][0], positions[faces[i][1]][1], positions[faces[i][1]][2],
                            textureCoords[uvFaves[i][1]][0], textureCoords[uvFaves[i][1]][1] };
		indexBufferOut[i * 3 + 1] = addVertex(faces[i][1], &vertex2[0], 5, vertexBufferOut);

		float vertex3[] = { positions[faces[i][2]][0], positions[faces[i][2]][1], positions[faces[i][2]][2],
                            textureCoords[uvFaves[i][2]][0], textureCoords[uvFaves[i][2]][1] };
		indexBufferOut[i * 3 + 2] = addVertex(faces[i][2], &vertex3[0], 5, vertexBufferOut);
	}

	faces.clear();
	faces.shrink_to_fit();
	uvFaves.clear();
	uvFaves.shrink_to_fit();
	positions.clear();
	positions.shrink_to_fit();
	textureCoords.clear();
	textureCoords.shrink_to_fit();

	std::map<int, std::vector<int>>().swap(m_vertexCache);
}

void Utils::MD2IO::md2ToSequence(const char* path, bool flipVertical, std::array<float, 3> eulerAngle, std::array<float, 3> scale, MeshSequence& sequenceOut) {
	std::ifstream file(path, std::ios::binary);
	std::vector<std::array<short, 3>> faces;
	std::vector<std::array<short, 3>> uvFaves;
	std::vector<std::array<float, 3>> positions;
	std::vector <std::array<float, 2>> textureCoords;
	std::vector<std::array<float, 3>> normals;

	std::vector<unsigned int> indexBuffer;
	std::vector<float> vertexBuffer;

	MD2Header header;
	file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));

	char* buffer = new char[header.num_tris * 6 * sizeof(short)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read(buffer, header.num_tris * 6 * sizeof(short));

	for (int i = 0; i < header.num_tris * 6 * sizeof(short); i = i + 6 * sizeof(short)) {
		Utils::UShort value[6];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt , value[1].shrt , value[2].shrt });

		value[3].c[0] = buffer[i + 6]; value[3].c[1] = buffer[i + 7];
		value[4].c[0] = buffer[i + 8]; value[4].c[1] = buffer[i + 9];
		value[5].c[0] = buffer[i + 10]; value[5].c[1] = buffer[i + 11];

		uvFaves.push_back({ value[3].shrt , value[4].shrt , value[5].shrt });
	}
	delete buffer;

	buffer = new char[header.num_st * 2 * sizeof(short)];
	file.seekg(header.ofs_st, std::ios::beg);
	file.read(buffer, header.num_st * 2 * sizeof(short));

	for (int i = 0; i < header.num_st * 2 * sizeof(short); i = i + 2 * sizeof(short)) {
		Utils::UShort s, t;

		s.c[0] = buffer[i + 0]; s.c[1] = buffer[i + 1];
		t.c[0] = buffer[i + 2]; t.c[1] = buffer[i + 3];

		if (flipVertical)
			textureCoords.push_back({ (float)s.shrt / header.skinwidth , 1.0f - (float)t.shrt / header.skinheight });
		else
			textureCoords.push_back({ (float)s.shrt / header.skinwidth , (float)t.shrt / header.skinheight });
	}
	delete buffer;

	int framesize = 40 + header.num_xyz * 4 * sizeof(unsigned char);
	buffer = new char[framesize];

	for (int i = 0; i < header.num_frames; i++) {

		file.seekg(header.ofs_frames + i * framesize, std::ios::beg);
		file.read(buffer, framesize);

		Utils::UFloat _scale[3];

		_scale[0].c[0] = buffer[0]; _scale[0].c[1] = buffer[1]; _scale[0].c[2] = buffer[2]; _scale[0].c[3] = buffer[3];
		_scale[1].c[0] = buffer[4]; _scale[1].c[1] = buffer[5]; _scale[1].c[2] = buffer[6]; _scale[1].c[3] = buffer[7];
		_scale[2].c[0] = buffer[8]; _scale[2].c[1] = buffer[9]; _scale[2].c[2] = buffer[10]; _scale[2].c[3] = buffer[11];

		Utils::UFloat translate[3];

		translate[0].c[0] = buffer[12]; translate[0].c[1] = buffer[13]; translate[0].c[2] = buffer[14]; translate[0].c[3] = buffer[15];
		translate[1].c[0] = buffer[16]; translate[1].c[1] = buffer[17]; translate[1].c[2] = buffer[18]; translate[1].c[3] = buffer[19];
		translate[2].c[0] = buffer[20]; translate[2].c[1] = buffer[21]; translate[2].c[2] = buffer[22]; translate[2].c[3] = buffer[23];

		std::string name;
		name.resize(16);
		std::copy(buffer + 24, buffer + 40, name.begin());

		for (int k = 0; k < header.num_xyz; k++) {
			Utils::UShort vertexS[4];
			vertexS[0].c[0] = buffer[40 + k * 4 + 0]; vertexS[0].c[1] = 0;
			vertexS[1].c[0] = buffer[40 + k * 4 + 1]; vertexS[1].c[1] = 0;
			vertexS[2].c[0] = buffer[40 + k * 4 + 2]; vertexS[2].c[1] = 0;
			vertexS[3].c[0] = buffer[40 + k * 4 + 3]; vertexS[3].c[1] = 0;

			Utils::UFloat vertex[3];
			vertex[0].flt = vertexS[0].shrt;
			vertex[1].flt = vertexS[1].shrt;
			vertex[2].flt = vertexS[2].shrt;

			std::array<float, 3> vert = Utils::ScalePoint(Utils::RotatePoint({ vertex[0].flt * _scale[0].flt + translate[0].flt, vertex[2].flt * _scale[2].flt + translate[2].flt, vertex[1].flt * _scale[1].flt + translate[1].flt }, eulerAngle[0], eulerAngle[1], eulerAngle[2]), scale[0], scale[1], scale[2]);
			positions.push_back({ vert[0], vert[1], vert[2] });

			std::array<float, 3> norm = Utils::RotatePoint({ aNormals[vertexS[3].shrt][0], aNormals[vertexS[3].shrt][1], aNormals[vertexS[3].shrt][2] }, eulerAngle[0], eulerAngle[1], eulerAngle[2]);
			normals.push_back({ norm[0], norm[1], norm[2] });
		}

		indexBuffer.resize(faces.size() * 3);
		for (int i = 0; i < faces.size(); i++) {
			float vertex1[] = { positions[faces[i][0]][0], positions[faces[i][0]][1], positions[faces[i][0]][2],
                                textureCoords[uvFaves[i][0]][0], textureCoords[uvFaves[i][0]][1],
                                normals[faces[i][0]][0], normals[faces[i][0]][1], normals[faces[i][0]][2] };
			indexBuffer[i * 3] = addVertex(faces[i][0], &vertex1[0], 8, vertexBuffer);

			float vertex2[] = { positions[faces[i][1]][0], positions[faces[i][1]][1], positions[faces[i][1]][2],
								textureCoords[uvFaves[i][1]][0], textureCoords[uvFaves[i][1]][1],
								normals[faces[i][1]][0], normals[faces[i][1]][1], normals[faces[i][1]][2] };
			indexBuffer[i * 3 + 1] = addVertex(faces[i][1], &vertex2[0], 8, vertexBuffer);

			float vertex3[] = { positions[faces[i][2]][0], positions[faces[i][2]][1], positions[faces[i][2]][2],
								textureCoords[uvFaves[i][2]][0], textureCoords[uvFaves[i][2]][1],
								normals[faces[i][2]][0], normals[faces[i][2]][1], normals[faces[i][2]][2] };
			indexBuffer[i * 3 + 2] = addVertex(faces[i][2], &vertex3[0], 8, vertexBuffer);
		}
		sequenceOut.addMesh(vertexBuffer, indexBuffer);

		positions.clear();
		positions.shrink_to_fit();
		indexBuffer.clear();
		indexBuffer.shrink_to_fit();
		vertexBuffer.clear();
		vertexBuffer.shrink_to_fit();
		std::map<int, std::vector<int>>().swap(m_vertexCache);
	}
	delete buffer;

	file.close();

	faces.clear();
	faces.shrink_to_fit();
	uvFaves.clear();
	uvFaves.shrink_to_fit();

	textureCoords.clear();
	textureCoords.shrink_to_fit();
	normals.clear();
	normals.shrink_to_fit();
}

void Utils::MD2IO::convert(const std::vector<float>& in, std::vector<Vertex>& out) {
	for (size_t i = 0; i < in.size() / 8; i++) {
		out.push_back({ in[i * 8 + 0], in[i * 8 + 1], in[i * 8 + 2], in[i * 8 + 5], in[i * 8 + 6], in[i * 8 + 7] });
	}
}

void Utils::MD2IO::extractTexels(const std::vector<float>& in, std::vector<float>& out) {
	for (size_t i = 0; i < in.size() / 8; i++) {
		out.push_back(in[i * 8 + 3]);
		out.push_back(in[i * 8 + 4]);
	}
}

void Utils::MD2IO::calcAABB(const std::vector<float>& vertices, BoundingBox& boundingBox) {
	float xmin = FLT_MAX; float ymin = FLT_MAX; float zmin = FLT_MAX;
	float xmax = -FLT_MAX; float ymax = -FLT_MAX; float zmax = -FLT_MAX;
	for (size_t i = 0; i < vertices.size() / 8; i++) {		
		xmin = (std::min)(vertices[i * 8 + 0], xmin);
		ymin = (std::min)(vertices[i * 8 + 1], ymin);
		zmin = (std::min)(vertices[i * 8 + 2], zmin);

		xmax = (std::max)(vertices[i * 8 + 0], xmax);
		ymax = (std::max)(vertices[i * 8 + 1], ymax);
		zmax = (std::max)(vertices[i * 8 + 2], zmax);
	}
	boundingBox.min.set(xmin, ymin, zmin);
	boundingBox.max.set(xmax, ymax, zmax);
}

int Utils::MD2IO::loadMd2(const char* path, bool flipVertical, std::array<float, 3> positionOffset, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<Vertex>& vertexBufferOut, std::vector<float>& texelsOut, std::vector<unsigned int>& indexBufferOut, std::vector<Animation>& animations) {
	std::ifstream file(path, std::ios::binary);
	std::vector<std::array<short, 3>> faces;
	std::vector<std::array<short, 3>> uvFaves;
	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<float, 2>> textureCoords;
	std::vector<std::array<float, 3>> normals;

	std::vector<float> vertexBuffer;

	animations.resize(7);

	MD2Header header;
	file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));

	char* buffer = new char[header.num_tris * 6 * sizeof(short)];
	file.seekg(header.ofs_tris, std::ios::beg);
	file.read(buffer, header.num_tris * 6 * sizeof(short));

	for (int i = 0; i < header.num_tris * 6 * sizeof(short); i = i + 6 * sizeof(short)) {
		Utils::UShort value[6];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt , value[1].shrt , value[2].shrt });

		value[3].c[0] = buffer[i + 6]; value[3].c[1] = buffer[i + 7];
		value[4].c[0] = buffer[i + 8]; value[4].c[1] = buffer[i + 9];
		value[5].c[0] = buffer[i + 10]; value[5].c[1] = buffer[i + 11];

		uvFaves.push_back({ value[3].shrt , value[4].shrt , value[5].shrt });
	}
	delete buffer;

	buffer = new char[header.num_st * 2 * sizeof(short)];
	file.seekg(header.ofs_st, std::ios::beg);
	file.read(buffer, header.num_st * 2 * sizeof(short));

	for (int i = 0; i < header.num_st * 2 * sizeof(short); i = i + 2 * sizeof(short)) {
		Utils::UShort s, t;

		s.c[0] = buffer[i + 0]; s.c[1] = buffer[i + 1];
		t.c[0] = buffer[i + 2]; t.c[1] = buffer[i + 3];

		if (flipVertical)
			textureCoords.push_back({ (float)s.shrt / header.skinwidth , 1.0f - (float)t.shrt / header.skinheight });
		else
			textureCoords.push_back({ (float)s.shrt / header.skinwidth , (float)t.shrt / header.skinheight });
	}
	delete buffer;

	int framesize = 40 + header.num_xyz * 4 * sizeof(unsigned char);
	buffer = new char[framesize];

	for (int i = 0; i < header.num_frames; i++) {

		file.seekg(header.ofs_frames + i * framesize, std::ios::beg);
		file.read(buffer, framesize);

		Utils::UFloat _scale[3];

		_scale[0].c[0] = buffer[0]; _scale[0].c[1] = buffer[1]; _scale[0].c[2] = buffer[2]; _scale[0].c[3] = buffer[3];
		_scale[1].c[0] = buffer[4]; _scale[1].c[1] = buffer[5]; _scale[1].c[2] = buffer[6]; _scale[1].c[3] = buffer[7];
		_scale[2].c[0] = buffer[8]; _scale[2].c[1] = buffer[9]; _scale[2].c[2] = buffer[10]; _scale[2].c[3] = buffer[11];

		Utils::UFloat translate[3];

		translate[0].c[0] = buffer[12]; translate[0].c[1] = buffer[13]; translate[0].c[2] = buffer[14]; translate[0].c[3] = buffer[15];
		translate[1].c[0] = buffer[16]; translate[1].c[1] = buffer[17]; translate[1].c[2] = buffer[18]; translate[1].c[3] = buffer[19];
		translate[2].c[0] = buffer[20]; translate[2].c[1] = buffer[21]; translate[2].c[2] = buffer[22]; translate[2].c[3] = buffer[23];

		std::string name;
		name.resize(16);
		std::copy(buffer + 24, buffer + 40, name.begin());

		for (int k = 0; k < header.num_xyz; k++) {
			Utils::UShort vertexS[4];
			vertexS[0].c[0] = buffer[40 + k * 4 + 0]; vertexS[0].c[1] = 0;
			vertexS[1].c[0] = buffer[40 + k * 4 + 1]; vertexS[1].c[1] = 0;
			vertexS[2].c[0] = buffer[40 + k * 4 + 2]; vertexS[2].c[1] = 0;
			vertexS[3].c[0] = buffer[40 + k * 4 + 3]; vertexS[3].c[1] = 0;

			Utils::UFloat vertex[3];
			vertex[0].flt = vertexS[0].shrt;
			vertex[1].flt = vertexS[1].shrt;
			vertex[2].flt = vertexS[2].shrt;

			std::array<float, 3> vert = Utils::ScalePoint(Utils::RotatePoint({ vertex[0].flt * _scale[0].flt + translate[0].flt, vertex[2].flt * _scale[2].flt + translate[2].flt, vertex[1].flt * _scale[1].flt + translate[1].flt }, eulerAngle[0], eulerAngle[1], eulerAngle[2]), scale[0], scale[1], scale[2]);			
			positions.push_back({ vert[0] + positionOffset[0], vert[1] + positionOffset[1], vert[2] + positionOffset[2] });

			std::array<float, 3> norm = Utils::RotatePoint({ aNormals[vertexS[3].shrt][0], aNormals[vertexS[3].shrt][1], aNormals[vertexS[3].shrt][2]}, eulerAngle[0], eulerAngle[1], eulerAngle[2]);
			normals.push_back({ norm[0], norm[1], norm[2] });
		}

		if (i == 0) {
			indexBufferOut.resize(faces.size() * 3);
			for (int i = 0; i < faces.size(); i++) {
				float vertex1[] = { positions[faces[i][0]][0], positions[faces[i][0]][1], positions[faces[i][0]][2],
									textureCoords[uvFaves[i][0]][0], textureCoords[uvFaves[i][0]][1],
									normals[faces[i][0]][0], normals[faces[i][0]][1], normals[faces[i][0]][2] };
				indexBufferOut[i * 3] = addVertex(faces[i][0], &vertex1[0], 8, vertexBuffer);

				float vertex2[] = { positions[faces[i][1]][0], positions[faces[i][1]][1], positions[faces[i][1]][2],
									textureCoords[uvFaves[i][1]][0], textureCoords[uvFaves[i][1]][1],
									normals[faces[i][1]][0], normals[faces[i][1]][1], normals[faces[i][1]][2] };
				indexBufferOut[i * 3 + 1] = addVertex(faces[i][1], &vertex2[0], 8, vertexBuffer);

				float vertex3[] = { positions[faces[i][2]][0], positions[faces[i][2]][1], positions[faces[i][2]][2],
									textureCoords[uvFaves[i][2]][0], textureCoords[uvFaves[i][2]][1],
				                    normals[faces[i][2]][0], normals[faces[i][2]][1], normals[faces[i][2]][2] };
				indexBufferOut[i * 3 + 2] = addVertex(faces[i][2], &vertex3[0], 8, vertexBuffer);
			}
			convert(vertexBuffer, vertexBufferOut);
			extractTexels(vertexBuffer, texelsOut);
		}else {
			for (int i = 0; i < faces.size(); i++) {
				float vertex1[] = { positions[faces[i][0]][0], positions[faces[i][0]][1], positions[faces[i][0]][2],
									textureCoords[uvFaves[i][0]][0], textureCoords[uvFaves[i][0]][1], 
									normals[faces[i][0]][0], normals[faces[i][0]][1], normals[faces[i][0]][2] };
				addVertex(faces[i][0], &vertex1[0], 8, vertexBuffer);

				float vertex2[] = { positions[faces[i][1]][0], positions[faces[i][1]][1], positions[faces[i][1]][2],
									textureCoords[uvFaves[i][1]][0], textureCoords[uvFaves[i][1]][1],
									normals[faces[i][1]][0], normals[faces[i][1]][1], normals[faces[i][1]][2] };
				addVertex(faces[i][1], &vertex2[0], 8, vertexBuffer);

				float vertex3[] = { positions[faces[i][2]][0], positions[faces[i][2]][1], positions[faces[i][2]][2],
									textureCoords[uvFaves[i][2]][0], textureCoords[uvFaves[i][2]][1],
									normals[faces[i][2]][0], normals[faces[i][2]][1], normals[faces[i][2]][2] };
				addVertex(faces[i][2], &vertex3[0], 8, vertexBuffer);
			}
		}

		if(0 <= i && i <= 39){		
			strcpy(animations[0].name, "stand");
			animations[0].fps = 9.0f;
			animations[0].frames.push_back(Frame());
			convert(vertexBuffer, animations[0].frames.back().vertices);
			calcAABB(vertexBuffer, animations[0].frames.back().boundingBox);
		}

		if (40 <= i && i <= 45) {			
			strcpy(animations[1].name, "run");
			animations[1].fps = 10.0f;
			animations[1].frames.push_back(Frame());
			convert(vertexBuffer, animations[1].frames.back().vertices);
			calcAABB(vertexBuffer, animations[1].frames.back().boundingBox);
		}

		if (46 <= i && i <= 53) {
			strcpy(animations[2].name, "attack");
			animations[2].fps = 10.0f;
			animations[2].frames.push_back(Frame());
			convert(vertexBuffer, animations[2].frames.back().vertices);
			calcAABB(vertexBuffer, animations[2].frames.back().boundingBox);
		}

		if (66 <= i && i <= 71) {
			strcpy(animations[3].name, "jump");
			animations[3].fps = 7.0f;
			animations[3].frames.push_back(Frame());
			convert(vertexBuffer, animations[3].frames.back().vertices);
			calcAABB(vertexBuffer, animations[3].frames.back().boundingBox);
		}
		
		if (178 <= i && i <= 183) {
			strcpy(animations[4].name, "death_back");
			animations[4].fps = 7.0f;
			animations[4].frames.push_back(Frame());
			convert(vertexBuffer, animations[4].frames.back().vertices);
			calcAABB(vertexBuffer, animations[4].frames.back().boundingBox);
		}
		
		if (184 <= i && i <= 189) {
			strcpy(animations[5].name, "death_forward");
			animations[5].fps = 7.0f;
			animations[5].frames.push_back(Frame());
			convert(vertexBuffer, animations[5].frames.back().vertices);
			calcAABB(vertexBuffer, animations[5].frames.back().boundingBox);
		}
		
		if (190 <= i && i <= 197) {
			strcpy(animations[6].name, "death_back_slow");
			animations[6].fps = 7.0f;
			animations[6].frames.push_back(Frame());
			convert(vertexBuffer, animations[6].frames.back().vertices);
			calcAABB(vertexBuffer, animations[6].frames.back().boundingBox);
		}

		positions.clear();
		positions.shrink_to_fit();
		normals.clear();
		normals.shrink_to_fit();
		vertexBuffer.clear();
		vertexBuffer.shrink_to_fit();
		std::map<int, std::vector<int>>().swap(m_vertexCache);
	}
	delete buffer;

	file.close();

	faces.clear();
	faces.shrink_to_fit();
	uvFaves.clear();
	uvFaves.shrink_to_fit();

	textureCoords.clear();
	textureCoords.shrink_to_fit();
	

	return static_cast<int>(vertexBufferOut.size());
}

void Utils::NavIO::writeNavigationMap(const std::string path, int numX, int numZ, const BoundingBox& boundingBox, const std::unordered_map<int, Buffer>& data) {
	std::ofstream stream(path, std::ios::binary);
	stream.write(reinterpret_cast<const char*>(&numX), sizeof(int));
	stream.write(reinterpret_cast<const char*>(&numZ), sizeof(int));
	stream.write(reinterpret_cast<const char*>(&boundingBox), sizeof(BoundingBox));

	for (int z = 0; z < numZ; ++z) {
		for (int x = 0; x < numX; ++x) {
			const Buffer& buffer = data.at(z * numX + x);
			stream.write(reinterpret_cast<const char*>(&buffer.size), sizeof(size_t));
			stream.write(buffer.data, buffer.size);
		}
	}
	stream.close();
}

void Utils::NavIO::readNavigationMap(const std::string path, int& numX, int& numZ, BoundingBox& boundingBox, std::unordered_map<int, Buffer>& data) {
	std::ifstream stream(path, std::ios::binary);
	stream.read(reinterpret_cast<char*>(&numX), sizeof(int));
	stream.read(reinterpret_cast<char*>(&numZ), sizeof(int));
	stream.read(reinterpret_cast<char*>(&boundingBox), sizeof(BoundingBox));
	size_t size;
	for (int z = 0; z < numZ; ++z) {
		for (int x = 0; x < numX; ++x) {			
			Buffer& bufer = data[z * numX + x];		
			stream.read(reinterpret_cast<char*>(&size), sizeof(size_t));
			bufer.resize(size);
			stream.read(bufer.data, bufer.size);
		}
	}
	stream.close();
}
