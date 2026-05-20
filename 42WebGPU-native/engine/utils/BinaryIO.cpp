#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
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

		std::array<unsigned char, 4> joint = { static_cast<unsigned char>(joints[i][0]), static_cast<unsigned char>(joints[i][1]),  static_cast<unsigned char>(joints[i][2]), static_cast<unsigned char>(joints[i][3]) };

		buffer[0] = joint[0]; buffer[1] = joint[1]; buffer[2] = joint[2]; buffer[3] = joint[3];
		file.write(buffer, sizeof(unsigned char) * 4);
	}
	delete[] buffer;

	unsigned int indexCount = static_cast<unsigned int>(indexBuffer.size());
	file.write(reinterpret_cast<const char*>(&indexCount), sizeof(unsigned int));
	file.write(reinterpret_cast<const char*>(indexBuffer.data()), indexCount * sizeof(unsigned int));

	unsigned int numBones = static_cast<unsigned int>(boneDescriptions.size());
	file.write(reinterpret_cast<const char*>(&numBones), sizeof(unsigned int));

	Utils::UFloat ret[12];
	for (size_t i = 0; i < boneDescriptions.size(); ++i) {
		const BoneDescription& boneDescription = boneDescriptions[i];
		file.write(&boneDescription.name[0], boneDescription.name.size() + 1);

		unsigned int parentIndex = boneDescription.parentIndex;
		file.write(reinterpret_cast<const char*>(&parentIndex), sizeof(unsigned int));

		char* bufferBoneTrans = new char[48];

		ret[0].flt = boneDescription.initialPosition[0]; ret[1].flt = boneDescription.initialPosition[1]; ret[2].flt = boneDescription.initialPosition[2];
		bufferBoneTrans[0] = ret[0].c[0]; bufferBoneTrans[1] = ret[0].c[1]; bufferBoneTrans[2]  = ret[0].c[2]; bufferBoneTrans[3]  = ret[0].c[3];
		bufferBoneTrans[4] = ret[1].c[0]; bufferBoneTrans[5] = ret[1].c[1]; bufferBoneTrans[6]  = ret[1].c[2]; bufferBoneTrans[7]  = ret[1].c[3];
		bufferBoneTrans[8] = ret[2].c[0]; bufferBoneTrans[9] = ret[2].c[1]; bufferBoneTrans[10] = ret[2].c[2]; bufferBoneTrans[11] = ret[2].c[3];
		file.write(bufferBoneTrans, sizeof(float) * 3);

		ret[0].flt = boneDescription.initialRotation[0]; ret[1].flt = boneDescription.initialRotation[1]; ret[2].flt = boneDescription.initialRotation[2]; ret[3].flt = boneDescription.initialRotation[3];
		bufferBoneTrans[0]  = ret[3].c[0]; bufferBoneTrans[1]  = ret[3].c[1]; bufferBoneTrans[2]  = ret[3].c[2]; bufferBoneTrans[3]  = ret[3].c[3];
		bufferBoneTrans[4]  = ret[0].c[0]; bufferBoneTrans[5]  = ret[0].c[1]; bufferBoneTrans[6]  = ret[0].c[2]; bufferBoneTrans[7]  = ret[0].c[3];
		bufferBoneTrans[8]  = ret[1].c[0]; bufferBoneTrans[9]  = ret[1].c[1]; bufferBoneTrans[10] = ret[1].c[2]; bufferBoneTrans[11] = ret[1].c[3];
		bufferBoneTrans[12] = ret[2].c[0]; bufferBoneTrans[13] = ret[2].c[1]; bufferBoneTrans[14] = ret[2].c[2]; bufferBoneTrans[15] = ret[2].c[3];

		file.write(bufferBoneTrans, sizeof(float) * 4);

		ret[0].flt = boneDescription.initialScale[0]; ret[1].flt = boneDescription.initialScale[1]; ret[2].flt = boneDescription.initialScale[2];
		bufferBoneTrans[0] = ret[0].c[0]; bufferBoneTrans[1] = ret[0].c[1]; bufferBoneTrans[2]  = ret[0].c[2]; bufferBoneTrans[3]  = ret[0].c[3];
		bufferBoneTrans[4] = ret[1].c[0]; bufferBoneTrans[5] = ret[1].c[1]; bufferBoneTrans[6]  = ret[1].c[2]; bufferBoneTrans[7]  = ret[1].c[3];
		bufferBoneTrans[8] = ret[2].c[0]; bufferBoneTrans[9] = ret[2].c[1]; bufferBoneTrans[10] = ret[2].c[2]; bufferBoneTrans[11] = ret[2].c[3];
		file.write(bufferBoneTrans, sizeof(float) * 3);

		ret[0].flt = boneDescription.offsetMatrix[0][0]; ret[4].flt = boneDescription.offsetMatrix[0][1]; ret[8].flt  = boneDescription.offsetMatrix[0][2];
		ret[1].flt = boneDescription.offsetMatrix[1][0]; ret[5].flt = boneDescription.offsetMatrix[1][1]; ret[9].flt  = boneDescription.offsetMatrix[1][2];
		ret[2].flt = boneDescription.offsetMatrix[2][0]; ret[6].flt = boneDescription.offsetMatrix[2][1]; ret[10].flt = boneDescription.offsetMatrix[2][2];
		ret[3].flt = boneDescription.offsetMatrix[3][0]; ret[7].flt = boneDescription.offsetMatrix[3][1]; ret[11].flt = boneDescription.offsetMatrix[3][2];

		bufferBoneTrans[0] = ret[0].c[0]; bufferBoneTrans[1] = ret[0].c[1]; bufferBoneTrans[2]  = ret[0].c[2]; bufferBoneTrans[3]  = ret[0].c[3];
		bufferBoneTrans[4] = ret[1].c[0]; bufferBoneTrans[5] = ret[1].c[1]; bufferBoneTrans[6]  = ret[1].c[2]; bufferBoneTrans[7]  = ret[1].c[3];
		bufferBoneTrans[8] = ret[2].c[0]; bufferBoneTrans[9] = ret[2].c[1]; bufferBoneTrans[10] = ret[2].c[2]; bufferBoneTrans[11] = ret[2].c[3];

		bufferBoneTrans[12] = ret[3].c[0]; bufferBoneTrans[13] = ret[3].c[1]; bufferBoneTrans[14] = ret[3].c[2]; bufferBoneTrans[15] = ret[3].c[3];
		bufferBoneTrans[16] = ret[4].c[0]; bufferBoneTrans[17] = ret[4].c[1]; bufferBoneTrans[18] = ret[4].c[2]; bufferBoneTrans[19] = ret[4].c[3];
		bufferBoneTrans[20] = ret[5].c[0]; bufferBoneTrans[21] = ret[5].c[1]; bufferBoneTrans[22] = ret[5].c[2]; bufferBoneTrans[23] = ret[5].c[3];

		bufferBoneTrans[24] = ret[6].c[0]; bufferBoneTrans[25] = ret[6].c[1]; bufferBoneTrans[26] = ret[6].c[2]; bufferBoneTrans[27] = ret[6].c[3];
		bufferBoneTrans[28] = ret[7].c[0]; bufferBoneTrans[29] = ret[7].c[1]; bufferBoneTrans[30] = ret[7].c[2]; bufferBoneTrans[31] = ret[7].c[3];
		bufferBoneTrans[32] = ret[8].c[0]; bufferBoneTrans[33] = ret[8].c[1]; bufferBoneTrans[34] = ret[8].c[2]; bufferBoneTrans[35] = ret[8].c[3];

		bufferBoneTrans[36] = ret[9].c[0];  bufferBoneTrans[37] = ret[9].c[1];  bufferBoneTrans[38] = ret[9].c[2];  bufferBoneTrans[39] = ret[9].c[3];
		bufferBoneTrans[40] = ret[10].c[0]; bufferBoneTrans[41] = ret[10].c[1]; bufferBoneTrans[42] = ret[10].c[2]; bufferBoneTrans[43] = ret[10].c[3];
		bufferBoneTrans[44] = ret[11].c[0]; bufferBoneTrans[45] = ret[11].c[1]; bufferBoneTrans[46] = ret[11].c[2]; bufferBoneTrans[47] = ret[11].c[3];

		file.write(bufferBoneTrans, sizeof(float) * 12);

		delete[] bufferBoneTrans;
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

		delete[] bufferBoneTrans;
	}

	file.close();
}

