#include "Model.h"

void Model::GenerateColors(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int& stride, unsigned int startIndex, unsigned int endIndex, ModelColor modelColor) {
	if (stride < 8)
		return;

	std::vector<float> tmpVertex;
	for (int i = 0; i < vertexBuffer.size(); i++) {
		tmpVertex.push_back(vertexBuffer[i]);

		if ((i + 1) % stride == 0) {
			if (modelColor == ModelColor::MC_POSITION) {
				tmpVertex.push_back(vertexBuffer[i - stride + 1]);
				tmpVertex.push_back(vertexBuffer[i - stride + 2]);
				tmpVertex.push_back(vertexBuffer[i - stride + 3]);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_WHITE) {
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_RED) {
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_GREEN) {
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_BLUE) {
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
				tmpVertex.push_back(1.0f);
			}
			else if (modelColor == ModelColor::MC_BLACK) {
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(0.0f);
				tmpVertex.push_back(1.0f);
			}
		}
	}

	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	copy(tmpVertex.begin(), tmpVertex.end(), back_inserter(vertexBuffer));
	tmpVertex.clear();
	stride += 4;
}

void Model::PackBuffer(std::vector<float>& vertexBuffer, unsigned int stride) {
	std::vector<float> vertexBufferNew;
	unsigned int strideNew = 0u;

	if (stride == 3) {
		vertexBufferNew.resize(vertexBuffer.size() / 3 * 4);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 4, k = k + 3) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;
		}
		strideNew = 4;
	}

	if (stride == 5) {
		vertexBufferNew.resize(vertexBuffer.size() / 5 * 8);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 8, k = k + 5) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;
		}
		strideNew = 8;
	}

	if (stride == 8) {
		vertexBufferNew.resize(vertexBuffer.size() / 8 * 12);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 12, k = k + 8) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;

			vertexBufferNew[i + 8] = vertexBuffer[k + 5];
			vertexBufferNew[i + 9] = vertexBuffer[k + 6];
			vertexBufferNew[i + 10] = vertexBuffer[k + 7];
			vertexBufferNew[i + 11] = 0.0f;
		}
		strideNew = 12;
	}

	if (stride == 14) {
		vertexBufferNew.resize(vertexBuffer.size() / 14 * 20);

		for (unsigned int i = 0, k = 0; i < vertexBufferNew.size(); i = i + 16, k = k + 20) {
			vertexBufferNew[i] = vertexBuffer[k];
			vertexBufferNew[i + 1] = vertexBuffer[k + 1];
			vertexBufferNew[i + 2] = vertexBuffer[k + 2];
			vertexBufferNew[i + 3] = 0.0f;

			vertexBufferNew[i + 4] = vertexBuffer[k + 3];
			vertexBufferNew[i + 5] = vertexBuffer[k + 4];
			vertexBufferNew[i + 6] = 0.0f;
			vertexBufferNew[i + 7] = 0.0f;

			vertexBufferNew[i + 8] = vertexBuffer[k + 5];
			vertexBufferNew[i + 9] = vertexBuffer[k + 6];
			vertexBufferNew[i + 10] = vertexBuffer[k + 7];
			vertexBufferNew[i + 11] = 0.0f;

			vertexBufferNew[i + 12] = vertexBuffer[k + 8];
			vertexBufferNew[i + 13] = vertexBuffer[k + 9];
			vertexBufferNew[i + 14] = vertexBuffer[k + 10];
			vertexBufferNew[i + 15] = 0.0f;

			vertexBufferNew[i + 16] = vertexBuffer[k + 11];
			vertexBufferNew[i + 17] = vertexBuffer[k + 12];
			vertexBufferNew[i + 18] = vertexBuffer[k + 13];
			vertexBufferNew[i + 19] = 0.0f;
		}
		strideNew = 20;
	}
	stride = strideNew;
	vertexBuffer.clear();
	vertexBuffer.shrink_to_fit();
	vertexBuffer.insert(vertexBuffer.end(), vertexBufferNew.begin(), vertexBufferNew.end());
}