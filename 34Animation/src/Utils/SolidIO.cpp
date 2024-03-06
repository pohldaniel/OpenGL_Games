#include "SolidIO.h"

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

bool Utils::SolidIO::getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, ComparerUv>& uvToOutIndex, short & result) {
	std::map<std::array<float, 2>, short>::iterator it = uvToOutIndex.find(packed);
	if (it == uvToOutIndex.end()) {
		return false;
	}else {
		result = it->second;
		return true;
	}
}

bool Utils::SolidIO::getSimilarVertexIndex(std::array<float, 5>& packed, std::map<std::array<float, 5>, short, Comparer>& uvToOutIndex, short & result) {
	std::map<std::array<float, 5>, short>::iterator it = uvToOutIndex.find(packed);
	if (it == uvToOutIndex.end()) {
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

	std::cout << "Vertex Count: " << vertexCount <<  std::endl;
	std::cout << "Triangle Count: " << triangleCount << std::endl;

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
	fileOut << "mtllib " << mltPath.filename() << std::endl;

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

std::array<float, 3> Utils::SolidIO::RotatePoint(std::array<float, 3> thePoint, float xang, float yang, float zang)
{
	static std::array<float, 3> newpoint;
	if (xang) {
		xang *= 6.283185f;
		xang /= 360;
	}
	if (yang) {
		yang *= 6.283185f;
		yang /= 360;
	}
	if (zang) {
		zang *= 6.283185f;
		zang /= 360;
	}

	if (yang) {
		newpoint[2] = thePoint[2] * cosf(yang) - thePoint[0] * sinf(yang);
		newpoint[0] = thePoint[2] * sinf(yang) + thePoint[0] * cosf(yang);
		thePoint[2] = newpoint[2];
		thePoint[0] = newpoint[0];
	}

	if (zang) {
		newpoint[0] = thePoint[0] * cosf(zang) - thePoint[1] * sinf(zang);
		newpoint[1] = thePoint[1] * cosf(zang) + thePoint[0] * sinf(zang);
		thePoint[0] = newpoint[0];
		thePoint[1] = newpoint[1];
	}

	if (xang) {
		newpoint[1] = thePoint[1] * cosf(xang) - thePoint[2] * sinf(xang);
		newpoint[2] = thePoint[1] * sinf(xang) + thePoint[2] * cosf(xang);
		thePoint[2] = newpoint[2];
		thePoint[1] = newpoint[1];
	}

	return thePoint;
}

std::array<float, 3> Utils::SolidIO::ScalePoint(std::array<float, 3> point, float scaleX, float scaleY, float scaleZ) {
	return { point[0] * scaleX, point[1] * scaleY , point[2] * scaleZ };
}

void Utils::SolidIO::solidToBuffer(const char* filename, bool flipTextureVertical, std::array<float, 3> eulerAngle, std::array<float, 3> scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	std::vector<std::array<float, 3>> vertices;
	std::map<std::array<float, 5>, short, Comparer> vertexCache;

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
		value[0].c[0] = (unsigned int)(buffer[i +  3]); value[0].c[1] = (unsigned int)(buffer[i +  2]); value[0].c[2] = (unsigned int)(buffer[i + 1]); value[0].c[3] = (unsigned int)(buffer[i + 0]);
		value[1].c[0] = (unsigned int)(buffer[i +  7]); value[1].c[1] = (unsigned int)(buffer[i +  6]); value[1].c[2] = (unsigned int)(buffer[i + 5]); value[1].c[3] = (unsigned int)(buffer[i + 4]);
		value[2].c[0] = (unsigned int)(buffer[i + 11]); value[2].c[1] = (unsigned int)(buffer[i + 10]); value[2].c[2] = (unsigned int)(buffer[i + 9]); value[2].c[3] = (unsigned int)(buffer[i + 8]);
		vertices.push_back({ value[0].flt, value[1].flt, value[2].flt });
		
		//std::cout << value[0].flt << "  " << value[1].flt << "  " << value[2].flt << std::endl;

		//vertices.push_back(ScalePoint(RotatePoint({ value[0].flt, value[1].flt, value[2].flt }, eulerAngle[0], eulerAngle[1], eulerAngle[2]), scale[0], scale[1], scale[2]));
	}
	
	char* bufferTris = new char[triangleCount * (6 * (sizeof(short) + sizeof(float)))];

	file.read(bufferTris, triangleCount * (6 * (sizeof(short) + sizeof(float))));
	file.close();

	std::array<short, 3> ccw = {0, 2, 1};
	std::array<short, 3> cw =  {0, 1, 2};

	for (int i = 0; i < triangleCount * (6 * (sizeof(short) + sizeof(float))); i = i + 36) {
	
		short index;
		bool found;
		std::array<float, 5> vert;

		for (const short &k : cw){
			vert = { vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])][0],
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])][1],
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])][2],
					 bytesToFloatBE(bufferTris[i + (k + 3) * 4], bufferTris[i + (k + 3) * 4 + 1], bufferTris[i + (k + 3) * 4 + 2], bufferTris[i + (k + 3) * 4 + 3]),
					 flipTextureVertical ? 1.0f - bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3]) : bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3])
					};

			found = getSimilarVertexIndex(vert, vertexCache, index);
			if (found) {
				indexBufferOut.push_back(index);
			}else {
				unsigned int newindex = (unsigned int)(vertexBufferOut.size() / 5);
				vertexBufferOut.insert(vertexBufferOut.end(), vert.begin(), vert.end());			
				indexBufferOut.push_back(newindex);
				vertexCache[vert] = newindex;
			}
		}
	}

	//for (int i = 0; i < triangleCount; i++) {
	//	std::cout << vertexBufferOut[indexBufferOut[i] * 5] << "  " << vertexBufferOut[indexBufferOut[i] * 5 + 1] << "  " << vertexBufferOut[indexBufferOut[i] * 5 + 2] << std::endl;
	//}
	//std::cout << "################################" << std::endl;
	delete buffer;
	delete bufferTris;
}