void  Utils::MdlcIO::animationToAnic(const char* out, const std::string& animationName, const float length, const std::vector<AnimationTrack>& animationTracks) {
	std::ofstream file(out, std::ios::binary);
	file.write(&animationName[0], animationName.size() + 1);

	file.write(reinterpret_cast<const char*>(&length), sizeof(float));

	unsigned int numTracks = static_cast<unsigned int>(animationTracks.size());
	file.write(reinterpret_cast<const char*>(&numTracks), sizeof(unsigned int));

	for (unsigned int i = 0; i < numTracks; ++i) {
		const AnimationTrack& animationTrack = animationTracks[i];
		file.write(&animationTrack.m_name[0], animationTrack.m_name.size() + 1);

		unsigned char channelMask = 7;
		file.write(reinterpret_cast<const char*>(&channelMask), sizeof(unsigned char));

		unsigned int numKeyFrames = static_cast<unsigned int>(animationTrack.m_keyFrames.size());
		file.write(reinterpret_cast<const char*>(&numKeyFrames), sizeof(unsigned int));

		Utils::UFloat ret[4];
		char* buffer = new char[16];
		for (size_t j = 0; j < numKeyFrames; ++j) {
			const AnimationKeyFrame& keyFrame = animationTrack.m_keyFrames[j];
			file.write(reinterpret_cast<const char*>(&keyFrame.m_time), sizeof(float));
			
			ret[0].flt = keyFrame.m_position[0]; ret[1].flt = keyFrame.m_position[1]; ret[2].flt = keyFrame.m_position[2];
			buffer[0] = ret[0].c[0]; buffer[1] = ret[0].c[1]; buffer[2]  = ret[0].c[2]; buffer[3]  = ret[0].c[3];
			buffer[4] = ret[1].c[0]; buffer[5] = ret[1].c[1]; buffer[6]  = ret[1].c[2]; buffer[7]  = ret[1].c[3];
			buffer[8] = ret[2].c[0]; buffer[9] = ret[2].c[1]; buffer[10] = ret[2].c[2]; buffer[11] = ret[2].c[3];
			file.write(buffer, sizeof(float) * 3);

			ret[0].flt = keyFrame.m_rotation[0]; ret[1].flt = keyFrame.m_rotation[1]; ret[2].flt = keyFrame.m_rotation[2]; ret[3].flt = keyFrame.m_rotation[3];
			buffer[0]  = ret[0].c[0]; buffer[1]  = ret[0].c[1]; buffer[2]  = ret[0].c[2]; buffer[3]  = ret[0].c[3];
			buffer[4]  = ret[1].c[0]; buffer[5]  = ret[1].c[1]; buffer[6] = ret[1].c[2]; buffer[7] = ret[1].c[3];
			buffer[8]  = ret[2].c[0]; buffer[9] = ret[2].c[1]; buffer[10] = ret[2].c[2]; buffer[11] = ret[2].c[3];
			buffer[12] = ret[3].c[0]; buffer[13] = ret[3].c[1]; buffer[14] = ret[3].c[2]; buffer[15] = ret[3].c[3];
			file.write(buffer, sizeof(float) * 4);

			ret[0].flt = keyFrame.m_scale[0]; ret[1].flt = keyFrame.m_scale[1]; ret[2].flt = keyFrame.m_scale[2];
			buffer[0] = ret[0].c[0]; buffer[1] = ret[0].c[1]; buffer[2]  = ret[0].c[2]; buffer[3]  = ret[0].c[3];
			buffer[4] = ret[1].c[0]; buffer[5] = ret[1].c[1]; buffer[6]  = ret[1].c[2]; buffer[7]  = ret[1].c[3];
			buffer[8] = ret[2].c[0]; buffer[9] = ret[2].c[1]; buffer[10] = ret[2].c[2]; buffer[11] = ret[2].c[3];
			file.write(buffer, sizeof(float) * 3);
		}
		delete[] buffer;
	}
}

