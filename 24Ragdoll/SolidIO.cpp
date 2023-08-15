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

	std::experimental::filesystem::path mltPath(outFileMtl);

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
	std::string absPath = std::experimental::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '\\', '/');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();
}

void Utils::SolidIO::solidToBuffer(const char* filename, bool flipVertical, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
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
		vertices.push_back({ value[0].flt , value[1].flt , value[2].flt });
	}

	char* bufferTris = new char[triangleCount * (6 * (sizeof(short) + sizeof(float)))];

	file.read(bufferTris, triangleCount * (6 * (sizeof(short) + sizeof(float))));
	file.close();

	std::array<short, 3> ccw = {0, 2, 1};

	for (int i = 0; i < triangleCount * (6 * (sizeof(short) + sizeof(float))); i = i + 36) {
	
		short index;
		bool found;
		std::array<float, 5> vert;

		for (const short &k : ccw){
			vert = { vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])][0],
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])][1],
					 vertices[bytesToShortBE(bufferTris[i + k * 4], bufferTris[i + k * 4 + 1])][2],
					 bytesToFloatBE(bufferTris[i + (k + 3) * 4], bufferTris[i + (k + 3) * 4 + 1], bufferTris[i + (k + 3) * 4 + 2], bufferTris[i + (k + 3) * 4 + 3]),
					 flipVertical ? 1.0f - bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3]) : bytesToFloatBE(bufferTris[i + (k + 6) * 4], bufferTris[i + (k + 6) * 4 + 1], bufferTris[i + (k + 6) * 4 + 2], bufferTris[i + (k + 6) * 4 + 3])
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

	delete buffer;
	delete bufferTris;
}

const unsigned ELEMENT_TYPESIZES[] = {
	sizeof(int),
	sizeof(float),
	2 * sizeof(float),
	3 * sizeof(float),
	4 * sizeof(float),
	sizeof(unsigned),
	sizeof(unsigned)
};

void Utils::MdlIO::mdlToObj(const char* filename, const char* outFileObj, const char* outFileMtl, const char* texturePath) {
	std::experimental::filesystem::path mltPath(outFileMtl);
	
	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<float, 3>> normals;
	std::vector <std::array<float, 2>> uvCoords;

	std::vector<std::array<short, 3>> faces;

	std::ifstream file(filename, std::ios::binary);

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
	std::string absPath = std::experimental::filesystem::current_path().generic_string();
	std::replace(absPath.begin(), absPath.end(), '\\', '/');
	fileOut << "map_Kd " << absPath << texturePath;
	fileOut.close();
}

void Utils::MdlIO::mdlToBuffer(const char* filename, float scale, std::vector<float>& vertexBufferOut, std::vector<unsigned int>& indexBufferOut) {
	std::ifstream file(filename, std::ios::binary);

	std::string ret;
	ret.resize(4);
	file.read(&ret[0], 4 * sizeof(char));

	if (ret.compare("UMD2") == 0) {
		char metaData[4];

		// num Vertexbuffer
		file.read(metaData, sizeof(unsigned int));

		file.read(metaData, sizeof(unsigned int));
		unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		file.read(metaData, sizeof(unsigned int));
		unsigned int numElements = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		unsigned int vertexSize = 0;

		for (unsigned j = 0; j < numElements; ++j) {
			file.read(metaData, sizeof(unsigned int));
			unsigned int elementDesc = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
			vertexSize += ELEMENT_TYPESIZES[elementDesc & 0xff];
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
			vertexBufferOut.push_back(value[0].flt * scale); vertexBufferOut.push_back(value[1].flt * scale); vertexBufferOut.push_back(value[2].flt * scale);

			value[0].c[0] = buffer[i + 24]; value[0].c[1] = buffer[i + 25]; value[0].c[2] = buffer[i + 26]; value[0].c[3] = buffer[i + 27];
			value[1].c[0] = buffer[i + 28]; value[1].c[1] = buffer[i + 29]; value[1].c[2] = buffer[i + 30]; value[1].c[3] = buffer[i + 31];
			vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt);

			value[0].c[0] = buffer[i + 12]; value[0].c[1] = buffer[i + 13]; value[0].c[2] = buffer[i + 14]; value[0].c[3] = buffer[i + 15];
			value[1].c[0] = buffer[i + 16]; value[1].c[1] = buffer[i + 17]; value[1].c[2] = buffer[i + 18]; value[1].c[3] = buffer[i + 19];
			value[2].c[0] = buffer[i + 20]; value[2].c[1] = buffer[i + 21]; value[2].c[2] = buffer[i + 22]; value[2].c[3] = buffer[i + 23];
			vertexBufferOut.push_back(value[0].flt); vertexBufferOut.push_back(value[1].flt); vertexBufferOut.push_back(value[2].flt);

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
			indexBufferOut.push_back(value[0].shrt); indexBufferOut.push_back(value[1].shrt); indexBufferOut.push_back(value[2].shrt);
		}
		delete buffer;
		
	}else if (ret.compare("UMDL") == 0) {
		char metaData[4];

		// num Vertexbuffer
		file.read(metaData, sizeof(unsigned int));

		file.read(metaData, sizeof(unsigned int));
		unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		file.read(metaData, sizeof(unsigned int));
		unsigned elementMask = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		unsigned int vertexSize = 0;

		if (elementMask & 1){
			vertexSize += sizeof(float) * 3;
		}
		if (elementMask & 2)
		{
			vertexSize += sizeof(float) * 3;
		}
		if (elementMask & 4)
		{
			vertexSize += 4;
		}
		if (elementMask & 8)
		{		
			vertexSize += sizeof(float) * 2;
		}
		if (elementMask & 16)
		{
			vertexSize += sizeof(float) * 2;
		}
		if (elementMask & 32)
		{
			vertexSize += sizeof(float) * 3;
		}
		if (elementMask & 64)
		{
			vertexSize += sizeof(float) * 3;		
		}
		if (elementMask & 128)
		{
			vertexSize += sizeof(float) * 4;
		}
		if (elementMask & 256)
		{
			vertexSize += sizeof(float) * 4;
		}
		if (elementMask & 512)
		{
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
			vertexBufferOut.push_back(value[0].flt * scale); vertexBufferOut.push_back(value[1].flt * scale); vertexBufferOut.push_back(value[2].flt * scale);

			if (elementMask & 4) {
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
			indexBufferOut.push_back(value[0].shrt); indexBufferOut.push_back(value[1].shrt); indexBufferOut.push_back(value[2].shrt);
		}
		delete buffer;
	}
	file.close();
}