void Utils::SolidIO::loadSkeleton(const char* filename, std::vector<float>& modelVertexBuffer) {
	Skeleton skeleton;
	
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

	//skeleton.print();

	for (auto& muscle : skeleton.m_muscles) {
		muscle.initialPosition = (skeleton.m_joints[muscle.parentIndex1].position + skeleton.m_joints[muscle.parentIndex2].position) * 0.5f;
		for (int vertIndex : muscle.vertexIndices) {
			modelVertexBuffer[vertIndex * 5 + 0] -= muscle.initialPosition[0];
			modelVertexBuffer[vertIndex * 5 + 1] -= muscle.initialPosition[1];
			modelVertexBuffer[vertIndex * 5 + 2] -= muscle.initialPosition[2];
		}
	}
}

void Utils::MdlIO::mdlToObj(const char* path, const char* outFileObj, const char* outFileMtl, const char* texturePath) {
	std::filesystem::path mltPath(outFileMtl);
	
	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<float, 3>> normals;
	std::vector <std::array<float, 2>> uvCoords;

	std::vector<std::array<short, 3>> faces;

	std::ifstream file(path, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));
	//std::cout << "Ret: " << ret << std::endl;

	char metaData[4];

	file.read(metaData, sizeof(unsigned int));
	//std::cout << "Buffer Count: " << Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]) << std::endl;


	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	//std::cout << "Vertex Count: " << vertexCount << std::endl;

	file.read(metaData, sizeof(unsigned int));
	unsigned int numElements = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	//std::cout << "Num Elements: " << numElements << std::endl;

	unsigned int vertexSize = 0;

	for (unsigned j = 0; j < numElements; ++j) {
		file.read(metaData, sizeof(unsigned int));
		unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
		vertexSize += ELEMENT_TYPESIZES[elementDesc & 0xff];	
	}
	//std::cout << "Vertex Size2: " << vertexSize << std::endl;

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

		value[0].c[0] = buffer[i + 12]; value[0].c[1] = buffer[i + 13]; value[0].c[2] = buffer[i + 14]; value[0].c[3] = buffer[i + 15];
		value[1].c[0] = buffer[i + 16]; value[1].c[1] = buffer[i + 17]; value[1].c[2] = buffer[i + 18]; value[1].c[3] = buffer[i + 19];
		value[2].c[0] = buffer[i + 20]; value[2].c[1] = buffer[i + 21]; value[2].c[2] = buffer[i + 22]; value[2].c[3] = buffer[i + 23];
		normals.push_back({ value[0].flt , value[1].flt , value[2].flt });

		value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
		value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
		uvCoords.push_back({ value[0].flt , value[1].flt });
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	//std::cout << "Num IndexBuffer: " << Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]) << std::endl;

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	//std::cout << "Index Count: " << indexCount << std::endl;

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexSize = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	//std::cout << "Index Size: " << indexSize << std::endl;

	buffer = new char[indexCount * indexSize];
	file.read(buffer, indexCount * indexSize);

	for (unsigned int i = 0; i < indexCount * indexSize; i = i + indexSize * 3) {
		UShort value[3];

		value[0].c[0] = buffer[i + 0]; value[0].c[1] = buffer[i + 1];
		value[1].c[0] = buffer[i + 2]; value[1].c[1] = buffer[i + 3];
		value[2].c[0] = buffer[i + 4]; value[2].c[1] = buffer[i + 5];

		faces.push_back({ value[0].shrt, value[1].shrt, value[2].shrt });
	}
	delete buffer;
	file.close();

	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";
	fileOut << "mtllib " << mltPath.filename() << std::endl;

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
	for (int i = 0; i < faces.size(); i++) {
		fileOut << "f " << faces[i][0] + 1 << "/" << faces[i][0] + 1 << "/" << faces[i][0] + 1 << " " << faces[i][1] + 1 << "/" << faces[i][1] + 1 << "/" << faces[i][1] + 1 << " " << faces[i][2] + 1 << "/" << faces[i][2] + 1 << "/" << faces[i][2] + 1 << std::endl;
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

void Utils::MdlIO::mdlToBuffer(const char* path, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float, 4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<std::vector<GeometryDesc>>& geomDescs, std::vector<MeshBone>& bones, BoundingBox& boundingBox) {
	mdlToBuffer(path, { scale, scale, scale }, vertexBufferOut, indexBufferOut, weightsOut, boneIdsOut, geomDescs, bones, boundingBox);
}

void Utils::MdlIO::mdlToBuffer(const char* path, std::array<float,3> _scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut, std::vector<std::array<float, 4>>& weightsOut, std::vector<std::array<unsigned int, 4>>& boneIdsOut, std::vector<std::vector<GeometryDesc>>& geomDescs, std::vector<MeshBone>& bones, BoundingBox& boundingBox) {
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

	for (unsigned j = 0; j < numElements; ++j) {
		file.read(metaData, sizeof(unsigned int));
		unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
		VertexElementSemantic semantic = (VertexElementSemantic)((elementDesc >> 8) & 0xff);
		vertexSize += Utils::ELEMENT_TYPESIZES[elementDesc & 0xff];
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

		value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
		value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
		vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

		//value[0].c[0] = buffer[i + 32]; value[0].c[1] = buffer[i + 33]; value[0].c[2] = buffer[i + 34]; value[0].c[3] = buffer[i + 35];
		//value[1].c[0] = buffer[i + 36]; value[1].c[1] = buffer[i + 37]; value[1].c[2] = buffer[i + 38]; value[1].c[3] = buffer[i + 39];
		//vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

		//value[0].c[0] = buffer[i + 40]; value[0].c[1] = buffer[i + 41]; value[0].c[2] = buffer[i + 42]; value[0].c[3] = buffer[i + 43];
		//value[1].c[0] = buffer[i + 44]; value[1].c[1] = buffer[i + 45]; value[1].c[2] = buffer[i + 46]; value[1].c[3] = buffer[i + 47];
		//vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

		//value[0].c[0] = buffer[i + 48]; value[0].c[1] = buffer[i + 49]; value[0].c[2] = buffer[i + 50]; value[0].c[3] = buffer[i + 51];
		//value[1].c[0] = buffer[i + 52]; value[1].c[1] = buffer[i + 53]; value[1].c[2] = buffer[i + 54]; value[1].c[3] = buffer[i + 55];
		//vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

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
		indexBufferOut.push_back(value[0].shrt); indexBufferOut.push_back(value[1].shrt); indexBufferOut.push_back(value[2].shrt);
	}
	delete buffer;

	file.read(metaData, sizeof(unsigned int));
	unsigned int numGeometries = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	geomDescs.resize(numGeometries);

	for (size_t i = 0; i < numGeometries; ++i) {
		file.read(metaData, sizeof(unsigned int));
		unsigned int boneMappingCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		file.read(metaData, sizeof(unsigned int));
		unsigned int numLodLevels = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		geomDescs[i].resize(numLodLevels);

		for (size_t j = 0; j < numLodLevels; ++j) {

			GeometryDesc& geomDesc = geomDescs[i][j];

		

			file.read(metaData, sizeof(float));
			geomDesc.lodDistance = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			// Primitive type
			file.read(metaData, sizeof(unsigned int));

			file.read(metaData, sizeof(unsigned int));
			geomDesc.vbRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			geomDesc.ibRef = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			geomDesc.drawStart = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			file.read(metaData, sizeof(unsigned int));
			geomDesc.drawCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
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