void Utils::MdlcIO::anicToBuffer(const char* in, std::string& animationName, float& length, std::map<std::string, AnimationTrack>& animationTracks) {
	std::ifstream file(in, std::ios::binary);

	char metaData[4];

	while (true) {
		file.read(metaData, sizeof(char));
		if (!metaData[0])
			break;
		else
			animationName += metaData[0];
	}

	file.read(metaData, sizeof(float));
	length = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	file.read(metaData, sizeof(unsigned int));
	unsigned int numTracks = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);

	for (unsigned int i = 0; i < numTracks; ++i) {

		std::string trackName;
		while (true) {
			file.read(metaData, sizeof(char));
			if (!metaData[0])
				break;
			else
				trackName += metaData[0];
		}

		AnimationTrack& newTrack = animationTracks[trackName];
		newTrack.m_name = trackName;

		file.read(metaData, sizeof(unsigned char));
		newTrack.m_channelMask = metaData[0];

		file.read(metaData, sizeof(unsigned int));
		unsigned int numKeyFrames = Utils::bytesToUIntLE(metaData[0], metaData[1], metaData[2], metaData[3]);
		
		newTrack.m_keyFrames.resize(numKeyFrames);

		Utils::UFloat ret[4];
		char* buffer = new char[16];

		for (size_t j = 0; j < numKeyFrames; ++j) {
			AnimationKeyFrame& newKeyFrame = newTrack.m_keyFrames[j];

			file.read(metaData, sizeof(float));
			newKeyFrame.m_time = Utils::bytesToFloatLE(metaData[0], metaData[1], metaData[2], metaData[3]);

			if (newTrack.m_channelMask & CHANNEL_POSITION) {
				file.read(buffer, 12);
				ret[0].c[0] = buffer[0]; ret[0].c[1] = buffer[1]; ret[0].c[2] = buffer[2];  ret[0].c[3] = buffer[3];
				ret[1].c[0] = buffer[4]; ret[1].c[1] = buffer[5]; ret[1].c[2] = buffer[6];  ret[1].c[3] = buffer[7];
				ret[2].c[0] = buffer[8]; ret[2].c[1] = buffer[9]; ret[2].c[2] = buffer[10]; ret[2].c[3] = buffer[11];
				newKeyFrame.m_position.set(ret[0].flt, ret[1].flt, ret[2].flt);
			}

			if (newTrack.m_channelMask & CHANNEL_ROTATION) {
				file.read(buffer, 16);		
				ret[0].c[0] = buffer[0];  ret[0].c[1] = buffer[1];  ret[0].c[2] = buffer[2];  ret[0].c[3] = buffer[3];
				ret[1].c[0] = buffer[4];  ret[1].c[1] = buffer[5];  ret[1].c[2] = buffer[6];  ret[1].c[3] = buffer[7];
				ret[2].c[0] = buffer[8];  ret[2].c[1] = buffer[9];  ret[2].c[2] = buffer[10]; ret[2].c[3] = buffer[11];
				ret[3].c[0] = buffer[12]; ret[3].c[1] = buffer[13]; ret[3].c[2] = buffer[14]; ret[3].c[3] = buffer[15];
				newKeyFrame.m_rotation.set(ret[0].flt, ret[1].flt, ret[2].flt, ret[3].flt);
			}

			if (newTrack.m_channelMask & CHANNEL_SCALE) {
				file.read(buffer, 12);			
				ret[0].c[0] = buffer[0]; ret[0].c[1] = buffer[1]; ret[0].c[2] = buffer[2]; ret[0].c[3] = buffer[3];
				ret[1].c[0] = buffer[4]; ret[1].c[1] = buffer[5]; ret[1].c[2] = buffer[6]; ret[1].c[3] = buffer[7];
				ret[2].c[0] = buffer[8]; ret[2].c[1] = buffer[9]; ret[2].c[2] = buffer[10]; ret[2].c[3] = buffer[11];
				newKeyFrame.m_scale.set(ret[0].flt, ret[1].flt, ret[2].flt);
			}		
		}
		delete[] buffer;
	}
	file.close();
}

