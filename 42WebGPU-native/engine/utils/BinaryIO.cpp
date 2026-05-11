#include <iostream>
#include "BinaryIO.h"

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

short Utils::bytesToShortLE(unsigned char b0, unsigned char b1) {
	short f;
	unsigned char b[] = { b0, b1 };
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
			//VertexElementSemantic semantic = (VertexElementSemantic)((elementDesc >> 8) & 0xff);
			//vertexSize += Utils::ELEMENT_TYPESIZES[elementDesc & 0xff];
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

	short maxIndex = 0, minIndex = positions.size();
	for (const std::pair<std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int>>& geometrie : geomteries) {
		if (geometrie.first.second == 0u) {
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

void Utils::MdlcIO::animatedModelToMdlc(const char* out, const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, uint32_t stride, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& joints, const std::vector<BoneDescription>& boneDescriptions) {
	if (stride != 8u)
		return;
	
	std::ofstream file(out, std::ios::binary);
	
	unsigned int vertexCount = static_cast<unsigned int>(vertexBuffer.size() / stride);
	file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(unsigned int));

	unsigned int vertexSize = static_cast<unsigned int>(stride);
	file.write(reinterpret_cast<const char*>(&vertexSize), sizeof(unsigned int));

	char* buffer = new char[16];
	for (unsigned int i = 0; i < vertexCount; ++i) {
		std::array<float, 3> position = { vertexBuffer[i * vertexSize], vertexBuffer[i * vertexSize + 1], vertexBuffer[i * vertexSize + 2] };
		std::array<float, 2> uv =       { vertexBuffer[i * vertexSize + 3], vertexBuffer[i * vertexSize + 4] };
		std::array<float, 3> normal =   { vertexBuffer[i * vertexSize + 5], vertexBuffer[i * vertexSize + 6], vertexBuffer[i * vertexSize + 7] };

		Utils::UFloat ret[4];

		ret[0].flt = position[0]; ret[1].flt = position[1]; ret[2].flt = position[2];
		buffer[0] = ret[0].c[0]; buffer[1] = ret[0].c[1]; buffer[2]  = ret[0].c[2]; buffer[3]  = ret[0].c[3];
		buffer[4] = ret[1].c[0]; buffer[5] = ret[1].c[1]; buffer[6]  = ret[1].c[2]; buffer[7]  = ret[1].c[3];
		buffer[8] = ret[2].c[0]; buffer[9] = ret[2].c[1]; buffer[10] = ret[2].c[2]; buffer[11] = ret[2].c[3];
		file.write(buffer, sizeof(float) * 3);

		ret[0].flt = uv[0]; ret[1].flt = uv[1];
		buffer[0] = ret[0].c[0]; buffer[1] = ret[0].c[1]; buffer[2] = ret[0].c[2]; buffer[3] = ret[0].c[3];
		buffer[4] = ret[1].c[0]; buffer[5] = ret[1].c[1]; buffer[6] = ret[1].c[2]; buffer[7] = ret[1].c[3];
		file.write(buffer, sizeof(float) * 2);

		ret[0].flt = normal[0]; ret[1].flt = normal[1]; ret[2].flt = normal[2];
		buffer[0] = ret[0].c[0]; buffer[1] = ret[0].c[1]; buffer[2] = ret[0].c[2]; buffer[3] = ret[0].c[3];
		buffer[4] = ret[1].c[0]; buffer[5] = ret[1].c[1]; buffer[6] = ret[1].c[2]; buffer[7] = ret[1].c[3];
		buffer[8] = ret[2].c[0]; buffer[9] = ret[2].c[1]; buffer[10] = ret[2].c[2]; buffer[11] = ret[2].c[3];
		file.write(buffer, sizeof(float) * 3);

		std::array<float, 4> weight = { weights[i][0], weights[i][1],  weights[i][2], weights[i][3] };

		ret[0].flt = weight[0]; ret[1].flt = weight[1]; ret[2].flt = weight[2]; ret[3].flt = weight[3];
		buffer[0]  = ret[0].c[0]; buffer[1]  = ret[0].c[1]; buffer[2]  = ret[0].c[2]; buffer[3]  = ret[0].c[3];
		buffer[4]  = ret[1].c[0]; buffer[5]  = ret[1].c[1]; buffer[6]  = ret[1].c[2]; buffer[7]  = ret[1].c[3];
		buffer[8]  = ret[2].c[0]; buffer[9]  = ret[2].c[1]; buffer[10] = ret[2].c[2]; buffer[11] = ret[2].c[3];
		buffer[12] = ret[3].c[0]; buffer[13] = ret[3].c[1]; buffer[14] = ret[3].c[2]; buffer[15] = ret[3].c[3];
		file.write(buffer, sizeof(float) * 4);

		std::array<char, 4> joint = { joints[i][0], joints[i][1],  joints[i][2], joints[i][3] };

		buffer[0] = joint[0]; buffer[1] = joint[1]; buffer[2] = joint[2]; buffer[3] = joint[3];
		file.write(buffer, sizeof(char) * 4);
	}
	delete buffer;

	unsigned int indexCount = static_cast<unsigned int>(indexBuffer.size());
	file.write(reinterpret_cast<const char*>(&indexCount), sizeof(unsigned int));
	file.write(reinterpret_cast<const char*>(indexBuffer.data()), indexCount * sizeof(unsigned int));

	unsigned int numBones = static_cast<unsigned int>(boneDescriptions.size());
	file.write(reinterpret_cast<const char*>(&numBones), sizeof(unsigned int));

	for (size_t i = 0; i < boneDescriptions.size(); ++i) {
		const BoneDescription& boneDescription = boneDescriptions[i];
		file.write(&boneDescription.name[0], boneDescription.name.size() + 1);

		unsigned int parentIndex = boneDescription.parentIndex;
		file.write(reinterpret_cast<const char*>(&parentIndex), sizeof(unsigned int));

		char* bufferBoneTrans = new char[48];

		Utils::UFloat position[3]; Utils::UFloat orientation[4]; Utils::UFloat scale[3]; Utils::UFloat offset[12];

		position[0].flt = boneDescription.initialPosition[0]; position[1].flt = boneDescription.initialPosition[1]; position[2].flt = boneDescription.initialPosition[2];
		bufferBoneTrans[0] = position[0].c[0]; bufferBoneTrans[1] = position[0].c[1]; bufferBoneTrans[2]  = position[0].c[2]; bufferBoneTrans[3]  = position[0].c[3];
		bufferBoneTrans[4] = position[1].c[0]; bufferBoneTrans[5] = position[1].c[1]; bufferBoneTrans[6]  = position[1].c[2]; bufferBoneTrans[7]  = position[1].c[3];
		bufferBoneTrans[8] = position[2].c[0]; bufferBoneTrans[9] = position[2].c[1]; bufferBoneTrans[10] = position[2].c[2]; bufferBoneTrans[11] = position[2].c[3];
		file.write(bufferBoneTrans, sizeof(float) * 3);

		orientation[0].flt = boneDescription.initialRotation[0]; orientation[1].flt = boneDescription.initialRotation[1]; orientation[2].flt = boneDescription.initialRotation[2]; orientation[3].flt = boneDescription.initialRotation[3];
		bufferBoneTrans[0]  = orientation[3].c[0]; bufferBoneTrans[1]  = orientation[3].c[1]; bufferBoneTrans[2]  = orientation[3].c[2]; bufferBoneTrans[3]  = orientation[3].c[3];
		bufferBoneTrans[4]  = orientation[0].c[0]; bufferBoneTrans[5]  = orientation[0].c[1]; bufferBoneTrans[6]  = orientation[0].c[2]; bufferBoneTrans[7]  = orientation[0].c[3];
		bufferBoneTrans[8]  = orientation[1].c[0]; bufferBoneTrans[9]  = orientation[1].c[1]; bufferBoneTrans[10] = orientation[1].c[2]; bufferBoneTrans[11] = orientation[1].c[3];
		bufferBoneTrans[12] = orientation[2].c[0]; bufferBoneTrans[13] = orientation[2].c[1]; bufferBoneTrans[14] = orientation[2].c[2]; bufferBoneTrans[15] = orientation[2].c[3];

		file.write(bufferBoneTrans, sizeof(float) * 4);

		scale[0].flt = boneDescription.initialScale[0]; scale[1].flt = boneDescription.initialScale[1]; scale[2].flt = boneDescription.initialScale[2];
		bufferBoneTrans[0] = scale[0].c[0]; bufferBoneTrans[1] = scale[0].c[1]; bufferBoneTrans[2]  = scale[0].c[2]; bufferBoneTrans[3]  = scale[0].c[3];
		bufferBoneTrans[4] = scale[1].c[0]; bufferBoneTrans[5] = scale[1].c[1]; bufferBoneTrans[6]  = scale[1].c[2]; bufferBoneTrans[7]  = scale[1].c[3];
		bufferBoneTrans[8] = scale[2].c[0]; bufferBoneTrans[9] = scale[2].c[1]; bufferBoneTrans[10] = scale[2].c[2]; bufferBoneTrans[11] = scale[2].c[3];
		file.write(bufferBoneTrans, sizeof(float) * 3);

		offset[0].flt = boneDescription.offsetMatrix[0][0]; offset[4].flt = boneDescription.offsetMatrix[0][1]; offset[8].flt  = boneDescription.offsetMatrix[0][2];
		offset[1].flt = boneDescription.offsetMatrix[1][0]; offset[5].flt = boneDescription.offsetMatrix[1][1]; offset[9].flt  = boneDescription.offsetMatrix[1][2];
		offset[2].flt = boneDescription.offsetMatrix[2][0]; offset[6].flt = boneDescription.offsetMatrix[2][1]; offset[10].flt  = boneDescription.offsetMatrix[2][2];
		offset[3].flt = boneDescription.offsetMatrix[3][0]; offset[7].flt = boneDescription.offsetMatrix[3][1]; offset[11].flt = boneDescription.offsetMatrix[3][2];

		bufferBoneTrans[0] = offset[0].c[0]; bufferBoneTrans[1] = offset[0].c[1]; bufferBoneTrans[2]  = offset[0].c[2]; bufferBoneTrans[3]  = offset[0].c[3];
		bufferBoneTrans[4] = offset[1].c[0]; bufferBoneTrans[5] = offset[1].c[1]; bufferBoneTrans[6]  = offset[1].c[2]; bufferBoneTrans[7]  = offset[1].c[3];
		bufferBoneTrans[8] = offset[2].c[0]; bufferBoneTrans[9] = offset[2].c[1]; bufferBoneTrans[10] = offset[2].c[2]; bufferBoneTrans[11] = offset[2].c[3];

		bufferBoneTrans[12] = offset[3].c[0]; bufferBoneTrans[13] = offset[3].c[1]; bufferBoneTrans[14] = offset[3].c[2]; bufferBoneTrans[15] = offset[3].c[3];
		bufferBoneTrans[16] = offset[4].c[0]; bufferBoneTrans[17] = offset[4].c[1]; bufferBoneTrans[18] = offset[4].c[2]; bufferBoneTrans[19] = offset[4].c[3];
		bufferBoneTrans[20] = offset[5].c[0]; bufferBoneTrans[21] = offset[5].c[1]; bufferBoneTrans[22] = offset[5].c[2]; bufferBoneTrans[23] = offset[5].c[3];

		bufferBoneTrans[24] = offset[6].c[0]; bufferBoneTrans[25] = offset[6].c[1]; bufferBoneTrans[26] = offset[6].c[2]; bufferBoneTrans[27] = offset[6].c[3];
		bufferBoneTrans[28] = offset[7].c[0]; bufferBoneTrans[29] = offset[7].c[1]; bufferBoneTrans[30] = offset[7].c[2]; bufferBoneTrans[31] = offset[7].c[3];
		bufferBoneTrans[32] = offset[8].c[0]; bufferBoneTrans[33] = offset[8].c[1]; bufferBoneTrans[34] = offset[8].c[2]; bufferBoneTrans[35] = offset[8].c[3];

		bufferBoneTrans[36] = offset[9].c[0];  bufferBoneTrans[37] = offset[9].c[1];  bufferBoneTrans[38] = offset[9].c[2];  bufferBoneTrans[39] = offset[9].c[3];
		bufferBoneTrans[40] = offset[10].c[0]; bufferBoneTrans[41] = offset[10].c[1]; bufferBoneTrans[42] = offset[10].c[2]; bufferBoneTrans[43] = offset[10].c[3];
		bufferBoneTrans[44] = offset[11].c[0]; bufferBoneTrans[45] = offset[11].c[1]; bufferBoneTrans[46] = offset[11].c[2]; bufferBoneTrans[47] = offset[11].c[3];

		file.write(bufferBoneTrans, sizeof(float) * 12);

		delete bufferBoneTrans;
	}

	file.close();
}

void Utils::MdlcIO::mdlcModelToBuffer(const char* in, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, uint32_t& stride, std::vector<std::array<float, 4>>& weights, std::vector<std::array<unsigned int, 4>>& joints, std::vector<BoneDescription>& boneDescriptions) {
	std::ifstream file(in, std::ios::binary);

	char metaData[4];

	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int vertexSize = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	stride = vertexSize;

	vertexSize *= sizeof(float);
	vertexSize += 4 * sizeof(float) + 4 * sizeof(char);

	char* buffer = new char[vertexCount * vertexSize];
	file.read(buffer, vertexCount * vertexSize);

	Utils::UFloat ret[12];
	for (unsigned int i = 0; i < vertexCount * vertexSize; i = i + vertexSize) {
		
		ret[0].c[0] = buffer[i + 0]; ret[0].c[1] = buffer[i + 1]; ret[0].c[2] = buffer[i + 2];  ret[0].c[3] = buffer[i + 3];
		ret[1].c[0] = buffer[i + 4]; ret[1].c[1] = buffer[i + 5]; ret[1].c[2] = buffer[i + 6];  ret[1].c[3] = buffer[i + 7];
		ret[2].c[0] = buffer[i + 8]; ret[2].c[1] = buffer[i + 9]; ret[2].c[2] = buffer[i + 10]; ret[2].c[3] = buffer[i + 11];
		vertexBuffer.push_back(ret[0].flt); vertexBuffer.push_back(ret[1].flt); vertexBuffer.push_back(ret[2].flt);

		ret[0].c[0] = buffer[i + 12]; ret[0].c[1] = buffer[i + 13]; ret[0].c[2] = buffer[i + 14]; ret[0].c[3] = buffer[i + 15];
		ret[1].c[0] = buffer[i + 16]; ret[1].c[1] = buffer[i + 17]; ret[1].c[2] = buffer[i + 18]; ret[1].c[3] = buffer[i + 19];
		vertexBuffer.push_back(ret[0].flt); vertexBuffer.push_back(ret[1].flt);
		
		ret[0].c[0] = buffer[i + 20]; ret[0].c[1] = buffer[i + 21]; ret[0].c[2] = buffer[i + 22]; ret[0].c[3] = buffer[i + 23];
		ret[1].c[0] = buffer[i + 24]; ret[1].c[1] = buffer[i + 25]; ret[1].c[2] = buffer[i + 26]; ret[1].c[3] = buffer[i + 27];
		ret[2].c[0] = buffer[i + 28]; ret[2].c[1] = buffer[i + 29]; ret[2].c[2] = buffer[i + 30]; ret[2].c[3] = buffer[i + 31];
		vertexBuffer.push_back(ret[0].flt); vertexBuffer.push_back(ret[1].flt); vertexBuffer.push_back(ret[2].flt);

		ret[0].c[0] = buffer[i + 32]; ret[0].c[1] = buffer[i + 33]; ret[0].c[2] = buffer[i + 34]; ret[0].c[3] = buffer[i + 35];
		ret[1].c[0] = buffer[i + 36]; ret[1].c[1] = buffer[i + 37]; ret[1].c[2] = buffer[i + 38]; ret[1].c[3] = buffer[i + 39];
		ret[2].c[0] = buffer[i + 40]; ret[2].c[1] = buffer[i + 41]; ret[2].c[2] = buffer[i + 42]; ret[2].c[3] = buffer[i + 43];
		ret[3].c[0] = buffer[i + 44]; ret[3].c[1] = buffer[i + 45]; ret[3].c[2] = buffer[i + 46]; ret[3].c[3] = buffer[i + 47];
		weights.push_back({ ret[0].flt, ret[1].flt, ret[2].flt, ret[3].flt });

		joints.push_back({ static_cast<unsigned int>(buffer[i + 48]), static_cast<unsigned int>(buffer[i + 49]), static_cast<unsigned int>(buffer[i + 50]), static_cast<unsigned int>(buffer[i +51]) });
	}

	file.read(metaData, sizeof(unsigned int));
	unsigned int indexCount = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	indexBuffer.resize(indexCount);
	file.read(reinterpret_cast<char*>(indexBuffer.data()), indexCount * sizeof(unsigned int));

	file.read(metaData, sizeof(unsigned int));
	unsigned int numBones = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
	boneDescriptions.resize(numBones);

	for (unsigned int i = 0; i < numBones; ++i) {
		BoneDescription& boneDescription = boneDescriptions[i];

		std::string boneName;
		while (true) {
			file.read(metaData, sizeof(char));
			if (!metaData[0])
				break;
			else
				boneName += metaData[0];
		}
		boneDescription.name = boneName;

		file.read(metaData, sizeof(unsigned int));
		boneDescription.parentIndex = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

		char* bufferBoneTrans = new char[48];

		file.read(bufferBoneTrans, sizeof(float) * 3);

		ret[0].c[0] = bufferBoneTrans[0]; ret[0].c[1] = bufferBoneTrans[1]; ret[0].c[2] = bufferBoneTrans[2];  ret[0].c[3] = bufferBoneTrans[3];
		ret[1].c[0] = bufferBoneTrans[4]; ret[1].c[1] = bufferBoneTrans[5]; ret[1].c[2] = bufferBoneTrans[6];  ret[1].c[3] = bufferBoneTrans[7];
		ret[2].c[0] = bufferBoneTrans[8]; ret[2].c[1] = bufferBoneTrans[9]; ret[2].c[2] = bufferBoneTrans[10]; ret[2].c[3] = bufferBoneTrans[11];
		boneDescription.initialPosition.set(ret[0].flt, ret[1].flt, ret[2].flt);

		file.read(bufferBoneTrans, sizeof(float) * 4);
		ret[0].c[0] = bufferBoneTrans[0]; ret[0].c[1]  = bufferBoneTrans[1]; ret[0].c[2]  = bufferBoneTrans[2];  ret[0].c[3]  = bufferBoneTrans[3];
		ret[1].c[0] = bufferBoneTrans[4]; ret[1].c[1]  = bufferBoneTrans[5]; ret[1].c[2]  = bufferBoneTrans[6];  ret[1].c[3]  = bufferBoneTrans[7];
		ret[2].c[0] = bufferBoneTrans[8]; ret[2].c[1]  = bufferBoneTrans[9]; ret[2].c[2]  = bufferBoneTrans[10]; ret[2].c[3] = bufferBoneTrans[11];
		ret[3].c[0] = bufferBoneTrans[12]; ret[3].c[1] = bufferBoneTrans[13]; ret[3].c[2] = bufferBoneTrans[14]; ret[3].c[3] = bufferBoneTrans[15];
		boneDescription.initialRotation.set(ret[1].flt, ret[2].flt, ret[3].flt, ret[0].flt);

		file.read(bufferBoneTrans, sizeof(float) * 3);
		ret[0].c[0] = bufferBoneTrans[0]; ret[0].c[1] = bufferBoneTrans[1]; ret[0].c[2] = bufferBoneTrans[2];  ret[0].c[3] = bufferBoneTrans[3];
		ret[1].c[0] = bufferBoneTrans[4]; ret[1].c[1] = bufferBoneTrans[5]; ret[1].c[2] = bufferBoneTrans[6];  ret[1].c[3] = bufferBoneTrans[7];
		ret[2].c[0] = bufferBoneTrans[8]; ret[2].c[1] = bufferBoneTrans[9]; ret[2].c[2] = bufferBoneTrans[10]; ret[2].c[3] = bufferBoneTrans[11];
		boneDescription.initialScale.set(ret[0].flt, ret[1].flt, ret[2].flt);

		file.read(bufferBoneTrans, sizeof(float) * 12);
		ret[0].c[0] = bufferBoneTrans[0];  ret[0].c[1] = bufferBoneTrans[1];  ret[0].c[2] = bufferBoneTrans[2];  ret[0].c[3] = bufferBoneTrans[3];
		ret[1].c[0] = bufferBoneTrans[4];  ret[1].c[1] = bufferBoneTrans[5];  ret[1].c[2] = bufferBoneTrans[6];  ret[1].c[3] = bufferBoneTrans[7];
		ret[2].c[0] = bufferBoneTrans[8];  ret[2].c[1] = bufferBoneTrans[9];  ret[2].c[2] = bufferBoneTrans[10]; ret[2].c[3] = bufferBoneTrans[11];
		ret[3].c[0] = bufferBoneTrans[12]; ret[3].c[1] = bufferBoneTrans[13]; ret[3].c[2] = bufferBoneTrans[14]; ret[3].c[3] = bufferBoneTrans[15];

		ret[4].c[0] = bufferBoneTrans[16]; ret[4].c[1] = bufferBoneTrans[17]; ret[4].c[2] = bufferBoneTrans[18]; ret[4].c[3] = bufferBoneTrans[19];
		ret[5].c[0] = bufferBoneTrans[20]; ret[5].c[1] = bufferBoneTrans[21]; ret[5].c[2] = bufferBoneTrans[22]; ret[5].c[3] = bufferBoneTrans[23];
		ret[6].c[0] = bufferBoneTrans[24]; ret[6].c[1] = bufferBoneTrans[25]; ret[6].c[2] = bufferBoneTrans[26]; ret[6].c[3] = bufferBoneTrans[27];
		ret[7].c[0] = bufferBoneTrans[28]; ret[7].c[1] = bufferBoneTrans[29]; ret[7].c[2] = bufferBoneTrans[30]; ret[7].c[3] = bufferBoneTrans[31];

		ret[8].c[0]  = bufferBoneTrans[32]; ret[8].c[1]  = bufferBoneTrans[33];  ret[8].c[2] = bufferBoneTrans[34];  ret[8].c[3] = bufferBoneTrans[35];
		ret[9].c[0]  = bufferBoneTrans[36]; ret[9].c[1]  = bufferBoneTrans[37];  ret[9].c[2] = bufferBoneTrans[38];  ret[9].c[3] = bufferBoneTrans[39];
		ret[10].c[0] = bufferBoneTrans[40]; ret[10].c[1] = bufferBoneTrans[41]; ret[10].c[2] = bufferBoneTrans[42]; ret[10].c[3] = bufferBoneTrans[43];
		ret[11].c[0] = bufferBoneTrans[44]; ret[11].c[1] = bufferBoneTrans[45]; ret[11].c[2] = bufferBoneTrans[46]; ret[11].c[3] = bufferBoneTrans[47];

		boneDescription.offsetMatrix.set(ret[0].flt, ret[4].flt, ret[8].flt, 0.0f,
                                         ret[1].flt, ret[5].flt, ret[9].flt, 0.0f,
                                         ret[2].flt, ret[6].flt, ret[10].flt, 0.0f,
                                         ret[3].flt, ret[7].flt, ret[11].flt, 1.0f);

		delete bufferBoneTrans;
	}

	file.close();
}