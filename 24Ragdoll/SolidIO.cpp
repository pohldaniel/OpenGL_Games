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

bool Utils::SolidIO::getSimilarVertexIndex(std::array<float, 2>& packed, std::map<std::array<float, 2>, short, Comparer>& uvToOutIndex, short & result) {
	std::map<std::array<float, 2>, short>::iterator it = uvToOutIndex.find(packed);
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
	std::map<std::array<float, 2>, short, Comparer> vertexCache;

	std::ifstream file(filename, std::ios::binary);
	char metaData[4];
	file.read(&metaData[0], 1);
	file.read(&metaData[1], 1);
	file.read(&metaData[2], 1);
	file.read(&metaData[3], 1);

	short vertexCount = bytesToShortBE(metaData[0], metaData[1]);
	short triangleCount = bytesToShortBE(metaData[2], metaData[3]);

	//std::cout << "Vertex Count: " << vertexCount <<  std::endl;
	//std::cout << "Triangle Count: " << triangleCount << std::endl;

	char* buffer = new char[vertexCount * sizeof(float) * 3];
	file.read(buffer, vertexCount * sizeof(float) * 3);

	for (int i = 0; i < vertexCount * 3 * sizeof(float); i = i + 12) {
		UFloat value[3];
		value[0].c[0] = (unsigned int)(buffer[i + 3]); value[0].c[1] = (unsigned int)(buffer[i + 2]); value[0].c[2] = (unsigned int)(buffer[i + 1]); value[0].c[3] = (unsigned int)(buffer[i + 0]);
		value[1].c[0] = (unsigned int)(buffer[i + 7]); value[1].c[1] = (unsigned int)(buffer[i + 6]); value[1].c[2] = (unsigned int)(buffer[i + 5]); value[1].c[3] = (unsigned int)(buffer[i + 4]);
		value[2].c[0] = (unsigned int)(buffer[i + 11]); value[2].c[1] = (unsigned int)(buffer[i + 10]); value[2].c[2] = (unsigned int)(buffer[i + 9]); value[2].c[3] = (unsigned int)(buffer[i + 8]);
		vertices.push_back({ value[0].flt , value[1].flt , value[2].flt });
		//vertices.push_back({ bytesToFloatBE(buffer[i + 0], buffer[i + 1], buffer[i + 2], buffer[i + 3]) , bytesToFloatBE(buffer[i + 4], buffer[i + 5], buffer[i + 6], buffer[i + 7]) , bytesToFloatBE(buffer[i + 8], buffer[i + 9], buffer[i + 10], buffer[i + 11]) });
	}

	char* bufferTris = new char[triangleCount * (6 * (sizeof(short) + sizeof(float)))];

	file.read(bufferTris, triangleCount * (6 * (sizeof(short) + sizeof(float))));
	file.close();

	for (int i = 0; i < triangleCount * (6 * (sizeof(short) + sizeof(float))); i = i + 36) {
		faces.push_back({ bytesToShortBE(bufferTris[i] ,bufferTris[i + 1]) + 1 , bytesToShortBE(bufferTris[i + 4], bufferTris[i + 5]) + 1 ,  bytesToShortBE(bufferTris[i + 8], bufferTris[i + 9]) + 1 });

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
				uvFaves.push_back(index + 1);
			}else {
				uvCoords.push_back(uv);
				short newindex = (short)uvCoords.size() - 1;
				uvFaves.push_back(newindex + 1);
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
		fileOut << "f " << faces[i][0] << "/" << uvFaves[i * 3] << " " << faces[i][1] << "/" << uvFaves[i * 3 + 1] << " " << faces[i][2] << "/" << uvFaves[i * 3 + 2] << std::endl;
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