void Utils::JsonIO::jsonToObj(const char* path, const char* outFileObj) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open()) {
		std::cout << "Could not open file: " << std::string(path) << std::endl;
	}

	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<short, 3>> faces;

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);
	int tmp = 0;
	for (rapidjson::Value::ConstValueIterator position = doc["positions"].GetArray().Begin(); position != doc["positions"].GetArray().End(); ++position) {		
		rapidjson::Value::ConstValueIterator coord = position->Begin();
		float x = coord->GetFloat();
		coord++;
		float y = coord->GetFloat();
		coord++;
		float z = coord->GetFloat();
		positions.push_back({ x, y, z});
	}

	for (rapidjson::Value::ConstValueIterator face = doc["cells"].GetArray().Begin(); face != doc["cells"].GetArray().End(); ++face) {
		rapidjson::Value::ConstValueIterator coord = face->Begin();
		short f0 = coord->GetInt();
		coord++;
		short f1 = coord->GetInt();
		coord++;
		short f2 = coord->GetInt();
		faces.push_back({ f0, f1, f2 });
	}
	file.close();

	std::ofstream fileOut;
	fileOut << std::setprecision(6) << std::fixed;
	fileOut.open(outFileObj);
	fileOut << "# OBJ file\n";

	for (int i = 0; i < positions.size(); i++) {
		fileOut << "v " << positions[i][0] << " " << positions[i][1] << " " << positions[i][2] << std::endl;
	}

	for (int i = 0; i < faces.size(); i++) {
		fileOut << "f " << faces[i][0] + 1  << " " << faces[i][1] + 1 << " " << faces[i][2] + 1 << std::endl;
	}

	fileOut.close();
}